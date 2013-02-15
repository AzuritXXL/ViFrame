#include "stdafx.h"
// ..
#include "MessageItem.h"

namespace DataModels {
namespace Message {

Item::Item() 
	: CreatedTime( QDateTime::currentDateTime() )
{ }

Item::Item(BaseLib::iReportable::eMessageType type, const QString& module, const QString& message)
	: Type(type), Module(module), Message(message), CreatedTime( QDateTime::currentDateTime() )
{ }

} }