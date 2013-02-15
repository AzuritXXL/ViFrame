#include "stdafx.h"
// ...
#include "IntSelection.hpp"

namespace BaseLib {
namespace Properties {

IntSelection::IntSelection(const std::string& name, const std::string& description, int& data, ValuesType& values)
	: Property(BaseProperty::IntSelection, name, description, data)
{
	std::swap(mValues, values);
}

IntSelection::IntSelection(const std::string& name, const std::string& description, int data, const ValuesType& values)
	: Property(BaseProperty::IntSelection, name, description, data), mValues(values)
{ }

IntSelection::ValuesType& IntSelection::GetValues()
{
	return mValues;
}

bool IntSelection::SetProperty(const int& data, std::string& log)
{
	if (data < 0 || data >= mValues.size())
	{	// prepare error message
		log = "Selection index out of range";
		return false;
	}
	else
	{	// call base implementation
		return Property::SetProperty(data, log);
	}
}

bool IntSelection::SetPropertyLoad(const int& data)
{
	mData = data;
	if (data < 0 || data >= mValues.size())
	{	// we need another check later
		return false;
	}
	else
	{
		return true;
	}
}

BaseProperty* CreateIntSelection(const std::string& name, const std::string& description, int& data, IntSelection::ValuesType& values)
{
	return new IntSelection(name, description, data, values);
}

BaseProperty* CreateIntSelection(const std::string& name, const std::string& description, int& data, const IntSelection::ValuesType& values)
{
	return new IntSelection(name, description, data, values);
}

} } 
