#include "stdafx.h"
// Std
#include <sstream>
// ..
#include "AssemblyScene.h"
#include "AssemblyItemModule.h"
#include "AssemblyItemConnection.h"
#include "DialogDetail.h"
#include "DialogConnections.h"
#include "DialogProperties.h"

namespace Assembly {

Scene::Scene(Logic::LibraryStorage& libraries, DataModels::Message::Model& messages, QWidget *parent)
	: QGraphicsScene(parent), mCounter(0), mModules(), mParent(parent), mLibraries(libraries), mMessages(messages)	  
{
	actRun = new QAction("Run", this);
	actProperties = new QAction("Properties", this);
	actDetail = new QAction("Detail", this);
	actDelete = new QAction("Delete", this);
	actDisconnect = new QAction("Disconnect", this);
}

Scene::~Scene()
{
	Clear();
}

void Scene::SetMinSize()
{
	auto iter = std::begin(mModules);
	auto iterEnd = std::end(mModules);
	double maxX = 0, maxY = 0, x = 0, y = 0;

	if (iter != iterEnd)
	{
		maxX = iter->second->pos().x();
		maxY = iter->second->pos().y();
		++iter;
	}

	for(;iter != iterEnd; ++iter)
	{
		x = iter->second->pos().x() + iter->second->boundingRect().width() + 5;
		y = iter->second->pos().y() + iter->second->boundingRect().height() + 5;
		maxX = maxX < x ? x : maxX;
		maxY = maxY < y ? y : maxY;		
	}

	// calculate size
	double sizeX = maxX, sizeY = maxY;	
	auto parentSize = mParent->size();

	// compare size with smalest size == 100
	sizeX = sizeX < 100 ? 100 : sizeX;	
	sizeY = sizeY < 100 ? 100 : sizeY;

	// set new sceneRect
	setSceneRect(0, 0, sizeX, sizeY);
}

void Scene::Clear()
{
	// delete data modules
	std::for_each(std::begin(mModules), std::end(mModules), 
		[&] (std::pair<const std::string, ItemModule*>& item)
		{ 
			// delete item ( that will remove it from scene to )
			// if we remove item from scene before deleting .. then not all data will be released
			delete item.second;
			item.second = nullptr;
		} 
	);
	mModules.clear();
	// set counter
	mCounter = 0;
}

void Scene::RetakeData(size_t& counter, ModulesType& modules)
{
	// release original
	Clear();
	// retake new 
	std::swap(mCounter, counter);
	std::swap(mModules, modules);
	// set size
	SetMinSize();
}

const Scene::ModulesType& Scene::GetModules() const
{
	return mModules;
}

const size_t& Scene::GetCounter() const
{
	return mCounter;
}

void Scene::ContextModule(ItemModule* module, const QPoint& position)
{
	QMenu menu;

	if (module->GetHolder().GetModule()->PrepareForConnection())
	{
		menu.addAction(actRun);
	}

	// if module has some propertie we allow property dialog
	if (module->GetHolder().GetModule()->GetProperties().size() > 0)
	{
		menu.addAction(actProperties);		
	}
	menu.addAction(actDetail);
	menu.addAction(actDelete);

	QAction* result = menu.exec(position);
	if (result == actRun)
	{
		siRunModule(module);
	}
	else if (result == actProperties)
	{
		ShowProperties(module);
	}
	else if (result == actDetail)
	{
		ShowDetail(module);
	}
	else if (result == actDelete)
	{
		Delete(module);
	}
}

void Scene::ContextConnection(ItemConnection* connection, const QPoint& position)
{
	QMenu menu;
	menu.addAction(actDisconnect);
	QAction* result = menu.exec(position);
	if (result == actDisconnect)
	{
		Assembly::ItemModule* target = connection->GetTarget();
		Assembly::ItemModule* source = connection->GetSource();

		bool errorOccure = false;

		// remove all sockets
		while (!connection->GetSockets().empty())
		{
			auto socketNumber = connection->GetSockets()[0];
			
			if (target->GetModule()->Connect(nullptr, socketNumber))
			{	// disconnect ok -> remove graphics rep for socket .. 
				target->RemoveConnection(source, socketNumber);
			}
			else
			{	// something failed .. 
				errorOccure = true;
			}
		}

		if (errorOccure)
		{	// error so show message
			QMessageBox::critical(mParent, tr("Error"), tr("Disconnecting failed."), QMessageBox::NoButton);
		}

		// now all connection should be gone .. 

	}
}

void Scene::LibraryDrop(QGraphicsSceneDragDropEvent *event)
{
	const QMimeData *mime = event->mimeData();

	QByteArray itemData = mime->data("eny/library/key");
	QDataStream dataStream(&itemData, QIODevice::ReadOnly);
	QString libId;
	dataStream >> libId;
	// call for creating object - should not throw exception
	Create(libId, event->scenePos());
	// update
	update();
}

void Scene::ModuleDrop(QGraphicsSceneDragDropEvent *event)
{
	const QMimeData *mime = event->mimeData();
	QByteArray itemData = mime->data("eny/scene/rep");
	QDataStream dataStream(&itemData, QIODevice::ReadOnly);

	QPoint offset;
	QString id;
	dataStream >> id >> offset;

	// obtain module
	if (mModules.count(id.toStdString()) == 0)
	{
		// there is no such source object .. exit ..
		event->ignore();
		return;
	}

	ItemModule *source = mModules[id.toStdString()];

	// select all posible items
	QList<QGraphicsItem*> graphicsTargets = items(event->scenePos().x() - offset.x(), event->scenePos().y() - offset.y(), 
		source->boundingRect().width(), source->boundingRect().height());
	
	QList<QGraphicsItem*>::iterator iter = std::begin(graphicsTargets);
	QList<QGraphicsItem*>::iterator iterEnd = std::end(graphicsTargets);

	ItemModule* target = nullptr;
	for(;iter != iterEnd; ++iter)
	{
		if ((*iter)->data(0) == ItemModule::Identification)
		{	// we found module, connection
			target = dynamic_cast<ItemModule*>((*iter));
			break;
		}
	}

	if (target == nullptr)
	{	// move 
		source->setPos(event->scenePos() - offset);
		source->OnPositionChanged();
		// recalculate size
		SetMinSize();
	}
	else if(target == source)
	{	// shortcut ? no way ... 
	}
	else
	{	// connection, request creation of dialog
		ShowConnection(source, target);
	}
	
	// we "ignore" event, so nothing happen with data
	event->setDropAction(Qt::IgnoreAction);
	event->accept();

	// update(); ?
}

void Scene::DragModuleItem(QGraphicsSceneMouseEvent* event, ItemModule* item)
{
	QPointF hotSpotF = event->scenePos() - item->pos();
	QPoint hotSpot(hotSpotF.x(), hotSpotF.y());
	// prepare data
	QByteArray itemData;
	QDataStream dataStream(&itemData, QIODevice::WriteOnly);
	// store identification and position
	QString qId(item->GetHolder().GetId().c_str());
	dataStream << qId << QPoint(hotSpot);

	QMimeData *mimeData = new QMimeData;
	mimeData->setData("eny/scene/rep", itemData);

	QDrag *drag = new QDrag(mParent);
	drag->setMimeData(mimeData);
	// set representation for drag and drop operation, we use actual item pixmap
	drag->setPixmap(item->pixmap()); 
	drag->setHotSpot(hotSpot);

	// we hide original object
	item->hide();
	Qt::DropActions action = drag->exec(Qt::MoveAction | Qt::IgnoreAction);

	switch(action)
	{
	case Qt::MoveAction: // data were moved out of module
		{	// we data should be deleted on our side
			Delete(item);
			// recalculate size
			SetMinSize();
			break;
		}
	case Qt::IgnoreAction: // data has not been moved any where
		{
			item->show();
			break;
		}
	default: // there is no more action we offer, so no more we support
		break;
	}
}

void Scene::ShowProperties(ItemModule* itemModule)
{	// show properties dialog
	Dialog::Properties dlg(itemModule->GetHolder(), mModules, mParent);
	dlg.exec();
}

void Scene::ShowDetail(ItemModule* itemModule)
{	// show detail dialog
	Dialog::Detail dlg(itemModule->GetHolder(), mParent);
	dlg.exec();
}

void Scene::ShowConnection(ItemModule* source, ItemModule* target)
{
	// show connection dialog
	Dialog::Connections dlg(*source, *target, mParent);
	dlg.exec();
}

void Scene::Delete(ItemModule* itemModule)
{
	// remove from mModules
	mModules.erase(itemModule->GetId().toStdString());
	// delete module, dtor of ItemModule will care about everything
	delete itemModule;
}

void Scene::Create(const QString& id, const QPointF& position)
{
	// prepare module id
	Logic::Library* lib = mLibraries.GetLibrary(id.toStdString());
	if (lib == nullptr)
	{
		QMessageBox msg(mParent);
		msg.setText(tr("Cannot find requested library!"));
		msg.exec();
		return;
	}

	Logic::ModuleHolder* holder = nullptr;
	try
	{
		// can throw
		std::string modId;
		std::stringstream ss;

		ss << ++mCounter << "." << lib->GetDescription().Name;
		std::getline(ss, modId);

		// create moduleholder
		holder = lib->Create(modId);
	}
	catch(std::exception ex)
	{	// nothing to release, so just throw exception
		QMessageBox msg(mParent);
		QString message = tr("Error when creating module : ");
		message += ex.what();
		msg.setText(message);
		msg.exec();
		return;
	}

	// calculate min max size
	SetMinSize();

	// create item module representation
	ItemModule* itemModule = new ItemModule(holder, mMessages, this);
	// set position
	itemModule->setPos(position);
	// add to modules
	mModules[itemModule->GetId().toStdString()] = itemModule;
}

//											QGraphicsScene										//

void Scene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	if ((event->button() &  Qt::LeftButton) != 0)
	{
		QList<QGraphicsItem*> itemsList = items(event->scenePos());
		if (itemsList.size() == 0)
		{	// user click in midle of no where
			return;
		}
		else
		{	// we use first item under cursor
			QGraphicsItem* graphItem = itemsList[0];
			if (graphItem->data(0) == ItemModule::Identification)
			{	// ModuleItem
				DragModuleItem(event, dynamic_cast<ItemModule*>(graphItem));
			}			
		}
	}
	QGraphicsScene::mousePressEvent(event);
}

