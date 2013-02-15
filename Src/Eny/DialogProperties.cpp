#include "stdafx.h"
// Qt
#include <QFileDialog>
#include <QtGui\qlabel.h>
#include <QtGui\qlayout.h>
#include <QtGui\qcheckbox.h>
#include <QtGui\qmessagebox.h>
#include <QtGui\QSpacerItem>
#include <QtGui\qcombobox.h>
// ...
#include "Library.h"
#include "DialogProperties.h"
#include "AssemblyItemModule.h"
// BaseLib
#include "FileProperty.hpp"
#include "IntSelection.hpp"

namespace Dialog {

Properties::Properties(Logic::ModuleHolder& moduleHolder, Assembly::Scene::ModulesType& modules, QWidget* parent)
	: QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint), mModules(modules)
{
	// init Ui
	ui.setupUi(this);
	// get module 
	mModule = moduleHolder.GetModule();
	// init mappers
	mSignalsFileSelect = new QSignalMapper(this);
	// ...
	InitProperties();
	// set connections
	connect(ui.btnApply, SIGNAL(clicked()), this, SLOT(slAccept()));
	connect(ui.btnCancel, SIGNAL(clicked()), this, SLOT(slCancel()));
	connect(ui.btnClose, SIGNAL(clicked()), this, SLOT(slClose()));

	FitSize();
};

Properties::~Properties()
{ }

//											SubPrograms											//

template<typename T> bool Properties::PropertySet(QLineEdit* lineEdit, QLineEdit* lineMessage, BaseLib::Properties::Property<T>* propertyPointer, const T& value)
{
	std::string logMessage;
	if (propertyPointer->SetProperty(value, logMessage))
	{
		lineEdit->setStyleSheet("QLineEdit{background: white;}");
		// set empty text
		lineMessage->setText("");
		lineMessage->setVisible(false);
	}
	else
	{	// we expect that .. as a result of invalid data
		lineEdit->setStyleSheet("QLineEdit{background: red;}");
		lineMessage->setText(logMessage.c_str());
		lineMessage->setVisible(true);
		return false;
	}
	return true;
}

