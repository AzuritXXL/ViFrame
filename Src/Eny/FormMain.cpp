#include "stdafx.h"
// Std
#include <set>
// Qt
#include <QInputDialog>
#include <QFileDialog.h>
#include <QXmlStreamWriter>
// Generated
#include "ui_TabAssembly.h"
// BaseLib
#include "Module.hpp"
#include "BaseProperty.hpp"
#include "Property.hpp"
// ...
#include "FormMain.h"
#include "AssemblyScene.h"
#include "AssemblyItemModule.h"
#include "AssemblyItemConnection.h"
#include "OutputTab.h"

FormMain::FormMain( DataModels::Library::LibraryModel* libraryModel, 
					Logic::LibraryStorage& libraryStorage, 
					QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags), mLibraryStorage(libraryStorage)
{
	ui.setupUi(this);

	// add module with assembly
	mTabAssembly = new Assembly::Tab(libraryModel, libraryStorage, this);
	ui.Tabs->addTab(mTabAssembly, "Assembly");

	// ui.Tabs
	ui.Tabs->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui.Tabs, SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(slTabContextMenu(const QPoint&)));

	// slots
	connect(mTabAssembly->GetScene(), SIGNAL(siRunModule(Assembly::ItemModule*)),  this, SLOT(slRunModule(Assembly::ItemModule*)) );
	connect(ui.actLoad, SIGNAL(triggered()), this, SLOT(slAssemblyLoad()));
	connect(ui.actSave, SIGNAL(triggered()), this, SLOT(slAssemblySave()));

	// WorkThread
	connect(&mWorkThread, SIGNAL(finished()), this, SLOT(slWorkDone()), Qt::QueuedConnection);
	connect(&mWorkThread, SIGNAL(siAddMessage(const DataModels::Message::Item&)), mTabAssembly->GetMessageModel(), SLOT(slAddMessage(const DataModels::Message::Item&)), Qt::QueuedConnection);

	// actions
	actClear = new QAction("Clear content", this);
	connect(actClear, SIGNAL(triggered()), this, SLOT(slTabClear()));

	actRename = new QAction("Rename", this);
	connect(actRename, SIGNAL(triggered()), this, SLOT(slTabRename()));

	actClose = new QAction("Close", this);
	connect(actClose, SIGNAL(triggered()), this, SLOT(slTabClose()));

}

FormMain::~FormMain()
{

}

