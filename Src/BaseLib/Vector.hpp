#ifndef baseLib_objects_vector_h
#define baseLib_objects_vector_h

/**
 * Contains template for vector of data,
 * template subclass Variant.
 *
 * @author
 * @version 1.0
 * @date 1.5.2012
 */

// Standart library includes
///
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include <iterator>

// Local includes
///
#include "Variant.hpp"

namespace BaseLib {
namespace Objects {

/**
 * Template for vector of simple data object class.
 * TYPE - stored type
 * ID - object id ( from Variant::eTypes )
 */ 
template <typename TYPE, int ID> struct Vector : public Variant
{
public:
	/**
	 * Objects data.
	 */
	std::vector<TYPE> Data;
public:
	virtual int GetType() const
	{
		return ID;
	}
	virtual std::string ToString() const
	{
		bool first = true;
		std::stringstream ss;
		ss << '(';
		std::for_each(Data.begin(), Data.end(),
			[&ss, &first] (const TYPE& item)
			{
				if (first)
				{					
					first = false;
				}
				else
				{
					ss << ',';
				}
				ss << item;
			}
		);
		ss << ')';
		return ss.str();
	}
};

typedef Vector<int, Variant::VectorInt> VInt;

typedef Vector<double, Variant::VectorDouble> VDouble;

} }

#endif