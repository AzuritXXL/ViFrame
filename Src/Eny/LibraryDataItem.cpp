#include "stdafx.h"
// BaseLibr
#include "Description.hpp"
// ...
#include "Library.h"
#include "LibraryDataItem.h"
#include "LibraryVirtualItem.h"

namespace DataModels {
namespace Library {	

LibraryDataItem::LibraryDataItem(Logic::Library& library, QString& name, LibraryBaseItem* parent)
	: LibraryBaseItem(parent), mLibrary(library), mName(name), mToolTip("")
{ }

Logic::Library& LibraryDataItem::GetLibrary()
{
	return mLibrary;
}

//											LibraryBaseItem										//

void LibraryDataItem::AppendChild(LibraryBaseItem *child)
{
	// we cant add child
	assert(false);
}

LibraryBaseItem* LibraryDataItem::GetChild(int row)
{
	assert(false);
	return nullptr;
}

int LibraryDataItem::GetChildCount() const
{
	return 0;
}

int LibraryDataItem::GetColumnCount() const
{
	return 1;
}

int LibraryDataItem::GetRow() const
{
    if (mParent)
	{
		return mParent->GetChildRow(this);
	}
    return 0;
}

QVariant LibraryDataItem::GetData(int role) const
{
	switch(role)
	{
	case Qt::DecorationRole: // The data to be rendered as a decoration in the form of an icon. (QColor, QIcon or QPixmap)
		break;
	case Qt::ForegroundRole: // The foreground brush (text color, typically) used for items rendered with the default delegate. (QBrush)
		if (mLibrary.GetIsLoaded())
		{
			return Qt::black;
		}
		else
		{
			return Qt::gray;
		}
	case Qt::DisplayRole: // The key data to be rendered in the form of text. (QString)
		return mName;
	case Qt::ToolTipRole: // The data displayed in the item's tooltip. (QString)
		return mToolTip;
	default:
		break;
	}
	// return empty variant
	return QVariant();
}

void LibraryDataItem::Load()
{
	mLibrary.Load();
	// library loaded sucesfully - get some data
	mToolTip = mLibrary.GetDescription().ShortDescription.c_str();
}

void LibraryDataItem::UnLoad()
{
	mLibrary.UnLoad();
}

int LibraryDataItem::GetChildRow(const LibraryBaseItem* item) const
{
	// we have no children
	assert(false);
	return -1;
}

bool LibraryDataItem::GetHoldLibrary() const
{
	return true;
}


} }