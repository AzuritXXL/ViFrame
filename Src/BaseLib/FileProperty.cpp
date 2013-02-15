#include "stdafx.h"
// ..
#include "BaseProperty.hpp"
#include "FileProperty.hpp"


namespace BaseLib {
namespace Properties {

FileProperty::FileProperty(const std::string& name, const std::string& description, std::string& data, const std::string& extension)
	: Property(BaseProperty::StringFile, name, description, data), mExtension(extension)
{ };

const std::string& FileProperty::GetExtension()
{
	return mExtension;
}		

BaseProperty* CreateFileProperty(const std::string& name, const std::string& description, std::string& data, const std::string& extension)
{
	return new FileProperty(name, description, data, extension);
}

} }
