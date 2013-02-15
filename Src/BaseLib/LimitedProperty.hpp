#ifndef baseLib_properties_limitedProperty_h
#define baseLib_properties_limitedProperty_h

/**
 * Contains template for the limited property.
 *
 * @author
 * @version 1.0
 * @date 1.5.2012
 */

// Standart library includes
///
#include <assert.h>
#include <sstream>
#include <algorithm>

// Local includes
///
#include "BaseProperty.hpp"
#include "Property.hpp"

namespace BaseLib {
namespace Properties {

/**
 * Template for the limited property.
 * TYPE - stored type
 */
template<typename TYPE> class LimitedProperty : public Property<TYPE>
{
public: // Property
	virtual bool SetProperty(const TYPE& data, std::string& log)
	{
		if (data < mMinValue || data > mMaxValue)
		{	// prepare an error message
			std::stringstream ss;
			ss << "Value must be greater or equal than " << mMinValue << " and less or equal than " << mMaxValue;
			log = ss.str();
			return false;
		}
		else
		{	// call base implementation
			return Property<TYPE>::SetProperty(data, log);
		}
	}
public:
	friend BaseProperty* CreateLimited(const std::string& name, const std::string& description, int& data, int min, int max);
	friend BaseProperty* CreateLimited(const std::string& name, const std::string& description, double& data, double min, double max);
protected:
	/**
	 * Max value should be greater than Min value, if not they are switched.
	 * @param[in] type Property type.
	 * @param[in] name Property name.
	 * @param[in] description Property description.
	 * @param[in] data Associated value.
	 * @param[in] min Min value.
	 * @param[in] max Max value.
	 */
	LimitedProperty(BaseProperty::eTypes type,  const std::string& name, const std::string& description, TYPE& data, TYPE min, TYPE max)
		: Property<TYPE>(type, name, description, data), mMaxValue(max), mMinValue(min)
	{
		// check that min is less than max .. if not swap them
		assert(mMinValue < mMaxValue);
		if (mMinValue > mMaxValue)
		{
			std::swap(mMaxValue, mMinValue);
		}
	};
protected:
	/**
	 * Max value.
	 */
	TYPE mMaxValue;
	/**
	 * Min value.
	 */
	TYPE mMinValue;
};

BaseProperty* CreateLimited(const std::string& name, const std::string& description, int& data, int min, int max);
BaseProperty* CreateLimited(const std::string& name, const std::string& description, double& data, double min, double max);

} }


#endif // baseLib_properties_limitedProperty_h
