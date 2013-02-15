#include "stdafx.h"
// ...
#include "Loader.hpp"

namespace BaseLib {
namespace Modules {

Loader::Loader()
	: mObject(nullptr)
{ }

Loader::~Loader()
{
	// if we have an object which wasn't passed
	if (mObject != nullptr && !mObjectRelease)
	{
		delete mObject;
		mObject = nullptr;
	}
}

//									Interfaces::SequentialAcces									//

bool Loader::Next(ExceptionHolder& ex)
{	
	try
	{
		// take an object from the Generate function
		mObject = GenerateObject();
	}
	catch(Exception e)
	{
		ex(e);
		return false;
	}
	catch(std::exception e)
	{
		ex(e);
		return false;
	}
	// actual object wasn't passed
	mObjectRelease = false;
	// if object is valid return true
	return mObject != nullptr;
}

Objects::Object* Loader::GetObject(ExceptionHolder& ex)
{	// object passed
	mObjectRelease = true;
	return mObject;
}

//										BaseLib::iModule										//

const int Loader::GetOutputInterfaces()
{
	return (int)Interfaces::SequentialAcces::Identification;
}

void* Loader::AccessInterface(const int& interfaceId)
{
	switch(interfaceId)
	{
	case Interfaces::SequentialAcces::Identification:
		return dynamic_cast<Interfaces::SequentialAcces*>(this);
	default:
		break;
	}
	return nullptr;
}

bool Loader::CheckIntegrity() throw()
{	// we have no source, so we are always valid
	return true;
}

void Loader::ValidateOutputSocket()
{	// we have no inputs so just copy what we added
	mOutputSocketPublic.Copy(mOutputSocketAdd);
	mOutputSocketPublic.SetInterface(BaseLib::Interfaces::SequentialAcces::Identification);
}

} }