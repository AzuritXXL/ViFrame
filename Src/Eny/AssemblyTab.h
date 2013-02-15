#ifndef eny_tabAssembly_h
#define eny_tabAssembly_h

/**
 * @author 
 * @version 1.0
 * @date 20.5.2012
 */

// Qt includes 
///
#include <qwidget.h>

// Generated includes
///
#include "ui_TabAssembly.h"

// Local includes
///
#include "MessageModel.h"
#include "LibraryModel.h"
#include "LibraryStorage.h"
#include "AssemblyItemModule.h"

namespace Assembly {

// Forward declaration
///
class Scene;

/**
 * Class with assembly tab.
 */
class Tab : public QWidget
{
	Q_OBJECT
public:
	Tab(DataModels::Library::LibraryModel* libraryModel,
		Logic::LibraryStorage& libraryStorage, QWidget* parent);
public:
	/**
	 * Disable/enable just tab.
	 * @param[in] enable Is enable ?
	 */
	void SetEnabled(bool enable);
	/**
	 * Return pointer to scene with assembly.
	 * @return pointer to scene
	 */
	Scene* GetScene() const;
	/**
	 * Retrun pointer to tab message model.
	 * @return tab message model
	 */
	DataModels::Message::Model* GetMessageModel();
	/**
	 * Return true if there is work on background. Is 
	 * determined from SetEnabled function. 
	 * If module is disable then there is work.
	 * @return is working ?
	 */
	bool GetIsWorking() const;
	/**
	 *  Called when backgroud work end.
	 *	Reset module images.
	 */
	void OnWorkDone();
protected slots:
	/**
	 * Show context menu for libraries.
	 */
	void slLibraryContextMenu(const QPoint&);	
private:
	/**
	 * Message model.
	 */
	DataModels::Message::Model mMessageModel;
	/**
	 * Scene with modules.
	 */
	Scene* mScene;
	/**
	 * True if tab is partialy disabled becouse of background work.
	 */
	bool mIsWorking;
	/**
	 * Graphics interface.
	 */
	Ui::TabAssembly ui;
private:
	QAction* actLoadLibrary;
	QAction* actUnloadLibrary;
};

}

#endif // eny_tabAssembly_h