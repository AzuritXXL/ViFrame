#ifndef eny_outputGraphicsView_h
#define eny_outputGraphicsView_h

/**
 * @author 
 * @version 1.0
 * @date 20.5.2012
 */

// Qt includes
///
#include <QGraphicsView>
#include <QAbstractItemModel>
#include <QItemSelectionModel>
#include <qsortfilterproxymodel.h>

// Local includes
///
#include "OutputModel.h"
#include "OutputDataItem.h"
#include "OutputModelSelected.h"

namespace Output {

class GraphicsView : public QGraphicsView
{
	Q_OBJECT
public:
	typedef std::vector<DataModels::Output::DataItem*> DataItemType;
public:
	GraphicsView(DataModels::Output::Model& model, DataItemType& dataItems, QSortFilterProxyModel& proxyModel, QItemSelectionModel& selectionModel, DataModels::Output::ModelSelected& modelSelection, QWidget* parent);
public:
	/**
	 * Update items size to keep them in the same size when zooming.
	 */
	void UpdateItemSize();
	/**
	 * Update scene size to create borders around object.
	 */
	void FitSceneSize();
	/**
	 * Set scene transformation to made whole scene visible in GraphicsView.
	 * Does not update item size so calling UpdateItemSize can be required.
	 */
	void MadeWholeSceneVisible();
	/**
	 * Call after changing selection in GraphicsView.
	 * Report selection change to SelectionModel.
	 */
	void OnUpdateSelection();
protected: // QGraphicsView
    virtual void wheelEvent(QWheelEvent* event);
	virtual void contextMenuEvent(QContextMenuEvent* event);
	virtual void keyPressEvent(QKeyEvent* event);
	virtual void keyReleaseEvent(QKeyEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent* event);
	virtual void mousePressEvent(QMouseEvent* event);
protected slots:
	void slModelSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
private:
	enum eSelectionState 
	{ 
		None		// no selection
		,Ready		// ctr down
		,Deactive	// wait do mouse up to deactive selection
		,Selecting	// ctr and mouse down ..
	};
private:
	DataModels::Output::Model& mModel;
	/**
	 * Proxy model used to translate selection.
	 */
	QSortFilterProxyModel& mProxyModel;
	/**
	 * Selection model.
	 */
	QItemSelectionModel& mSelectionModel;
	/**
	 * Data items.
	 */
	DataItemType& mDataItems;
	/**
	 * True if we do selection.
	 */
	eSelectionState mSelectionState;
	/**
	 * Refrence to model for selected data.
	 */
	DataModels::Output::ModelSelected& mModelSelected;
};

}
 
#endif // eny_outputGraphicsView_h
