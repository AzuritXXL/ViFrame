#ifndef baseLib_propoerties_fileProperty_h
#define baseLib_propoerties_fileProperty_h

/**
 * Contains class for string property, that should be 
 * used to select path to a file.
 *
 * @author
 * @version 1.0
 * @date 1.5.2012
 */

// Local includes
///
#include "BaseProperty.hpp"
#include "Property.hpp"

namespace BaseLib {
namespace Properties {

class FileProperty : public Property<std::string>
{
public:
	/**
	 * Returns allowed file extensions.
	 * @return File extensions
	 */
	const std::string& GetExtension();
public:
	friend BaseProperty* CreateFileProperty(const std::string& name, const std::string& description, std::string& data, const std::string& extension);
protected:
	/**
	 * Allowed file extensions. In Qt format.
	 */
	std::string mExtension;
protected:
	/**
	 * Max value should be greater than Min value, if not they are switched.
	 * @param[in] type Property type.
	 * @param[in] name Property name.
	 * @param[in] description Property description.
	 * @param[in] data Associated value.
	 * @param[in] extension Allowed file extensions.
	 */
	FileProperty(const std::string& name, const std::string& description, std::string& data, const std::string& extension);
};

BaseProperty* CreateFileProperty(const std::string& name, const std::string& description, std::string& data, const std::string& extension);

} }

#endif // baseLib_propoerties_fileProperty_h
