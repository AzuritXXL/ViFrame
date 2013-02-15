#include "stdafx.h"
// ..
#include "BaseProperty.hpp"
#include "LimitedProperty.hpp"


namespace BaseLib {
namespace Properties {

BaseProperty* CreateLimited(const std::string& name, const std::string& description, int& data, int min, int max)
{
	return new LimitedProperty<int>(BaseProperty::Int, name, description, data, min, max);
}

BaseProperty* CreateLimited(const std::string& name, const std::string& description, double& data, double min, double max)
{
	return new LimitedProperty<double>(BaseProperty::Double, name, description, data, min, max);
}

} }
