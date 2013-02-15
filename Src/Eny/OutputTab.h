#ifndef eny_tabOutput_h
#define eny_tabOutput_h

/**
 * @author 
 * @version 1.0
 * @date 20.5.2012
 */

// Standart librarie includes
///
#include <vector>
#include <unordered_set>

// Qt incldues
///
#include <qlist.h>
#include <qwidget.h>
#include <QProgressDialog.h>
#include <QTabWidget.h>
#include <QListView>

// BaseLib includes
///
#include "Object.hpp"

// Local includes
///
#include "OutputModel.h"
#include "OutputPolicy.h"
#include "OutputDataItem.h"
#include "OutputVirtualItem.h"
#include "OutputGraphicsView.h"
#include "OutputModelSelected.h"

// Forward declarations
///
class QTreeView;
class QSortFilterProxyModel;

namespace Output {

/**
 * Present result to user.
 */
class Tab : public QWidget
{
	Q_OBJECT
public:
	typedef std::vector<DataModels::Output::DataItem*> DataItemType;
public:
	Tab(std::vector<BaseLib::Objects::Object*>& objects, DataModels::Output::VirtualItem* root, QList<QString>& keys, QList<QString>& integralKeys, DataItemType& dataItems, QWidget* parent);
	~Tab();
public:
	/**
	 * Made whole scene visible.
	 */
	void AdjustScene();
	/**
	 * @retrun true if there is work in background
	 */
	bool GetIsWorking() const;
protected slots:
	/**
	 * Show context menu for tree object.
	 */
	void sShowContextMenu(const QPoint& position);
	void slAttributes();
	void slCreateGroup();
	void slDismissGroup();
	void slSetColor();
	void slRename();
	void slReGroup();
	void slSetMain();
	void slExport();
	void slResizeScene();
	void slHide();
	void slShow();
	void slSaveScreen();
	void slGenColors();
	void slSaveAllScreens();
	void sShowContextMenuList(const QPoint& position);
	void slItemViewDoubleClicked(const QModelIndex & index);
private:
	/**
	 * Stored objects.
	 */
	std::vector<BaseLib::Objects::Object*> mObjects;
	/**
	 * Store pointers to model items.
	 */
	DataItemType mModelDataItems;
	/**
	 * Data model.
	 */
	DataModels::Output::Model mModel;
	/**
	 * Proxy model.
	 */
	QSortFilterProxyModel* mProxyModel;
	/**
	 * Graphics view.
	 */
	GraphicsView* mGraphicsView;
	/**
	 * Tree view.
	 */
	QTreeView* mTreeView;
	/**
	 * Selected item, used to transfer selected items between
	 * slots.
	 */
	DataModels::Output::BaseItem* mSelectedItem;
	/**
	 * Keys to data.
	 */
	QList<QString> mDataKeys;
	/**
	 * Keys to integral data.
	 */
	QList<QString> mIntegralDataKeys;
	/**
	 * Data max size.
	 */
	std::pair<double, double> mSceneSize;
	/**
	 * Actula scene scale.
	 */
	std::pair<double, double> mSceneScale;
	/**
	 * If true Tab act in silent mode and do not show any dialog to user.
	 */
	bool mSilentMode;
	/**
	 * View for selected items.
	 */
	QListView* mListView;
	/**
	 * Tab for mTreeView and mTableView.
	 */
	QTabWidget* mTabWidget;
	/**
	 * Model for selected data.
	 */
	DataModels::Output::ModelSelected mModelSelected;
private:
	/**
	 * Show object attributes in dialog.
	 */
	QAction* actAttributes;
	/**
	 * Create group in parent object.
	 */
	QAction* actCreateGroup;
	/**
	 * Dismiss groupe.
	 */
	QAction* actDismissGroup;
	/**
	 * Set color to selected groupe.
	 */
	QAction* actSetColor;
	/**
	 * Rename selected object.
	 */
	QAction* actRename;
	/**
	 * Regroup item in tree view according to value in attributes.
	 */
	QAction* actReGroup;
	/**
	 * Set main att. that will be used to calculate graphics rep. coordinates.
	 */
	QAction* actSetMain;
	/**
	 * Export selected files.
	 */
	QAction* actExport;
	/**
	 * Resize scene.
	 */
	QAction* actReSizeScene;
	/**
	 * Hide graphics representation.
	 */
	QAction* actHide;
	/**
	 * Show graphics representation;
	 */
	QAction* actShow;
	/**
	 * Take and save screen.
	 */
	QAction* actSaveScreen;
	/**
	 * Generate colors for man groups.
	 */
	QAction* actGenColors;
	/**
	 * Export into directory images of all posible combination from 
	 * single MainValue
	 */
	QAction* actSaveAllScreens;
	/**
	 * Used do show atributes in 
	 *
	 */
	QAction* actListAttributes;
	/**
	 * Move selection to new group.
	 */
	QAction* actMoveSelectionToNewGroup;
private:
	/**
	 * Create graphics representation for objects.
	 */
	void CreateGraphics();
	/**
	 * Initialize user interface.
	 */
	void InitGui();
	/**
	 * Paint legend.
	 * @param[in] width
	 * @param[in] height
	 * @param[in] lenght of longest item
	 * @param[in] items in legend
	 * @param painter 
	 * @param[in] position ( values from Dialog::SaveScreen )
	 */
	void DrawLegend(int width, int height, int longestText, const std::unordered_set<DataModels::Output::VirtualItem*>& items, QPainter& painter, int position) const;
	/**
	 * Automaticaly rescale scene by changing items position to fit into bounds.
	 * Does not affected mGraphicsView or his snece in any way.
	 */
	void AutoRescale();
	/**
	 * Set new main data and call AutoRescale.
	 * @return false is some data were missing
	 */ 
	bool SetMain(const Output::Policy& policy);
};

}

#endif // eny_tabOutput_h