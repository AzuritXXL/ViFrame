#ifndef baselib_reportable_h
#define baselib_reportable_h

/**
 * Contains abstract interface for classes, to which
 * modules can send report.
 *
 * @author
 * @version 1.0
 * @date 1.5.2012
 */

// Standart libratry includes
///
#include <string>

namespace BaseLib {

class iModule;

/**
 * Class is ment to be used as a communication chanel 
 * between a module and the outside world.
 */
class iReportable
{
public:
	/**
	 * Type of Reportable messages.
	 */
	enum eMessageType{
		Information	/*!< Information. */
		,Warning	/*!< Warning. */
		,Error		/*!< Error. */
	};
public:
	/**
	 * Report message. 
	 * @param[in] sender Sender module.
	 * @param[in] type Type of message ( values of eMessageType ).
	 * @param[in] message Message text.
	 */
	virtual void Message(BaseLib::iModule* sender, BaseLib::iReportable::eMessageType type, std::string message) = 0;
	/**
	 * Report module work progress value.
	 * @param[in] sender Sender module.
	 * @param[in] min Min or start value.
	 * @param[in] max Max or end value.
	 * @param[in] value Actual value.
	 */
	virtual void Progress(BaseLib::iModule* sender, int min, int max, int value) = 0;
	/**
	 * Called when a module changes one of his input connections in OnSourceChange method.
	 * @param[in] sender Sender module.
	 */
	virtual void ConnectionChanged(BaseLib::iModule* sender) = 0;
	/**
	 * Called when the module interface could have been changed.  
	 * Especially when value of PrepareForConnection could have been changed.
	 * @param[in] sender Sender module.
	 */
	virtual void InterfaceChanged(BaseLib::iModule* sender) = 0;
};

}

#endif
