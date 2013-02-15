#include "stdafx.h"
// ...
#include "OutputBaseItem.h"
#include "OutputVirtualItem.h"

namespace DataModels {
namespace Output {	

BaseItem::BaseItem(VirtualItem* parent)
	: mParent(parent)
{ }

BaseItem::~BaseItem()
{ }

VirtualItem *BaseItem::GetParent()
{
	return mParent;
}

void BaseItem::SetParent(VirtualItem* item)
{
	mParent = item;
}

void BaseItem::SelectionChange(bool selected)
{
}

} }