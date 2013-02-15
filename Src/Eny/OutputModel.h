#ifndef eny_outputModel_h
#define eny_outputModel_h

/**
 * @author 
 * @version 1.0
 * @date 20.5.2012
 */

// Standart libraries includes
///
#include <vector>

// Qt includes
///
#include <QtGui\qtreewidget.h>
#include <qwidget.h>

// Local includes
///
#include "OutputVirtualItem.h"

namespace DataModels {
namespace Output {

// Forward declaration
///
class DataItem;

class Model : public QAbstractItemModel
{
	Q_OBJECT
public:
	typedef std::vector<DataItem*> DataItemsType;
public:
	Model(VirtualItem* root, DataItemsType& dataItems, QWidget* parent);
	~Model();
public:
	/**
	 * @return return root item
	 */
	VirtualItem* GetRoot();
	/**
	 * Add new groupe with certain name.
	 * @param[in] name
	 * @param[in] object under wich add group
	 */
	void AddGroup(const QString& name, VirtualItem* parent);
	/**
	 * Dismiss group, items are add to group's parent.
	 * Group is deleted, pointer become invalid after call.
	 * @param[in] pointer to group
	 */
	void DismissGroup(VirtualItem* group);
	/**
	 * Wrap public acces to createIndex method.
	 */
	QModelIndex CreateIndex(int row, int column, void * ptr) const;
	/**
	 * Regroupe items in model, according to values in data 
	 * under certain key.
	 * @param[in] key to data
	 */
	void ReGroup(const QString& key);
	/**
	 * Move item to newly created group.
	 * @param[in] groupName Name for new group.
	 * @param[in] items Items to move.
	 */
	void MoveIntoNewGroup(const QString& groupName, const std::vector<DataItem*>& items);
public: // Read-Only access
    Qt::ItemFlags flags(const QModelIndex& index) const;
	QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	int rowCount(const QModelIndex& parent = QModelIndex()) const;
public: // Read-Only access - subclass QAbstractItemModel:
	int columnCount(const QModelIndex& parent = QModelIndex()) const;
public: // Hierarchical models
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex& index) const;
public: // Drag and drop
	Qt::DropActions supportedDropActions() const;
	Qt::DropActions supportedDragActions() const;
	QStringList mimeTypes() const;
	QMimeData* mimeData(const QModelIndexList& indexes) const;
	bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &index);
private:
	/**
	 * Root item.
	 */
	VirtualItem* mRoot;
	/**
	 * Used to stor selected items in drag and drop operation.
	 * Used in const mimeData method.
	 */
	mutable std::vector<BaseItem*> mSelection;
	/**
	 * DataItems in module.
	 */
	DataItemsType& mDataItems;
	/**
	 * Icon for hiden items.
	 */
	QIcon mIconHiden;
	/**
	 * Parent widget
	 */
	QWidget* mParent;
private:
	/**
	 * Move item from current location under new parent.
	 * @param[in] targetParent New parent.
	 * @param[in] item Item to move.
	 */
	void MoveObjects(VirtualItem* targetParent, BaseItem* item);
};

} }

#endif // eny_outputModel_h