void Properties::InitProperty(int indexProperty, BaseLib::iModule::PropertyType::iterator& propertyIter)
{
	int indexGui = 2 * indexProperty;

	QWidget* setter = nullptr;
	QVariant defaultValue;

	try
	{

		switch((*propertyIter)->GetType())
		{
		case BaseLib::Properties::BaseProperty::Bool:
			{
				QCheckBox* checkBox = new QCheckBox(this);
				BaseLib::Properties::Property<bool>* prop
					= dynamic_cast<BaseLib::Properties::Property<bool>*>(*propertyIter);

				checkBox->setChecked( prop->GetProperty() );
				setter = checkBox;

				defaultValue = prop->GetProperty();
				break;
			}
		case BaseLib::Properties::BaseProperty::Double:
			{
				BaseLib::Properties::Property<double>* prop
					= dynamic_cast<BaseLib::Properties::Property<double>*>(*propertyIter);

				QLineEdit* edit = new QLineEdit(QString::number( prop->GetProperty() ), this);
				setter = edit;

				edit->setValidator(new QDoubleValidator(edit));
				connect(edit, SIGNAL(textChanged(const QString&)), this, SLOT(slTextChange()));

				defaultValue = edit->text();
				break;
			}
		case BaseLib::Properties::BaseProperty::Int:
			{
				BaseLib::Properties::Property<int>* prop
					= dynamic_cast<BaseLib::Properties::Property<int>*>(*propertyIter);

				QLineEdit* edit = new QLineEdit( QString::number( prop->GetProperty() ), this);
				setter = edit;

				edit->setValidator(new QIntValidator(edit));				
				connect(edit, SIGNAL(textChanged(const QString&)), this, SLOT(slTextChange()));
								
				defaultValue = edit->text();
				break;
			}
		case BaseLib::Properties::BaseProperty::IntSelection:
			{
				BaseLib::Properties::IntSelection* prop
					= dynamic_cast<BaseLib::Properties::IntSelection*>(*propertyIter);

				QComboBox* combo = new QComboBox(this);
				setter = combo;
				combo->setEditable(false);
				// set values for combo box
				auto iter = std::begin(prop->GetValues());
				auto iterEnd = std::end(prop->GetValues());
				int topIndex = prop->GetValues().size() + 1;
				for (; iter != iterEnd; ++iter)
				{	// insert on end ( index > size -> append )
					combo->insertItem(topIndex, iter->c_str());
				}

				// set defaul value
				combo->setCurrentIndex(prop->GetProperty());
				connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(slCurrentIndexChanged(int)));
								
				defaultValue = combo->currentIndex();

				break;
			}
		case BaseLib::Properties::BaseProperty::StringFile:
			{
				BaseLib::Properties::FileProperty* prop
					= dynamic_cast<BaseLib::Properties::FileProperty*>(*propertyIter);

				QLineEdit* edit = new QLineEdit( prop->GetProperty().c_str(), this);
				setter = edit;
				
				// ..
				QPushButton* showFile = new QPushButton("Select file", this);
				ui.Properties->addWidget(showFile, indexGui, 2); // add to layout

				connect(showFile, SIGNAL(clicked()), mSignalsFileSelect, SLOT(map()));
				// add connection, in cProperty.Data are stored extensions
				mSignalsFileSelect->setMapping(showFile, new sFileSelectArg(edit, prop->GetExtension().c_str()));
				connect(edit, SIGNAL(textChanged(const QString&)), this, SLOT(slTextChange()));
								
				defaultValue = edit->text();
				break;
			}
		case BaseLib::Properties::BaseProperty::String:
			{
				BaseLib::Properties::Property<std::string>* prop
					= dynamic_cast<BaseLib::Properties::Property<std::string>*>(*propertyIter);

				QLineEdit* edit = new QLineEdit( prop->GetProperty().c_str(), this);
				setter = edit;

				connect(edit, SIGNAL(textChanged(const QString&)), this, SLOT(slTextChange()));			
				
				defaultValue = edit->text();
				break;
			}
		default: // unknown property type		
			break;
		}
	}
	catch(...)
	{
		QMessageBox msg(this);
		msg.setText("Creating of property failed.");
		msg.setIcon(QMessageBox::Warning);
		msg.exec();

		// delete setter
		if (setter != nullptr)
		{
			delete setter;
			setter = nullptr;
		}

		return;
	}

	// create label and error textbox
	QString propertyName = (*propertyIter)->GetName().c_str();
	QLabel* label = new QLabel(propertyName, this);	
	
	if ((*propertyIter)->GetDescription() == "")
	{	// no description
	}	
	else
	{	// assing description 
		label->setToolTip((*propertyIter)->GetDescription().c_str());
		// add underline
		auto font = label->font();
		font.setUnderline(true);
		label->setFont(font);
	}

	QLineEdit* tipLine = new QLineEdit(this);
	tipLine->setReadOnly(true);
	ui.Properties->addWidget(label, indexGui, 0);
	ui.Properties->addWidget(tipLine, indexGui + 1, 1);
	// add setter to layout
	ui.Properties->addWidget(setter, indexGui, 1);

	// store information	
	mProperties.push_back(sProperty(indexProperty, setter, tipLine, defaultValue));

	// set tooltip
	mProperties[mProperties.size() - 1].Setter->setToolTip((*propertyIter)->GetDescription().c_str());
	mProperties[mProperties.size() - 1].TipLine->setVisible(false);	
}

