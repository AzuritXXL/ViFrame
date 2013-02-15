#include "stdafx.h"
// ...
#include "IntSelectionReport.hpp"

namespace BaseLib {
namespace Properties {

IntSelectionReport::IntSelectionReport(const std::string& name, const std::string& description, int& data, IntSelection::ValuesType& values, Reportable *report)
	: IntSelection(name, description, data, values), mReport(report)
{ }

IntSelectionReport::IntSelectionReport(const std::string& name, const std::string& description, int data, const IntSelection::ValuesType& values, Reportable *report)
	: IntSelection(name, description, data, values), mReport(report)
{ }

bool IntSelectionReport::SetProperty(const int& data, std::string& log)
{
	// call base implementation
	bool result = IntSelection::SetProperty(data, log);
	if (result)
	{	// report change 
		if (mReport != nullptr)
		{
			mReport->OnChange(this);
		}
		return true;
	}
	else
	{
		return false;
	}	
}

BaseProperty* CreateIntSelectionReport(const std::string& name, const std::string& description, int& data, IntSelection::ValuesType& values, Reportable *report)
{
	return new IntSelectionReport(name, description, data, values, report);
}

BaseProperty* CreateIntSelectionReport(const std::string& name, const std::string& description, int& data, const IntSelection::ValuesType& values, Reportable *report)
{
	return new IntSelectionReport(name, description, data, values, report);
}

} } 
