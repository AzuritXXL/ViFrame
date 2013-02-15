#include "stdafx.h"
// ...
#include "BaseProperty.hpp"

namespace BaseLib {
namespace Properties {

BaseProperty::BaseProperty(eTypes type, const std::string& name, const std::string& description)
	: mType(type), mName(name), mDescription(description)
{ }

BaseProperty::~BaseProperty()
{ }

BaseProperty::eTypes BaseProperty::GetType() const throw()
{
	return mType;
}

const std::string& BaseProperty::GetName() const throw()
{
	return mName;
}

const std::string& BaseProperty::GetDescription() const throw()
{
	return mDescription;
}

} }