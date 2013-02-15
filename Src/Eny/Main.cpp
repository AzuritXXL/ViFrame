#include "stdafx.h"
// ...
#include "FormMain.h"
#include "LibraryModel.h"
#include "LibraryStorage.h"
#include "LibraryDirCreator.h"
#include "AssemblyItemModule.h"
// Qt
#include <QtGui/QApplication>


#include <vector>
#include <algorithm>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	
// Load libraries

	QString appPath = app.applicationDirPath();	
#ifdef _DEBUG
	QString modulePath = appPath;
#else
	QString modulePath = appPath + "/Modules/";
#endif

	// Libraries model
	DataModels::Library::LibraryModel libraryModel;	
	// Storage for libraries
	Logic::LibraryStorage libraryStorage;
	// Load libraries structure
	try
	{
		Logic::LibraryDirCreator::Create(libraryModel.GetRoot(), libraryStorage, QDir(appPath), QDir(modulePath), "*.dll");
	}
	catch (std::exception ex)
	{
		QMessageBox msg;
		msg.setText("Failed do load libraries from: " + modulePath);
		msg.exec();
		// all resources from root will be released with end of the program
		return 1;
	}
	
#ifdef EXTERN_STYLE
	{
		//QFile styleFile(":/Styles/Resources/MainStyle.qss");
		QFile styleFile("MainStyle.qss");
		styleFile.open(QFile::ReadOnly);
		// Apply the loaded stylesheet
		QString style(styleFile.readAll());
		styleFile.close();
		app.setStyleSheet(style);
	}
#endif 

// Register new object for socket/signal
	qRegisterMetaType<Assembly::ItemModule*>("Assembly::ItemModule");
	qRegisterMetaType<DataModels::Message::Item>("DataModels::Message::Item");	

	// Main window
	FormMain mainForm(&libraryModel, libraryStorage);
	// Run app
	mainForm.show();
	return app.exec();
}