bool FormMain::AssemblySave(const QString& path, std::stringstream& ss)
{
	QFile outputFile(path);
	if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		ss << "Save failed, can't open output file.";
        return false;
	}
	// save to xml
	QXmlStreamWriter xml(&outputFile);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
	xml.writeDTD("<!DOCTYPE Eny:Assembly>");
    xml.writeStartElement("assembly");
	
	Assembly::Scene::ModulesType::const_iterator iter = mTabAssembly->GetScene()->GetModules().cbegin();
	Assembly::Scene::ModulesType::const_iterator iterEnd = mTabAssembly->GetScene()->GetModules().cend();
	for(;iter != iterEnd; ++iter)
	{
		xml.writeStartElement("module");
		// basic information
		xml.writeAttribute("id", iter->second->GetId());
		xml.writeAttribute("libId", iter->second->GetHolder().GetLibrary()->GetId().c_str());
		xml.writeAttribute("x", QString::number(iter->second->pos().x()));
		xml.writeAttribute("y", QString::number(iter->second->pos().y()));
		// values of properties
		BaseLib::iModule* module = iter->second->GetModule();
		BaseLib::iModule::PropertyType::const_iterator propertyIter = module->GetProperties().cbegin();
		BaseLib::iModule::PropertyType::const_iterator propertyIterEnd = module->GetProperties().cend();
		
		for(size_t index = 0;propertyIter != propertyIterEnd; ++propertyIter, ++index)
		{
			xml.writeStartElement("property");
			xml.writeAttribute("index", QString::number(index));

			auto type = (*propertyIter)->GetType();

			// save propoerty according to his type
			if ((type & BaseLib::Properties::BaseProperty::Bool) == BaseLib::Properties::BaseProperty::Bool)
			{
				BaseLib::Properties::Property<bool>* prop
					= dynamic_cast<BaseLib::Properties::Property<bool>*>((*propertyIter));

				xml.writeAttribute("type", "bool");
				if (prop->GetProperty())
				{
					xml.writeCharacters("true");
				}
				else
				{
					xml.writeCharacters("false");
				}
			} 
			else if ((type & BaseLib::Properties::BaseProperty::Double) == BaseLib::Properties::BaseProperty::Double)
			{
				BaseLib::Properties::Property<double>* prop
					= dynamic_cast<BaseLib::Properties::Property<double>*>((*propertyIter));
				xml.writeAttribute("type", "double");
				xml.writeCharacters(QString::number(prop->GetProperty()));
			}
			else if ((type & BaseLib::Properties::BaseProperty::Int) == BaseLib::Properties::BaseProperty::Int)
			{
				BaseLib::Properties::Property<int>* prop
					= dynamic_cast<BaseLib::Properties::Property<int>*>((*propertyIter));

				xml.writeAttribute("type", "int");
				xml.writeCharacters(QString::number(prop->GetProperty()));
			}
			else if ((type & BaseLib::Properties::BaseProperty::String) == BaseLib::Properties::BaseProperty::String)
			{
				BaseLib::Properties::Property<std::string>* prop
					= dynamic_cast<BaseLib::Properties::Property<std::string>*>((*propertyIter));

				xml.writeAttribute("type", "string");
				xml.writeCharacters(prop->GetProperty().c_str());
			}

			xml.writeEndElement(); // properties
		}
		xml.writeEndElement(); // module
	}
	// connections
	xml.writeStartElement("connections");
	iter = mTabAssembly->GetScene()->GetModules().cbegin();
	for(;iter != iterEnd; ++iter)
	{
		BaseLib::iModule* module = iter->second->GetModule();
		size_t socketSize = module->GetInputSockets().size();
		for (size_t i = 0; i < socketSize; ++i)
		{
			BaseLib::iModule* source = module->GetSocket(i);
			if (source == nullptr)
			{	// no connection
			}
			else
			{
				xml.writeStartElement("connection");
				xml.writeAttribute("source", source->GetId().c_str());
				xml.writeAttribute("target", module->GetId().c_str());
				xml.writeAttribute("number", QString::number(i));
				xml.writeEndElement(); // connection
			}
		}
	}
	xml.writeEndElement(); // connections
	// mCounter
	xml.writeStartElement("counter");
	xml.writeAttribute("value", QString::number(mTabAssembly->GetScene()->GetCounter()));
	xml.writeEndElement(); // counter

    xml.writeEndElement(); // assembly
    xml.writeEndDocument();

	outputFile.close();

	return true;
}