void Properties::InitProperties()
{
	BaseLib::iModule::PropertyType properties = mModule->GetProperties();
	BaseLib::iModule::PropertyType::iterator iter = std::begin(properties);
	BaseLib::iModule::PropertyType::iterator iterEnd = std::end(properties);

	// iterate through properties
	for(int i = 0; iter != iterEnd; ++iter, ++i)
	{
		try
		{
			InitProperty(i, iter);
		}
		catch(...)
		{	// one more check for sure .. 
			QMessageBox msg(this);
			msg.setText("Creating of property failed.");
			msg.setIcon(QMessageBox::Warning);
			msg.exec();
		}
	}
	// add QSpacerItem on end
	QSpacerItem* spacer = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
	ui.Properties->addItem(spacer, properties.size() * 2, 0);

	// add signal connection
	connect(mSignalsFileSelect, SIGNAL(mapped(QObject*)), this, SLOT(slSetFile(QObject*)));
}

void Properties::FitSize()
{
	// resize ... ( only height )
	int height = ui.Properties->sizeHint().height() + 100;
	height = height > maximumHeight() ? maximumHeight() : height; // size limitation
	resize(size().width(), height);
}

//											Slots												//

void Properties::slTextChange()
{
	QWidget* senderWidget = qobject_cast<QWidget*>(sender());
	senderWidget->setStyleSheet("QLineEdit{background: yellow;}");
}

void Properties::slAccept()
{	// try to set all variables
	std::vector<sProperty>::iterator iter = std::begin(mProperties);
	std::vector<sProperty>::iterator iterEnd = std::end(mProperties);
	for (;iter != iterEnd;++iter)
	{
		switch(mModule->GetProperty(iter->Index)->GetType())
		{
		case BaseLib::Properties::BaseProperty::Bool:
			{
				BaseLib::Properties::Property<bool>* prop
					= dynamic_cast<BaseLib::Properties::Property<bool>*>(mModule->GetProperty(iter->Index));

				QCheckBox* edit = qobject_cast<QCheckBox*>(iter->Setter);
				bool value = edit->isChecked();
				std::string log;
				if ( prop->SetProperty(value, log) )
				{
					// update original value
					iter->OriginalValue = edit->isChecked();
					iter->TipLine->setVisible(false);
				}
				else
				{	// return to original state and show message
					iter->TipLine->setText(log.c_str());
					iter->TipLine->setVisible(true);
				}
				break;
			}
		case BaseLib::Properties::BaseProperty::Double:
			{
				BaseLib::Properties::Property<double>* prop
					= dynamic_cast<BaseLib::Properties::Property<double>*>(mModule->GetProperty(iter->Index));
				QLineEdit* edit = qobject_cast<QLineEdit*>(iter->Setter);

				if (PropertySet(edit, iter->TipLine, prop, edit->text().toDouble()))
				{
					iter->OriginalValue = edit->text();
				}
				break;
			}
		case BaseLib::Properties::BaseProperty::Int:
			{
				BaseLib::Properties::Property<int>* prop
					= dynamic_cast<BaseLib::Properties::Property<int>*>(mModule->GetProperty(iter->Index));
				QLineEdit* edit = qobject_cast<QLineEdit*>(iter->Setter);

				if(PropertySet(edit, iter->TipLine, prop, edit->text().toInt()))
				{
					iter->OriginalValue = edit->text();
				}
				break;
			}
		case BaseLib::Properties::BaseProperty::IntSelection:
			{
				BaseLib::Properties::IntSelection* prop
					= dynamic_cast<BaseLib::Properties::IntSelection*>(mModule->GetProperty(iter->Index));
				QComboBox* combo = qobject_cast<QComboBox*>(iter->Setter);

				std::string log;
				if ( prop->SetProperty(combo->currentIndex(), log) )
				{					// update original value
					iter->OriginalValue = combo->currentIndex();
					iter->Setter->setStyleSheet("QComboBox{background: white;}");
					
					iter->TipLine->setStyleSheet("QLineEdit{background: white;}");
					iter->TipLine->setText("");
					iter->TipLine->setVisible(false);
				}
				else
				{	// show message					
					iter->TipLine->setStyleSheet("QLineEdit{background: red;}");
					iter->TipLine->setText(log.c_str());
					iter->TipLine->setVisible(true);
				}

				break;
			}
		case BaseLib::Properties::BaseProperty::StringFile:
		case BaseLib::Properties::BaseProperty::String:
			{
				BaseLib::Properties::Property<std::string>* prop
					= dynamic_cast<BaseLib::Properties::Property<std::string>*>(mModule->GetProperty(iter->Index));
				QLineEdit* edit = qobject_cast<QLineEdit*>(iter->Setter);

				if(PropertySet(edit, iter->TipLine, prop, edit->text().toStdString()))
				{
					iter->OriginalValue = edit->text();
				}
				break;
			}
		default: // unknown property type
			break;
		}
	}
	// resize becouse of show/hidden content ( editboxex )
	FitSize();

	//
	bool showDeleteDialog = false;
	// show dialog if connection were broken ...
	auto moduleIter = mModules.begin();
	auto moduleIterEnd = mModules.end();
	for (;moduleIter != moduleIterEnd; ++moduleIter)
	{
		if (moduleIter->second->TestConnectionDeleted())
		{
			showDeleteDialog = true;
			break;
		}
	}
	if (showDeleteDialog)
	{	// show dialog
		QMessageBox msg(this);
		QString message = 
			"Due the changes in properties of the module some connections become invalid and so they were disconnected.";
		
		msg.setText(message);
		msg.exec();

	}
	// TestConnectionDeleted
}

