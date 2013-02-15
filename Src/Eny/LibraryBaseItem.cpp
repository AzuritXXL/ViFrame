#include "stdafx.h"
// ...
#include "LibraryBaseItem.h"

namespace DataModels {
namespace Library {	

LibraryBaseItem::LibraryBaseItem(LibraryBaseItem* parent)
	: mParent(parent)
{ }

LibraryBaseItem::~LibraryBaseItem()
{ }

LibraryBaseItem* LibraryBaseItem::GetParent()
{
	return mParent;
}

} }