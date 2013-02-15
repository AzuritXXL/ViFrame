#ifndef baseLib_objects_variable_h
#define baseLib_objects_variable_h

/**
 * Contains template for simple data types,
 * template subclass Variant.
 *
 * @author
 * @version 1.0
 * @date 1.5.2012
 */

// Standart library includes
///
#include <sstream>

// Local includes
/// 
#include "Variant.hpp"

namespace BaseLib {
namespace Objects {

/**
 * Template for simple data object class.
 * TYPE - stored type
 * ID - object id ( from. Variant::eTypes )
 */ 
template <typename TYPE, int ID> struct Variable : Variant
{
public:
	/**
	 * Objects data.
	 */
	TYPE Data;
public:
	/**
	 * Emtpy ctor.
	 */
	Variable() 
	{ };
	/**
	 * @param[in] value Value.
	 */
	Variable(const TYPE& value)
		: Data(value)
	{ };
public:	// Variant
	virtual int GetType() const
	{
		return ID;
	}
	virtual std::string ToString() const
	{
		std::stringstream ss;
		ss << Data;
		return ss.str();
	}
};

typedef Variable<int, Variant::Int> Int;

typedef Variable<double, Variant::Double> Double;

typedef Variable<std::string, Variant::String> String;

} }

#endif // baseLib_objects_variable_h