void Properties::slCancel()
{
	std::vector<sProperty>::iterator iter = std::begin(mProperties);
	std::vector<sProperty>::iterator iterEnd = std::end(mProperties);
	for (;iter != iterEnd;++iter)
	{
		switch(mModule->GetProperty(iter->Index)->GetType())
		{
		case BaseLib::Properties::BaseProperty::Bool:
			{
				QCheckBox* edit = qobject_cast<QCheckBox*>(iter->Setter);
				edit->setChecked(iter->OriginalValue.toBool());
				break;
			}
		case BaseLib::Properties::BaseProperty::IntSelection:
			{
				QComboBox* combo = qobject_cast<QComboBox*>(iter->Setter);
				combo->setCurrentIndex(iter->OriginalValue.toInt());
				combo->setStyleSheet("QComboBox{background: white;}");
				iter->TipLine->setVisible(false);
				break;
			}
		case BaseLib::Properties::BaseProperty::Double:
		case BaseLib::Properties::BaseProperty::Int:
		case BaseLib::Properties::BaseProperty::StringFile:
		case BaseLib::Properties::BaseProperty::String:
			{
				QLineEdit* edit = qobject_cast<QLineEdit*>(iter->Setter);
				edit->setText(iter->OriginalValue.toString());
				edit->setStyleSheet("QLineEdit{background: white;}");
				iter->TipLine->setVisible(false);
				break;
			}
		default: // unknown property type
			break;
		}
		// set toolTipLine
		iter->TipLine->setText("");
	}

	// resize becouse of show/hidden content ( editboxex )
	FitSize();
}

void Properties::slClose()
{
	close();
}

void Properties::slSetFile(QObject* objArg)
{	// obtain related lineEdit
	sFileSelectArg* arg = dynamic_cast<sFileSelectArg*>(objArg);
	
	QString fileName = QFileDialog::getOpenFileName(this, tr("Select file"),
                                                 arg->LineEdit->text(),
                                                 arg->Extensions);
	if (fileName.isNull())
	{	// dialog canceled
	}
	else
	{
		if (arg->LineEdit->text() == fileName)
		{	// no change to values			
		}
		else
		{
			arg->LineEdit->setText(fileName);
		}
	}
	return;
}

void Properties::slCurrentIndexChanged(int index)
{
	QWidget* senderWidget = qobject_cast<QWidget*>(sender());
	senderWidget->setStyleSheet("QComboBox{background: yellow;}");
}

}