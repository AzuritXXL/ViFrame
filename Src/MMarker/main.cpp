#include "stdafx.h"

#include "main.h"
// ..
#include <cstdlib> 
#include <set>
#include <time.h>
// ...
#include "Description.hpp"
#include "PropertyCreator.hpp"
#include "Exception.hpp"
#include "Variable.hpp"
#include "Variant.hpp"

Marker::Marker()
	: mMarkName("mark"), mMarkValue(0), mAcces(nullptr)
{
	// add main socket
	auto mainSocket = new BaseLib::Sockets::Socket("Input", "",BaseLib::Interfaces::SequentialAcces::Identification);
	mInputSockets.push_back(mainSocket);

	// registre
	mProperties.push_back(::BaseLib::Properties::CreateReport("Name", "Name of added variable.", mMarkName, this) );
	mProperties.push_back(::BaseLib::Properties::PropertyCreator::Create("Value", "Value of added variable.", mMarkValue) );

	ValidateOutputSocket();
}

bool Marker::Next(BaseLib::ExceptionHolder& ex)
{
	return mAcces->Next(ex);
}

BaseLib::Objects::Object* Marker::GetObject(BaseLib::ExceptionHolder& ex)
{
	auto object = mAcces->GetObject(ex);
	ex.Rethrow();

	// add value
	auto value = new BaseLib::Objects::Int(mMarkValue);
	object->Add(mMarkName, value);
	object->Pass();

	return object;
}

void Marker::OnChange(BaseLib::Properties::BaseProperty* propoerty)
{
	ValidateOutputSocket();
}

const int Marker::GetOutputInterfaces()
{
	return BaseLib::Interfaces::SequentialAcces::Identification;
}

void* Marker::AccessInterface(const int& interfaceId)
{
	if (interfaceId == BaseLib::Interfaces::SequentialAcces::Identification)
	{
		return dynamic_cast<BaseLib::Interfaces::SequentialAcces*>(this);
	}
	return nullptr;
}

void Marker::innerOpen()
{
	if (GetSocket(0) == nullptr)
	{
		ReportMessage(BaseLib::iReportable::Error, "Source socket has value nullptr.");
		throw BaseLib::Exception();
	}
	BaseLib::ExceptionHolder ex;
	GetSocket(0)->Open(ex);
	ex.Rethrow();
}

void Marker::innerPrepare()
{
	if (GetSocket(0) == nullptr)
	{
		ReportMessage(BaseLib::iReportable::Error, "Source socket has value nullptr.");
		throw BaseLib::Exception();
	}
	BaseLib::ExceptionHolder ex;
	GetSocket(0)->Prepare(ex);
	ex.Rethrow();

	mAcces = reinterpret_cast<BaseLib::Interfaces::SequentialAcces*>
				(GetSocket(0)->AccessInterface(BaseLib::Interfaces::SequentialAcces::Identification));
}

void Marker::innerClose()
{
	if (GetSocket(0) != nullptr)
	{
		GetSocket(0)->Close();
	}
	mAcces = nullptr;
}

bool Marker::CheckIntegrity()
{	// we only require all source connected
	if (GetSocket(0) == nullptr)
	{
		ReportMessage(BaseLib::iReportable::Error, "Not all source connected.");
		return false;
	}
	return GetSocket(0)->CheckIntegrity();
}

void Marker::ValidateOutputSocket()
{	
	if (GetSocket(0) == nullptr)
	{	// nothing to add -> start from plain
		mOutputSocketPublic.Copy(mOutputSocketAdd);
	}
	else
	{
		mOutputSocketPublic.Copy(GetSocket(0)->GetOutputSocket());
		mOutputSocketPublic.IncreasePassCounter();
	}

	// add primary
	mOutputSocketPublic.AddData(mMarkName, BaseLib::Objects::Variant::Int);
	// set output socket interface
	mOutputSocketPublic.SetInterface(BaseLib::Interfaces::SequentialAcces::Identification);
}


_DYNLINK void* CreateModule() 
{ 
	return new Marker(); 
}

_DYNLINK void DeleteModule(void* module) 
{
	Marker* convModule = reinterpret_cast<Marker*>(module);
	delete convModule;
}

BaseLib::Description locDescription(std::pair<int, int>(1,0), 
	"Marker",
	"Mark objects.",
	"Add integral variable to object to mark them."
	);

_DYNLINK const BaseLib::Description* GetDescription() 
{ 
	return &locDescription; 
}