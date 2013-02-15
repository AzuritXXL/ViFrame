#include "stdafx.h"
// Qt
#include <QProgressDialog>
// ..
#include "OutputModel.h"
#include "OutputDataItem.h"

namespace DataModels {
namespace Output {

Model::Model(VirtualItem* root,  DataItemsType& dataItems, QWidget* parent)
	:QAbstractItemModel(parent), mRoot(root), mDataItems(dataItems), mParent(parent)
{
	mIconHiden.addFile(":/Icons/Resources/IconHiden.png");
}

Model::~Model()
{
	if (mRoot != nullptr)
	{
		// delete him
		delete mRoot;
		mRoot = nullptr;
	}
}

VirtualItem* Model::GetRoot()
{
	return mRoot;
}

void Model::AddGroup(const QString& name, VirtualItem* parent)
{
	// obtain parent under which we can add molecules
	VirtualItem* usedParent = nullptr;
	if (parent->GetType() == VirtualItem::Identification)
	{
		usedParent = dynamic_cast<VirtualItem*>(parent);
	}
	else
	{
		usedParent = parent->GetParent();
	}
	// we use usedParent instead of parent now ..
	QModelIndex parentIndex;
	if (usedParent == mRoot)
	{	// for root we use invalid index
		parentIndex = QModelIndex();
	}
	else
	{	// in other cases we create index 
		parentIndex = createIndex(usedParent->GetRow(), 0, usedParent);
	}

	size_t childCount = usedParent->GetChildCount();
	beginInsertRows(parentIndex, childCount, childCount);
	// insert item	
	usedParent->AppendChild(new VirtualItem(name, usedParent));
	endInsertRows();
}

void Model::DismissGroup(VirtualItem* group)
{
	QModelIndex originalParent = createIndex(group->GetRow(), 0, group);
	QModelIndex targetParent;
	if (group->GetParent() == mRoot)
	{	// for root we use invalid index
		targetParent = QModelIndex();
	}
	else
	{
		targetParent = createIndex(group->GetParent()->GetRow(), 0, group->GetParent());
	}
	// report that we move all items from group to his parent folder
	beginMoveRows(originalParent, 0, group->GetChildCount(), targetParent, group->GetParent()->GetChildCount());
	// move item to parent
	group->GetParent()->RetakeItems(group);
	endMoveRows();

	// remove groupe from ViewModel
	beginRemoveRows(targetParent, group->GetRow(), group->GetRow());
	group->GetParent()->RemoveChild(group);
	endRemoveRows();
	// delete group
	delete group;
	group = nullptr;
}

QModelIndex Model::CreateIndex(int row, int column, void * ptr) const
{
	return createIndex(row, column, ptr);
}

void Model::ReGroup(const QString& key)
{
	beginResetModel();
	// DataItems are stored in mDataItems, so we can destroy structure .. 
	mRoot->DecayStructure();
	// ad we start build new one
	
	VirtualItem* noValue = new VirtualItem("no value", nullptr);
	// we split items to group
	std::map<QString, VirtualItem*> groups;

	std::string keyStd = key.toStdString();
	QString attValue;

	// split into groups
	auto iter = std::begin(mDataItems);
	auto iterEnd = std::end(mDataItems);
	for (;iter != iterEnd; ++iter)
	{
		if ( (*iter)->GetObject().GetData().count(keyStd) == 0)
		{	// no value under key, go into extra groupe
			noValue->AppendChild(*iter);
		}
		else
		{
			// get value translated into string
			attValue = (*iter)->GetObject().GetData().at(keyStd)->ToString().c_str();
			// if there is no group for value, we established one
			if (groups[attValue] == nullptr)
			{
				groups[attValue] = new VirtualItem(attValue, nullptr);
				mRoot->AppendChild(groups[attValue]);
			}
			// add item
			groups[attValue]->AppendChild(*iter);
		}
	}

	// is something in 'noValue' group ?
	if (noValue->GetChildCount() > 0)
	{
		mRoot->AppendChild(noValue);
	}
	else
	{	// no data release
		delete noValue;
		noValue = nullptr;
	}

	endResetModel();
}

void Model::MoveIntoNewGroup(const QString& groupName, const std::vector<DataItem*>& items)
{	// start with crating group, parent is root
	QModelIndex parentIndex = QModelIndex();
	// new group is also targetParent for items
	VirtualItem* targetParent = new VirtualItem(groupName, mRoot);

	QProgressDialog progressDialog(tr("Moving items..."), QString(), 0, items.size(), mParent);

	beginInsertRows(parentIndex, mRoot->GetChildCount(), mRoot->GetChildCount());	
	mRoot->AppendChild(targetParent);
	endInsertRows();
	
	size_t progress = 0;
	size_t next_progress = 0;
	// now we will move items into new group
	std::for_each(std::begin(items), std::end(items), 
		[&, targetParent] (DataItem* item)
		{	
			// move item
			MoveObjects(targetParent, item);

			// report 
			++progress;
			if (progress > next_progress)
			{
				next_progress += mSelection.size() / 20;
				progressDialog.setValue(progress);
			}
		}
	);

	progressDialog.setValue(items.size());
}

//									QAbstractItemModel											//

Qt::ItemFlags Model::flags(const QModelIndex &index) const
{
    if (!index.isValid())
	{
		if (index.row() == -1 && index.column() == -1)
		{	// we allow drop to root item ( and to some in fact .. )
			return Qt::ItemIsDropEnabled;
		}
		else
		{
			return 0;
		}		
	}

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

QVariant Model::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
	{
        return QVariant();
	}
	
	BaseItem *item = static_cast<BaseItem*>(index.internalPointer());
	
	switch (role)
	{
	case Qt::DecorationRole: // The data to be rendered as a decoration in the form of an icon. (QColor, QIcon or QPixmap)
		if (item->GetVisible())
		{	// no icon ( object is visible )
			return QVariant();
		}
		else
		{
			return mIconHiden;
		}
		break;
	default:	// all other roles solve items him self
		return item->GetData(role);
	}
}

QVariant Model::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole)
     {
         if (orientation == Qt::Horizontal) 
		 {
             switch (section)
             {
             case 0:
                 return QString("Objects name");
             }
         }
     }
     return QVariant();
}