void Scene::dragEnterEvent(QGraphicsSceneDragDropEvent* event)
{
	if (event->mimeData()->hasFormat("eny/library/key"))
	{
		// we will create new module from library
		event->setDropAction(Qt::CopyAction);
		event->accept();
	}
	else if (event->mimeData()->hasFormat("eny/scene/rep"))
	{
		// test move only in our scope
		event->setDropAction(Qt::MoveAction);
		event->accept();
	}
	else
	{
		event->ignore();
	}
}    

void Scene::dragMoveEvent(QGraphicsSceneDragDropEvent* event)
{
}

void Scene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
	if (event->mimeData()->hasFormat("eny/library/key")) 
	{	// create module		
		LibraryDrop(event);		
	}
	else if (event->mimeData()->hasFormat("eny/scene/rep")) 
	{	// move module
		ModuleDrop(event);
	}
}

void Scene::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
	// select all posible items
	QList<QGraphicsItem*> graphicsTargets = items(event->scenePos());
	
	QList<QGraphicsItem*>::iterator iter = std::begin(graphicsTargets);
	QList<QGraphicsItem*>::iterator iterEnd = std::end(graphicsTargets);

	for(;iter != iterEnd; ++iter)
	{
		if ((*iter)->data(0) == ItemModule::Identification)
		{	// we found module
			ContextModule(dynamic_cast<ItemModule*>((*iter)), event->screenPos());
			break;
		}

		// test connections
		if ( (*iter)->parentItem() != 0)
		{	
			if ((*iter)->parentItem()->data(0) == ItemConnection::Identification)
			{
				ContextConnection(dynamic_cast<ItemConnection*>(((*iter)->parentItem())), event->screenPos());
				break;
			}
		}
	}
}

}
