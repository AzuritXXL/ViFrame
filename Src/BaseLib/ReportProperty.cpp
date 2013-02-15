#include "stdafx.h"
// ..
#include "ReportProperty.hpp"

namespace BaseLib {
namespace Properties {


BaseProperty* CreateReport(const std::string& name, const std::string& description, bool& data, Reportable* report)
{
	return new ReportProperty<bool>(BaseProperty::Bool, name, description, data, report);
}

BaseProperty* CreateReport(const std::string& name, const std::string& description, double& data, Reportable* report)
{
	return new ReportProperty<double>(BaseProperty::Double, name, description, data, report);
}

BaseProperty* CreateReport(const std::string& name, const std::string& description, int& data, Reportable* report)
{
	return new ReportProperty<int>(BaseProperty::Int, name, description, data, report);
}

BaseProperty* CreateReport(const std::string& name, const std::string& description, std::string& data, Reportable* report)
{
	return new ReportProperty<std::string>(BaseProperty::String, name, description, data, report);
}

} }