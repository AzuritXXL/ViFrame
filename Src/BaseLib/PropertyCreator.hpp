#ifndef baseLib_properties_propertyCreator_h
#define baseLib_properties_propertyCreator_h

/**
 * Contains class that provides functions
 * for creating base properties.
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
#include "Property.hpp"

namespace BaseLib {
namespace Properties {

/**
 * Class containing functions for creation of base property classes.
 */
struct PropertyCreator
{
public:
	/**
	 * Creates and binds simple property class to data.
	 * @param[in] name Property name.
	 * @patam[in] description Property description.
	 * @param[in] data Associated data.
	 * @return Pointer to the new property class.
	 */
	static BaseProperty* Create(const std::string& name, const std::string& description, int& data);
	/**
	 * Creates and binds simple property class to data.
	 * @param[in] name Property name.
	 * @patam[in] description Property description.
	 * @param[in] data Associated data.
	 * @return Pointer to new property class
	 */
	static BaseProperty* Create(const std::string& name, const std::string& description, double& data);
	/**
	 * Creates and bind simple property class to data.
	 * @param[in] name Property name.
	 * @patam[in] description Property description.
	 * @param[in] data Associated data.
	 * @return Pointer to new property class.
	 */
	static BaseProperty* Create(const std::string& name, const std::string& description, std::string& data);	
	/**
	 * Creates and binds simple property class to data.
	 * @param[in] name Property name.
	 * @patam[in] description Property description.
	 * @param[in] data Associated data.
	 * @return Pointer to new property class
	 */
	static BaseProperty* Create(const std::string& name, const std::string& description, bool& data);
};

} }

#endif // baseLib_properties_propertyCreator_h