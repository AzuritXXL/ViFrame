#include "stdafx.h"

#include "main.h"
// ..
#include <cstdlib> 
#include <set>
#include <time.h>
#include <string>
#include <sstream>
// ...
#include "Description.hpp"
#include "PropertyCreator.hpp"
#include "Exception.hpp"
#include "Variable.hpp"
#include "Variant.hpp"

Uniq::Uniq()
	: mAcces(nullptr), mRemoveLower(false)
{
	ValidateOutputSocket();


	// input socket specfication
	auto mainSocket = new BaseLib::Sockets::Socket("Input", "", BaseLib::Interfaces::RandomAcces::Identification | BaseLib::Interfaces::SequentialAcces::Identification);
	// we set requirments for main data
	mainSocket->SetPrimary("Main data", BaseLib::Objects::Int().GetType());
	// set input socket specification
	mInputSockets.push_back(mainSocket);

	// property
	mProperties.push_back(BaseLib::Properties::PropertyCreator::Create("Remove lower values: ", "If checked object with same name and lowe id are removed.", mRemoveLower));
}

void Uniq::LoadData()
{
	// report that we are working
	ReportMessage(BaseLib::iReportable::Information, "Working ...");
	
	// count in objects
	int inCount = 0;

	BaseLib::ExceptionHolder ex;
	while (mAcces->Next(ex))
	{
		// count input objects
		inCount++;
		// get objects
		ex.Rethrow();
		BaseLib::Objects::Object* object = mAcces->GetObject(ex);
		ex.Rethrow();
		// get object main value -> age
		BaseLib::Objects::Int& age = *dynamic_cast<BaseLib::Objects::Int*>( object->GetMain() );

		// check if registred
		if (mRegister.count(object->GetName()) == 0)
		{	// not registred -> register and add
			mRegister[object->GetName()] = Record(age.Data, mStorage.size() );
			// add object
			mStorage.push_back( ObjectType(object) );
		}
		else
		{	// registred .. check if add or drop
			if (mRegister[object->GetName()].Age > age.Data)
			{	// new is lower
				if (mRemoveLower)
				{	// new is lower -> remove new
					delete object;
					object = nullptr;
					continue;
				}
				else
				{	// old is higher -> we want keep lower -> remove old
					delete mStorage[mRegister[object->GetName()].Position].Object;
					// add new
					mStorage[mRegister[object->GetName()].Position].Object = object;
					// save new age
					mRegister[object->GetName()].Age = age.Data;
				}
			}
			else
			{
				if (mRemoveLower)
				{	// new is higher -> we remove lower -> remove old
					delete mStorage[mRegister[object->GetName()].Position].Object;
					// add new
					mStorage[mRegister[object->GetName()].Position].Object = object;
					// save new age
					mRegister[object->GetName()].Age = age.Data;
				}
				else
				{	// new is higher -> we remowe higher -> remove new
					delete object;
					object = nullptr;
					continue;
				}
			}

			// when drop (delete object ) we will delete data -> source module will have invalid pointer, 
			// but he should know that data is not his anymore and should not touch them
		}		
	}

	std::stringstream ss;
	ss << "In objects count: " << inCount << " Out objects count: " << mStorage.size();
	// report output statistics
	ReportMessage(BaseLib::iReportable::Information, ss.str());
}

size_t Uniq::GetCount() const
{
	return mStorage.size();
}

BaseLib::Objects::Object* Uniq::GetObject(const size_t& index, BaseLib::ExceptionHolder& ex)
{	// all work is done in innerPrapeer we just return results .. 
	if (index < 0 || index >= mStorage.size())
	{
		return nullptr;
	}
	// mark object ( we give up ownership)
	mStorage[index].Owned = false;
	return mStorage[index].Object;
}

void Uniq::ClearOutState()
{	// all objects are back in our ownership
	std::for_each( std::begin(mStorage), std::end(mStorage),
		[&] (std::vector<ObjectType>::reference item)
		{
			item.Owned = true;
		}
	);
}

const int Uniq::GetOutputInterfaces()
{
	return BaseLib::Interfaces::RandomAcces::Identification;
}

void* Uniq::AccessInterface(const int& interfaceId)
{
	if (interfaceId == BaseLib::Interfaces::RandomAcces::Identification)
	{
		return dynamic_cast<BaseLib::Interfaces::RandomAcces*>(this);
	}
	return nullptr;
}

void Uniq::innerOpen()
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

void Uniq::innerPrepare()
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

	// load data .. 
	LoadData();
}

void Uniq::innerClose()
{
	if (GetSocket(0) != nullptr)
	{
		GetSocket(0)->Close();
	}
	mAcces = nullptr;

	// -> owned objects will be delete when clearing storage
	mStorage.clear();
	mRegister.clear();
}

bool Uniq::CheckIntegrity()
{	// we only require all source connected
	if (GetSocket(0) == nullptr)
	{
		ReportMessage(BaseLib::iReportable::Error, "Not all source connected.");
		return false;
	}
	return GetSocket(0)->CheckIntegrity();
}

void Uniq::ValidateOutputSocket()
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

	// set output socket interface
	mOutputSocketPublic.SetInterface(BaseLib::Interfaces::RandomAcces::Identification);
}

_DYNLINK void* CreateModule() 
{ 
	return new Uniq(); 
}

_DYNLINK void DeleteModule(void* module) 
{
	Uniq* convModule = reinterpret_cast<Uniq*>(module);
	delete convModule;
}

BaseLib::Description locDescription(std::pair<int, int>(1,0), 
	"Uniq",
	"Filter objects.",
	"Filter objects, if there are two objects with same name, object with higher/lower main value is removed."
	);

_DYNLINK const BaseLib::Description* GetDescription() 
{ 
	return &locDescription; 
}