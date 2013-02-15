#ifndef baseLib_properties_property_h
#define baseLib_properties_property_h

/**
 * Contains base template for properties.
 *
 * @author
 * @version 1.0
 * @date 1.5.2012
 */

// Standart library includes 
///
#include <string>

// Local includes
///
#include "BaseProperty.hpp"

namespace BaseLib {
namespace Properties {

// Forward declaration
///
struct PropertyCreator;

/**
 * Base template the class property.
 * Data validation can be done in SetProperty function.
 * Property do not store data themselves. These are stored
 * in the passed refrence.
 */
template<typename TYPE> class Property : public BaseProperty
{
public:	
	friend struct PropertyCreator;
public:
	/**
	 * Do validation and set property. If validation failes, 
	 * the property is not changed, and message is stored in log.
	 * @param[in] data New property value.
	 * @param[out] log Error message.
	 * Returns false in case of invalid data.
	 */
	virtual bool SetProperty(const TYPE& data, std::string& log)
	{
		mData = data;
		return true;
	}
	/**
	 * Called when loading. If false is returned
	 * another call is done before module validation and
	 * after conneting all sources.
	 * @param[in] data New property value.
	 * Returns false in case of invalid data.
	 */
	virtual bool SetPropertyLoad(const TYPE& data)
	{
		std::string log;
		return SetProperty(data, log);
	}
	/**
	 * Returns property value.
	 * @return Property value
	 */
	virtual const TYPE& GetProperty() const
	{
		return mData;
	}
protected:
	/**
	 * Refrence to property data.
	 */
	TYPE& mData;
protected:
	/**
	 * @param[in] type Property type.
	 * @param[in] name Property name.
	 * @param[in] description Property description.
	 * @param[in] data Refrence to the associated data.
	 */
	Property(BaseProperty::eTypes type, const std::string& name, const std::string& description, TYPE& data)
		: BaseProperty(type, name, description), mData(data)
	{ }
};

} }

#endif // baseLib_properties_property_h