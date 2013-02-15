#include "stdafx.h"
// ...
#include "LibraryModel.h"
#include "LibraryBaseItem.h"
#include "LibraryDataItem.h"
#include "LibraryVirtualItem.h"

namespace DataModels {
namespace Library {

LibraryModel::LibraryModel(QObject *parent)
	: QAbstractItemModel(parent), mRoot(new LibraryVirtualItem(QString(""), nullptr))
{
}

LibraryModel::LibraryModel(LibraryVirtualItem* root, QObject *parent)
	: QAbstractItemModel(parent), mRoot(root)
{
}

LibraryModel::~LibraryModel()
{
	delete mRoot;
}

LibraryVirtualItem* LibraryModel::GetRoot()
{
	return mRoot;
}

//										QAbstractItemModel										//

Qt::ItemFlags LibraryModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
	{	// even for invalid index we accept drop
        return Qt::ItemIsDropEnabled;
	}
	LibraryBaseItem* item = reinterpret_cast<LibraryBaseItem*>(index.internalPointer());

	if (item->GetHoldLibrary())
	{
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
	}
	else
	{
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
	}	
}

QVariant LibraryModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
	{
        return QVariant();
	}
	LibraryBaseItem* item = reinterpret_cast<LibraryBaseItem*>(index.internalPointer());

	return item->GetData(role);
}

QVariant LibraryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	return QVariant();
}

int LibraryModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
	{
        return 0;
	}

	LibraryBaseItem* item;
	// select item as a root or from valid index
    if (parent.isValid())
	{
		item = static_cast<LibraryBaseItem*>(parent.internalPointer());        
	}
    else
	{
        item = static_cast<LibraryBaseItem*>(mRoot);
	}
    return item->GetChildCount();
}

int LibraryModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
	{
        return static_cast<LibraryBaseItem*>(parent.internalPointer())->GetColumnCount();
	}
    else
	{
        return mRoot->GetColumnCount();
	}
}

QModelIndex LibraryModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
	{
        return QModelIndex();
	}

	LibraryBaseItem* parentItem;
	// select item as a root or from valid index
    if (parent.isValid())
	{
		parentItem = static_cast<LibraryBaseItem*>(parent.internalPointer());        
	}
    else
	{
        parentItem = static_cast<LibraryBaseItem*>(mRoot);
	}

	// obtain child
    LibraryBaseItem* childItem = parentItem->GetChild(row);
    if (childItem)
	{
        return createIndex(row, column, childItem);
	}
    else
	{
        return QModelIndex();
	}
}

QModelIndex LibraryModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
	{
         return QModelIndex();
	}

    LibraryBaseItem *childItem = static_cast<LibraryBaseItem*>(index.internalPointer());
    LibraryBaseItem *parentItem = childItem->GetParent();

    if (parentItem == mRoot)
	{
        return QModelIndex();
	}

    return createIndex(parentItem->GetRow(), 0, parentItem);
}

QMimeData *LibraryModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
	
    QByteArray encodedData;
    QDataStream stream(&encodedData, QIODevice::WriteOnly);

	// foreach object in selection
    foreach (QModelIndex index, indexes) {
        if (index.isValid()) 
		{
			LibraryBaseItem* itemBase = static_cast<LibraryBaseItem*>(index.internalPointer());
			if (itemBase->GetHoldLibrary())
			{	// object hold library
				stream << QString( static_cast<LibraryDataItem*>(itemBase)->GetLibrary().GetId().c_str() );
			}
        }
    }
	// return libraries id 
    mimeData->setData("eny/library/key", encodedData);
    return mimeData;
}

QStringList LibraryModel::mimeTypes() const
{
	QStringList types;
    types << "eny/scene/rep";
    return types;
}

Qt::DropActions LibraryModel::supportedDragActions() const
{
	return Qt::CopyAction;
}

bool LibraryModel::dropMimeData(const QMimeData *mimeData, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{	// we support incoming data from table
	if (mimeData->hasFormat("eny/scene/rep") && action == Qt::MoveAction)
	{	// move action from table, table will release data
		return true;
	}

	return false;
}

Qt::DropActions LibraryModel::supportedDropActions() const
{
	return Qt::MoveAction;
}
	
} }