#ifndef baseLib_interfaces_randomAcces_h
#define baseLib_interfaces_randomAcces_h

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
 * Provides interface for accessing data directly through an index.
 * A class can be repeatedly asked for an object with the same index, in that 
 * case the same object should be returned.
 * Objects that are once passed are not any more in the ownership of the class
 * and thus must not be deleted. On the other hand, objects that 
 * have not been passed should be deleted.
 *
 * The class is ment to be used as an interface between modules, so it should
 * not throw any exceptions. In case an error occurs it should be passed through 
 * an appropriate parameter. If the class is asked for an object and an exception 
 * occures, the class should delete the object and return nullptr.
 */
class RandomAcces
{
public:
	enum {
		Identification = 1	/*!< Interface identification */
	};
public:
	/**
	 * Returns the number of objects stored.
	 * @return Objects count
	 */
	virtual size_t GetCount() const
		throw() = 0;
	/**
	 * Returns pointer to the n-th object. The caller becomes owner and
	 * when the time comes, she should delete the passed object. If an exception occurs
	 * it should be passed by using the ex param.
	 * @param[in] index Objects index.
	 * @param[out] ex	Exception holder, for passing an exception.
	 * @return Pointer to the desired object or nullptr if the index is out of range.
	 */
	virtual BaseLib::Objects::Object* GetObject(const size_t& index, ExceptionHolder& ex) = 0;
	/**
	 * Resets the out state which means that the class should retake back
	 * the ownership of all stored objects.
	 */
	virtual void ClearOutState() throw() = 0;
};

} }

#endif // baseLib_interfaces_randomAcces_h