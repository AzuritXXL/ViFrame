#ifndef formMain_h
#define formMain_h

/**
 * @author 
 * @version 1.0
 * @date 20.5.2012
 */

// Standart includes
///
#include <sstream>

// Qt includes
///
#include <QtGui/QMainWindow>

// Generated includes
///
#include "ui_FormMain.h"

// Local includes
///
#include "AssemblyTab.h"
#include "LibraryModel.h"
#include "LibraryStorage.h"
#include "AssemblyItemModule.h"
#include "WorkThread.h"

class FormMain : public QMainWindow
{
	Q_OBJECT
public:
	FormMain(DataModels::Library::LibraryModel* libraryModel, 
		Logic::LibraryStorage& libraryStorage, 
		QWidget *parent = 0, 
		Qt::WFlags flags = 0);
	~FormMain();
public:
	/**
	 * Save assembly.
	 * @param[in] file name
	 * @param[in] stream to write error into
	 * @return false if error ocured
	 */
	bool AssemblySave(const QString& path, std::stringstream& ss);
	/**
	 * Load assembly.
	 * @param[in] file name
	 * @param[in] stream to write error into
	 * @return false if error ocured
	 */
	bool AssemblyLoad(const QString& path, std::stringstream& ss);
protected: // QMainWindow
	virtual void closeEvent(QCloseEvent *event);
protected slots:
	/**
	 * Run process on module.
	 */
	void slRunModule(Assembly::ItemModule* item);
	/**
	 * Called when mWorkThread finish.
	 */
	void slWorkDone();
	/**
	 * Save assembly.
	 */
	void slAssemblySave();
	/**
	 * Load assembly.
	 */
	void slAssemblyLoad();
	/**
	 * Show context menu.
	 */
	void slTabContextMenu(const QPoint& point);
	/**
	 * Clear assembly tab.
	 */
	void slTabClear();
	/**
	 * Rename tab.
	 * To indentify number use actRename data.
	 */
	void slTabRename();
	/**
	 * Close tab.
	 * To indentify use actClose data.
	 */
	void slTabClose();
private:
	/**
	 * Graphical interface.
	 */
	Ui::FormMainClass ui;
	/**
	 * Tab with module assembly.
	 */
	Assembly::Tab* mTabAssembly;
	/**
	 * Working thread.
	 */
	Logic::WorkThread mWorkThread;
	/**
	 * Library storage.
	 */
	Logic::LibraryStorage& mLibraryStorage;
private:
	/**
	 * Clear assembly content.
	 */
	QAction* actClear;
	/**
	 * Rename result tab.
	 */
	QAction* actRename;
	/**
	 * Close result tab.
	 */
	QAction* actClose;
};

#endif // formMain_h
