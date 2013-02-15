#include "stdafx.h"
// ..
#include "OutputVirtualItem.h"

namespace DataModels {
namespace Output {

VirtualItem::VirtualItem(const QString& name, VirtualItem* parent)
	: BaseItem(parent), mName(name), mChilds(), mColor(Qt::black)
{ }

VirtualItem::~VirtualItem()
{
	auto iter = std::begin(mChilds);
	auto iterEnd = std::end(mChilds);
	for (; iter != iterEnd; ++iter)
	{
		// delete no mather what is it .. 
		delete (*iter);
	}
	mChilds.clear();
}

const QString& VirtualItem::GetName() const
{
	return mName;
}

void VirtualItem::SetName(const QString& name)
{
	mName = name;
}

void VirtualItem::SetColor(const QColor& color)
{
	mColor = color;
	// set for children, use out stored color, so color will be valid for long enough
	std::for_each(std::begin(mChilds), std::end(mChilds), 
		[&] (BaseItem* item) 
		{
			item->SetColor(&mColor);
		}
	);
}

void VirtualItem::RetakeItems(VirtualItem* source)
{
	assert(this != source);
	if (this == source)
	{	// nothing to do
		return;
	}

	std::for_each(std::begin(source->mChilds), std::end(source->mChilds),
		[&] (BaseItem* item) 
		{	// add on the end
			mChilds.push_back(item);
			// set color
			item->SetColor(&mColor);
			item->SetParent(this);
		}
	);
	// clear source mChilds
	source->mChilds.clear();
}

void VirtualItem::FlatSubItems()
{	// we go though all items, becouse we will add items in time we can use iterators
	size_t originCount = mChilds.count();
	VirtualItem* virtualItem = nullptr;
	for (size_t i = 0; i < originCount && i < mChilds.count(); ++i)
	{
		if (mChilds[i]->GetType() == VirtualItem::Identification)
		{
			virtualItem = dynamic_cast<VirtualItem*>(mChilds[i]);
			// flat
			virtualItem->FlatSubItems();
			// retake items
			RetakeItems(virtualItem);
			// delete item
			delete mChilds[i];
			// and remove
			mChilds.removeAt(i);
		}
		else
		{	// item is DataItem, so we do nothing with it
		}
	}
}

void VirtualItem::RemoveChild(BaseItem* item)
{
	bool value = mChilds.removeOne(item);
	assert(value);
//. Be more efective ?
}

int VirtualItem::GetObjectRow(BaseItem* item)
{
	return mChilds.indexOf(item);
}

void VirtualItem::DecayStructure()
{
	auto iter = std::begin(mChilds);
	auto iterEnd = std::end(mChilds);
	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->GetType() == VirtualItem::Identification)
		{	
			// call on child item
			dynamic_cast<DataModels::Output::VirtualItem*>((*iter))->DecayStructure();
			// delete VirtualItems
			delete (*iter);
		}
		else
		{	// we don't delete DataItems

		}
		(*iter) = nullptr;
	}

	mChilds.clear();
}


//											BaseItem											//

void VirtualItem::AppendChild(BaseItem *child)
{
	mChilds.append(child);
	// set color and parent
	child->SetColor(&mColor);
	child->SetParent(this);
}

BaseItem* VirtualItem::GetChild(int row)
{
	return mChilds[row];
}

int VirtualItem::GetChildCount() const
{
	return mChilds.count();
}

int VirtualItem::GetColumnCount() const
{
	return 1;
}

int VirtualItem::GetRow() const
{
    if (mParent != nullptr)
	{
		return mParent->GetObjectRow(const_cast<VirtualItem*>(this));
	}
    return 0;
}

QVariant VirtualItem::GetData(int role) const
{
	switch(role)
	{
	case Qt::DisplayRole: // The key data to be rendered in the form of text. (QString)
		return mName;
	case Qt::ForegroundRole: // The foreground brush (text color, typically) used for items rendered with the default delegate. (QBrush)
		return mColor;
	default:
		break;
	}
	// return empty variant
	return QVariant();
}

void VirtualItem::SetColor(const QColor* color)
{	// use out special SetColor method
	SetColor(*color);
}

int VirtualItem::GetType() const
{
	return Identification;
}

void VirtualItem::Serialize(const std::string& key, std::ostream& stream)
{
	std::for_each(std::begin(mChilds), std::end(mChilds), 
		[&] (BaseItem* item) 
		{
			item->Serialize(key, stream);
		}
	);
}

void VirtualItem::SetVisible(bool visibility)
{
	std::for_each(std::begin(mChilds), std::end(mChilds), 
		[&] (BaseItem* item) 
		{
			item->SetVisible(visibility);
		}
	);
}

bool VirtualItem::GetVisible() const
{	// item is always in visible state .. or check visibility of all sub items ?
	return true;
}

const QColor& VirtualItem::GetColor() const
{
	return mColor;
}

} }