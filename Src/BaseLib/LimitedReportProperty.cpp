#include "stdafx.h"
// ..
#include "LimitedReportProperty.hpp"

namespace BaseLib {
namespace Properties {
	
BaseProperty* CreateLimitedReport(const std::string& name, const std::string& description, double& data, double min, double max, Reportable* report)
{
	return new LimitedReportProperty<double>(BaseProperty::Double, name, description, data, min, max, report);
}

BaseProperty* CreateLimitedReport(const std::string& name, const std::string& description, int& data, int min, int max, Reportable* report)
{
	return new LimitedReportProperty<int>(BaseProperty::Int, name, description, data, min, max, report);
}

} }