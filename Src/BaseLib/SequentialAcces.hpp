#ifndef baseLib_interfaces_sequentialAcces_h
#define baseLib_interfaces_sequentialAcces_h

/**
 * Contains class with interface.
 *
 * @author
 * @version 1.0
 * @date 1.5.2012
 */

// Local includes
/// 
#include "Object.hpp"
#include "Exception.hpp"

namespace BaseLib {
namespace Interfaces {

/**
 * Povides interface to access objects in sequential way. 
 * The same object can be accesed via GetObject many times, 
 * but after calling Next and moving on to the next object the previous object
 * can't be obtained any more.
 *
 * The class is ment to be used as an interface between modules, so it should
 * not throw exceptions, if exception occur they should be passed through the appropriate param.
 * If the class is asked for an object and an exception occurs, the class should delete the object 
 * and return nullptr.
 */
class SequentialAcces
{
public:
	enum {
		Identification = 2	/*!< Interface identification */
	};
public:
	/**
	 * Move on next object.
	 * @param[out] ex Exception holder.
	 * @return False if there is no object.
	 */
	virtual bool Next(ExceptionHolder& ex) = 0;
	/**
	 * Returns pointer to the object. Returns nullptr if an exception occurs. 
	 * @param[out] ex Exception holder.
	 * @return Pointer to the object or nullptr.
	 */
	virtual BaseLib::Objects::Object* GetObject(ExceptionHolder& ex) = 0;
};

} }

#endif // baseLib_interfaces_sequentialAcces_h