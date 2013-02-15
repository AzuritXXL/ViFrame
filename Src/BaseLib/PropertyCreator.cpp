#include "stdafx.h"
// ..
#include "BaseProperty.hpp"
#include "Property.hpp"
#include "PropertyCreator.hpp"

namespace BaseLib {
namespace Properties {

BaseProperty* PropertyCreator::Create(const std::string& name, const std::string& description, int& data)
{
	return new Property<int>(BaseProperty::Int, name, description, data);
}

BaseProperty* PropertyCreator::Create(const std::string& name, const std::string& description, double& data)
{
	return new Property<double>(BaseProperty::Double, name, description, data);
}

BaseProperty* PropertyCreator::Create(const std::string& name, const std::string& description, std::string& data)
{
	return new Property<std::string>(BaseProperty::String, name, description, data);
}

BaseProperty* PropertyCreator::Create(const std::string& name, const std::string& description, bool& data)
{
	return new Property<bool>(BaseProperty::Bool, name, description, data);
}

} }