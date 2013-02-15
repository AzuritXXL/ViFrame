#include "stdafx.h"
// ...
#include "LibraryVirtualItem.h"

namespace DataModels {
namespace Library {	

LibraryVirtualItem::LibraryVirtualItem(QString& name, LibraryBaseItem* parent)
	: LibraryBaseItem(parent), mName(name), mChilds()
{ }

LibraryVirtualItem::~LibraryVirtualItem()
{
	qDeleteAll(mChilds);
}

//											LibraryBaseItem										//

void LibraryVirtualItem::AppendChild(LibraryBaseItem *child)
{
	mChilds.append(child);
}

LibraryBaseItem* LibraryVirtualItem::GetChild(int row)
{
	return mChilds[row];
}

int LibraryVirtualItem::GetChildCount() const
{
	return mChilds.count();
}

int LibraryVirtualItem::GetColumnCount() const
{
	return 1;
}

int LibraryVirtualItem::GetRow() const
{
    if (mParent)
	{
		return mParent->GetChildRow(this);
	}
    return 0;
}

QVariant LibraryVirtualItem::GetData(int role) const
{
	switch(role)
	{
	case Qt::DisplayRole: // The key data to be rendered in the form of text. (QString)
		return mName;
	default:
		break;
	}
	// return empty variant
	return QVariant();
}

void LibraryVirtualItem::Load()
{
	std::for_each(mChilds.begin(), mChilds.end(), 
		[](LibraryBaseItem* item)
		{
			item->Load();
		}
	);
}

void LibraryVirtualItem::UnLoad()
{
	std::for_each(mChilds.begin(), mChilds.end(), 
		[](LibraryBaseItem* item)
		{
			item->UnLoad();
		}
	);	
}

int LibraryVirtualItem::GetChildRow(const LibraryBaseItem* item) const
{
	return mChilds.indexOf(const_cast<LibraryBaseItem*>(item));
}

bool LibraryVirtualItem::GetHoldLibrary() const
{
	return false;
}

} }