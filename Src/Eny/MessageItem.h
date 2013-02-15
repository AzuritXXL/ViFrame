#ifndef eny_messagesItem_h
#define eny_messagesItem_h

/**
 * @author 
 * @version 1.0
 * @date 20.5.2012
 */

// Qt includes
///
#include <qstring.h>
#include <qdatetime.h>

// BaseLib incldues
///
#include "Reportable.hpp"

namespace DataModels {
namespace Message {

struct Item
{
	/**
	 * Message type.
	 */
	BaseLib::iReportable::eMessageType Type;
	/**
	 * Id of module that send message.
	 */
	QString Module;
	/**
	 * Message
	 */
	QString Message;
	/**
	 * Time of creation of message.
	 */
	QDateTime CreatedTime;	
public:
	Item();
	Item(BaseLib::iReportable::eMessageType type, const QString& module, const QString& message);
};

} }

#endif // eny_messagesItem_h