bool FormMain::AssemblyLoad(const QString& path, std::stringstream& ss)
{	
	// we load modules here
	size_t counter = 0;
	std::map<std::string, Assembly::ItemModule*> modules;

	// open file 
	QFile inputFile(path);	
	if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		ss << "Load failed, can't open input file.";
        return false;
	}
	QXmlStreamReader xml(&inputFile);

	// try to read start element
	if (!xml.readNextStartElement())
	{
		inputFile.close();
		ss << "Load failed, can't parse xml file.";
		return false;
	}

	if (xml.name() != "assembly")
	{
		inputFile.close();
		ss << "Load failed, wrong xml format.";
		return false;
	}

	// Store indexes to properties that failed through SetPropoertyLoad in first time
	// store module name, and propoerties ie. index and value
	std::map<BaseLib::iModule*, std::vector<std::pair<int, QVariant> > > unInitializedPropertiesIndexes;

	// used to store modules, that are no source for anyone
	std::set<BaseLib::iModule*> noSources;

	bool troubleOccured = false;
	BaseLib::iModule* activeModule = nullptr;
	while (!xml.atEnd()) 
	{
		xml.readNext();
		if (xml.name() == "module" && xml.isStartElement())
		{
			Logic::Library* lib = mLibraryStorage.GetLibrary(xml.attributes().value("libId").toString().toStdString());
			if (lib == nullptr)
			{					
				ss << "Can't find library " << xml.attributes().value("libId").toString().toStdString() << " to load module " << xml.attributes().value("id").toString().toStdString() << std::endl;
				troubleOccured = true;
			}
			else
			{
				// create module
				Logic::ModuleHolder* holder = lib->Create(xml.attributes().value("id").toString().toStdString());
				activeModule = holder->GetModule();
				// create graph representation
				Assembly::ItemModule* itemModule = new Assembly::ItemModule(holder, *mTabAssembly->GetMessageModel(), mTabAssembly->GetScene());
				itemModule->setPos(xml.attributes().value("x").toString().toInt(), xml.attributes().value("y").toString().toInt());
				modules[holder->GetId()] = itemModule;
				// add to noSources
				noSources.insert(activeModule);
			}
		}
		else if (xml.name() == "property" && xml.isStartElement())
		{	// module's property ( name, type, text )
			if (activeModule == nullptr)
			{	// no active module .. cant load data
				ss << "Property to non existing module detected." << std::endl;
				troubleOccured = true;
			}
			else
			{	// we have module, try to set property
				std::string typeName = xml.attributes().value("type").toString().toStdString();
				int index = xml.attributes().value("index").toString().toUInt();
				
				if (index < 0 || index >= activeModule->GetProperties().size())
				{	
					// index out of range
					ss << "Property index out of range." << std::endl;
					troubleOccured = true;
				}
				else
				{
					auto propertyType = activeModule->GetProperty(index)->GetType();

					if (typeName == "bool")
					{
						if ((propertyType & BaseLib::Properties::BaseProperty::Bool) == 
							BaseLib::Properties::BaseProperty::Bool)
						{
							BaseLib::Properties::Property<bool>* prop
								= dynamic_cast<BaseLib::Properties::Property<bool>*>(activeModule->GetProperty(index));
							bool value = false;
							if (xml.readElementText() == "true")
							{
								value = true;							
							}
							else
							{	// remain false
							}

							if (!prop->SetPropertyLoad(value))
							{	// setting failed
								unInitializedPropertiesIndexes[activeModule].push_back(std::pair<int, QVariant>(index, value) );
							}
						}
						else
						{	// ignore bad type ... 
							ss << "Wrong property type detected." << std::endl;
							troubleOccured = true;
						}
					}
					else if (typeName == "double")
					{
						if ((propertyType & BaseLib::Properties::BaseProperty::Double) == 
							BaseLib::Properties::BaseProperty::Double)
						{
							BaseLib::Properties::Property<double>* prop
								= dynamic_cast<BaseLib::Properties::Property<double>*>(activeModule->GetProperty(index));

							double value = xml.readElementText().toDouble();
							if (!prop->SetPropertyLoad(value))
							{	// setting failed
								unInitializedPropertiesIndexes[activeModule].push_back(std::pair<int, QVariant>(index, value) );
							}
						}
						else
						{	// ignore bad type ... 
							ss << "Wrong property type detected." << std::endl;
							troubleOccured = true;
						}
					}
					else if (typeName == "int")
					{
						if ((propertyType & BaseLib::Properties::BaseProperty::Int) == 
							BaseLib::Properties::BaseProperty::Int)
						{
							BaseLib::Properties::Property<int>* prop
								= dynamic_cast<BaseLib::Properties::Property<int>*>(activeModule->GetProperty(index));
							
							int value = xml.readElementText().toInt();
							if (!prop->SetPropertyLoad(value))
							{	// setting failed
								unInitializedPropertiesIndexes[activeModule].push_back(std::pair<int, QVariant>(index, value) );
							}
						}
						else
						{	// ignore bad type ... 
							ss << "Wrong property type detected." << std::endl;
							troubleOccured = true;
						}
					}
					else if (typeName == "string")
					{
						if ((propertyType & BaseLib::Properties::BaseProperty::String) == 
							BaseLib::Properties::BaseProperty::String)
						{
							BaseLib::Properties::Property<std::string>* prop
								= dynamic_cast<BaseLib::Properties::Property<std::string>*>(activeModule->GetProperty(index));
						
							std::string value = xml.readElementText().toStdString();
							if (!prop->SetPropertyLoad(value))
							{	// setting failed
								unInitializedPropertiesIndexes[activeModule].push_back(std::pair<int, QVariant>(index, value.c_str()) );
							}
						}
						else
						{	// ignore bad type ... 
							ss << "Wrong property type detected." << std::endl;
							troubleOccured = true;
						}
					}				
				}
			}
		}
		else if (xml.name() == "connection" && xml.isStartElement())
		{	// connection (source, target, number)
			try
			{
				std::string sourceId = xml.attributes().value("source").toString().toStdString();
				std::string targetId = xml.attributes().value("target").toString().toStdString();

				if ( modules.count(sourceId) == 1 && modules.count(targetId) == 1)
				{	// everything all right, we have sourca and target
					auto sourceItem = modules.at(sourceId);
					auto targetItem = modules.at(targetId);

					if ( sourceItem == nullptr || targetItem == nullptr)
					{	// we should not get to this .. 
						troubleOccured = true;
					}
					else
					{
						size_t socket = xml.attributes().value("number").toString().toInt();
						// add logic connection
						if (targetItem->GetModule()->ConnectPure(sourceItem->GetModule(), socket))
						{	// ok add graphics rep
							Assembly::ItemConnection* connection = new Assembly::ItemConnection(sourceItem, targetItem, socket, mTabAssembly->GetScene());
							sourceItem->AddConnection(connection);
							targetItem->AddConnection(connection);
							// remove source from noSources
							noSources.erase(sourceItem->GetModule());
						}
						else
						{	// error 
							troubleOccured = true;
						}
					}
				}
				else
				{	// invalid source or target it -> ignore connection
					ss << "Connection between: " << sourceId << " and " << targetId << "has not been established, becouse one of module can't be found." << std::endl;
					troubleOccured = true;
				}
			}
			catch(std::exception ex)
			{
				ss << "Exception occurred:" << ex.what() << std::endl;
				troubleOccured = true;
			}
		}
		else if (xml.name() == "counter" && xml.isStartElement())
		{ // counter (value)
			counter = xml.attributes().value("value").toString().toInt();
		}
	}
	
	// close source xml
	if (xml.hasError())
	{
		ss << "There was error when loading xml file." << std::endl;
		troubleOccured = true;
	}
	inputFile.close();

	// we now have to set unseted propoerties once more and call valid in proper order
	// se start with creating a topological order, which also check for cycles
	// in noSources we have root of inverted graph ( modules that are not sources )
	BaseLib::iModule::TopologicalOrderStateType topologicalStates;
	std::vector<BaseLib::iModule*> topologicalOrder;
	{	// create topological order
		auto iter = std::begin(noSources);
		auto iterEnd = std::end(noSources);
		for (;iter != iterEnd; ++iter)
		{
			if ((*iter)->CreateTopologyOrder(topologicalStates, topologicalOrder))
			{	// no cycles, continue .. 
			}
			else
			{	// cycle ! -> release data all data ( modules )

				auto iterModules = std::begin(modules);
				auto iterModulesEnd = std::end(modules);
				for (;iterModules != iterModulesEnd; ++iterModules)
				{
					delete (*iterModules).second;
				}
				modules.clear();
				
				// scene will be left empty

				ss << "Cycle in assembly detected !" << std::endl;
				return false;
			}
		}
	}

	// now we have topological order in topologicalOrder
	// so we start to load modules in topological order
	{
		auto iter = std::begin(topologicalOrder);
		auto iterEnd = std::end(topologicalOrder);
		for (;iter != iterEnd; ++iter)
		{
			BaseLib::iModule* activeModule = (*iter);
			if (unInitializedPropertiesIndexes.count(activeModule) == 0)
			{	// all property set .. 
			}
			else
			{
				std::string propertyLog; // used to storelog from SetProperty

				// report intention to load propoerty
				activeModule->PropertyLoadSecondWave();

				std::for_each(std::begin(unInitializedPropertiesIndexes[activeModule]), std::end(unInitializedPropertiesIndexes[activeModule]),
					[&] (std::pair<int, QVariant> item)
					{
						auto type = activeModule->GetProperty(item.first)->GetType();

						if ( (type & BaseLib::Properties::BaseProperty::Bool) == BaseLib::Properties::BaseProperty::Bool)
						{
							BaseLib::Properties::Property<bool>* prop
								= dynamic_cast<BaseLib::Properties::Property<bool>*>(activeModule->GetProperty(item.first));
							if (prop->SetProperty(item.second.toBool(), propertyLog))
							{	// all goes well
							}
							else
							{
								ss << "Setting property "<< prop->GetName() << " failed : " << propertyLog << std::endl;
								troubleOccured = true;
							}
						}
						else if ( (type & BaseLib::Properties::BaseProperty::Double) == BaseLib::Properties::BaseProperty::Double)
						{
							BaseLib::Properties::Property<double>* prop
								= dynamic_cast<BaseLib::Properties::Property<double>*>(activeModule->GetProperty(item.first));
							if (prop->SetProperty(item.second.toDouble(), propertyLog))
							{	// all goes well
							}
							else
							{
								ss << "Setting property "<< prop->GetName() << " failed : " << propertyLog << std::endl;
								troubleOccured = true;
							}

						}
						else if ( (type & BaseLib::Properties::BaseProperty::Int) == BaseLib::Properties::BaseProperty::Int)
						{
							BaseLib::Properties::Property<int>* prop
								= dynamic_cast<BaseLib::Properties::Property<int>*>(activeModule->GetProperty(item.first));
							if (prop->SetProperty(item.second.toInt(), propertyLog))
							{	// all goes well
							}
							else
							{
								ss << "Setting property "<< prop->GetName() << " failed : " << propertyLog << std::endl;
								troubleOccured = true;
							}
						}
						else if ( (type & BaseLib::Properties::BaseProperty::String) == BaseLib::Properties::BaseProperty::String)
						{
							BaseLib::Properties::Property<std::string>* prop
								= dynamic_cast<BaseLib::Properties::Property<std::string>*>(activeModule->GetProperty(item.first));
							if (prop->SetProperty(item.second.toString().toStdString(), propertyLog))
							{	// all goes well
							}
							else
							{
								ss << "Setting property "<< prop->GetName() << " failed : " << propertyLog << std::endl;
								troubleOccured = true;
							}
						}
						else 
						{	// unknown pripoerty in module .. 
						}								
					}
				);
			}
			// and on the end, becouse of topologcal order we secure that
			// all sources for activeModule now all valid
			activeModule->ModuleValidation();
		}
	}

	// "load" data into scene	
	mTabAssembly->GetScene()->RetakeData(counter, modules);

	if (troubleOccured)
	{
		ss << "Assembly may be loaded incompletely." << std::endl;
	}

	// if troubleOccured is true, then there was erros so we return false 
	return !troubleOccured;
}