int Model::rowCount(const QModelIndex &parent) const
{
    BaseItem *parentItem;
    if (parent.column() > 0)
	{
        return 0;
	}

    if (!parent.isValid())
	{
        parentItem = mRoot;
	}
    else
	{
        parentItem = static_cast<BaseItem*>(parent.internalPointer());
	}

    return parentItem->GetChildCount();
}

int Model::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
	{
        return static_cast<BaseItem*>(parent.internalPointer())->GetColumnCount();
	}
    else
	{
		return mRoot->GetColumnCount();
	}
}

QModelIndex Model::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
	{
        return QModelIndex();
	}
    BaseItem *parentItem = nullptr;

    if (!parent.isValid())
	{
        parentItem = mRoot;
	}
    else
	{
        parentItem = static_cast<BaseItem*>(parent.internalPointer());
	}

    BaseItem *childItem = parentItem->GetChild(row);
    if (childItem)
	{
        return createIndex(row, column, childItem);
	}
    else
	{
        return QModelIndex();
	}
}

QModelIndex Model::parent(const QModelIndex &index) const
{
    if (!index.isValid())
	{
         return QModelIndex();
	}

    BaseItem *childItem = static_cast<BaseItem*>(index.internalPointer());
    BaseItem *parentItem = childItem->GetParent();

    if (parentItem == mRoot)
	{	// root can't be touched
        return QModelIndex();
	}

    return createIndex(parentItem->GetRow(), 0, parentItem);
}

Qt::DropActions Model::supportedDropActions() const
{
	return Qt::MoveAction;
}

Qt::DropActions Model::supportedDragActions() const
{
	return Qt::MoveAction;
}

QStringList Model::mimeTypes() const
{
	QStringList types;
    types << "output/model/item";
    return types;
}

QMimeData* Model::mimeData(const QModelIndexList& indexes) const
{
    QMimeData *mimeData = new QMimeData();
		
	mSelection.clear();
	// foreach object in selection, we add it to our local selection pool
    foreach (QModelIndex index, indexes)
	{
        if (index.isValid()) 
		{			
			mSelection.push_back(reinterpret_cast<BaseItem*>(index.internalPointer()));
        }
    }

    mimeData->setData("output/model/item", 0);
    return mimeData;
}

bool Model::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &index)
{	// drop means move data somewhere into structure
	BaseItem* targetParentBase = reinterpret_cast<BaseItem*>(index.internalPointer());
	
	if (targetParentBase == nullptr)
	{	// if we don't have parent, insert into root
		targetParentBase = mRoot;
	}
	// parent can be only VirtualItem
	
	if (targetParentBase->GetType() == VirtualItem::Identification)
	{	// ok parent is virtual
	}
	else
	{	// user parent of targetParent, he will be VirtualItem
		targetParentBase = targetParentBase->GetParent();
	}

	// recast targetParent
	VirtualItem* targetParent = dynamic_cast<VirtualItem*>(targetParentBase);

	QProgressDialog progressDialog(tr("Moving items..."), QString(), 0, mSelection.size(), mParent);

	// source parent is known from moved objects
	if (data->hasFormat("output/model/item") && action == Qt::MoveAction)
	{
		size_t progress = 0;
		size_t next_progress = 0;

		std::for_each(std::begin(mSelection), std::end(mSelection), 
			[&, targetParent] (BaseItem* item)
			{	// remove item from parent
				assert(targetParent != dynamic_cast<BaseItem*>(this));
				// move item
				MoveObjects(targetParent, item);
				
				// report 
				++progress;
				if (progress > next_progress)
				{
					next_progress += mSelection.size() / 20;
					progressDialog.setValue(progress);
				}
			}
		);
		// we use selection, so clear it
		progressDialog.setValue(mSelection.size());
		mSelection.clear();
		return true;
	}
	progressDialog.setValue(mSelection.size());

	return false;
}

void Model::MoveObjects(VirtualItem* targetParent, BaseItem* item)
{
	if (targetParent == item->GetParent())
	{ // don't move 
	}
	else
	{	// for each item we create source and target parent index
		QModelIndex originalParentIndex, targetParentIndex;
		if (item->GetParent() == mRoot)
		{
			originalParentIndex = QModelIndex();
		}
		else
		{
			originalParentIndex = createIndex(item->GetParent()->GetRow(), 0, item->GetParent());
		}
					
		if (targetParent == mRoot)
		{
			targetParentIndex = QModelIndex();
		}
		else
		{
			targetParentIndex = createIndex(targetParent->GetRow(), 0, targetParent);
		}

		bool result = beginMoveRows(originalParentIndex, item->GetRow(), item->GetRow(), targetParentIndex, targetParent->GetChildCount());
		//beginMoveRows(originalParentIndex, item->GetRow(), item->GetRow(), index, targetParent->GetChildCount());
		assert(result);

		// remove from old position
		assert(item->GetParent() != nullptr);

		item->GetParent()->RemoveChild(item);
		// append item in new target
		targetParent->AppendChild(item);
		endMoveRows();
	}
}

} }