//										QMainWindow												//

void FormMain::closeEvent(QCloseEvent *event)
{
	if (mTabAssembly->GetIsWorking())
	{	// we have active bacground thread
		auto result = QMessageBox::warning(this, "Application is working.", "Closing application will result in loosing of all unsaved data and force stop of bacground work, that may end with error. Do you want to continue ?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		if (result == QMessageBox::Yes)
		{	// closing ..
			QMainWindow::closeEvent(event);
		}
		else
		{	// ignore ..
			event->ignore();
		}
	}
	else
	{	// just show dialog about lost unsaved data .. 
		auto result = QMessageBox::question(this, "Close application ?", "Close application ? All unsaved data will be lost.", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		if (result == QMessageBox::Yes)
		{	// continue in closing ...
			QMainWindow::closeEvent(event);
		}
		else
		{	// ignore
			event->ignore();
		}
	}
}

//											Slots												//

void FormMain::slRunModule(Assembly::ItemModule* item)
{
	mTabAssembly->SetEnabled(false);

	mWorkThread.SetModule(item->GetModule());
		
	mWorkThread.start();
}

void FormMain::slWorkDone()
{
	// report that work is done
	mTabAssembly->OnWorkDone();

	// enable
	mTabAssembly->SetEnabled(true);

	if (mWorkThread.GetObjects().size() == 0)
	{	// no objects on output
	}
	else
	{
		Output::Tab* tab = new Output::Tab(
			mWorkThread.GetObjects(), 
			mWorkThread.GetRoot(), 
			mWorkThread.GetKeys(), 		 
			mWorkThread.GetIntegralKeys(), 
			mWorkThread.GetDataItems(),
			this); 
	
		ui.Tabs->addTab(tab, "Result");	
	}
}

void FormMain::slAssemblySave()
{
	QString path = QFileDialog::getSaveFileName(this, tr("Select file"), "", "XML files (*.xml)");
	if (path.isNull())
	{	// dialog canceled
	}
	else
	{	// save
		bool loadOk = true;
		std::stringstream log;
		try
		{
			loadOk = AssemblySave(path, log);
		}
		catch(std::exception ex)
		{
			log << "Outer exception: " << ex.what() << std::endl;
			loadOk = false;
		}
		
		if (!loadOk)
		{
			QMessageBox::critical(0, "Save failed.", log.str().c_str(), QMessageBox::Ok, QMessageBox::Ok);
		}
	}
}

void FormMain::slAssemblyLoad()
{
	QString path = QFileDialog::getOpenFileName(this, tr("Select file to load"), "", "XML files (*.xml)");
	if (path.isNull())
	{	// dialog canceled
	}
	else
	{	// load
		bool loadOk = true;
		std::stringstream log;
		try
		{
			loadOk = AssemblyLoad(path, log);
		}
		catch(std::exception ex)
		{
			log << "Outer exception: " << ex.what() << std::endl;
			loadOk = false;
		}
		
		if (!loadOk)
		{
			QMessageBox::critical(0, "Load failed.", log.str().c_str(), QMessageBox::Ok, QMessageBox::Ok);
		}
	}
}

void FormMain::slTabContextMenu(const QPoint& point)
{
	if (point.isNull())
	{
		return;
	}

	QMenu menu(this);

	int tabIndex = ui.Tabs->GetTabBar()->tabAt(point);
	if (tabIndex == 0)
	{	// assembly
		if (mTabAssembly->GetIsWorking())
		{	// work on background .. so no contextmenu
			return;
		}

		menu.addAction(actClear);
		menu.addAction(ui.actLoad);
		menu.addAction(ui.actSave);
	}
	else if(tabIndex != -1)
	{	// -> result, add actions
		menu.addAction(actRename);
		menu.addAction(actClose);
		// set data
		actRename->setData(tabIndex);
		actRename->setData(tabIndex);
	}
	else
	{	// click somewhere else .. ( where no tab captain is )
	}

	menu.exec(ui.Tabs->mapToGlobal(point));
}

void FormMain::slTabClear()
{	
	auto result = QMessageBox::question(this, tr("Clear scene"), tr("Delete current assembly ?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
	
	if (result == QMessageBox::Yes)
	{	// clear data from scene
		mTabAssembly->GetScene()->Clear();
	}
}

void FormMain::slTabRename()
{
	int index = actRename->data().toInt();
	if (index < 1 || index > ui.Tabs->count())
	{	// invalid index
		return;
	}
	
	bool ok = false;
	QString originalText = ui.Tabs->tabText(index);
	QString name =  QInputDialog::getText(this, tr("Rename tab"),
                                        tr("Name:"), QLineEdit::Normal,
                                        originalText, &ok);
	if (ok && !name.isEmpty())
	{
		ui.Tabs->setTabText(index, name);
	}
}

void FormMain::slTabClose()
{
	int index = actRename->data().toInt();
	if (index < 1 || index > ui.Tabs->count())
	{	// invalid index
		return;
	}

	QMessageBox::StandardButton result = QMessageBox::question(this, "Close ?", "Closing tab will cause loosing of all unsave data. Continue ?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

	if (result == QMessageBox::Yes)
	{	// close .. continue		
	}
	else
	{
		// closing canceled
		return;
	}

	// get tab widget
	QWidget* widget = ui.Tabs->widget(index);
	// remove from tab
	ui.Tabs->removeTab(index);
	// delete widget
	delete widget;
	widget = nullptr;
}