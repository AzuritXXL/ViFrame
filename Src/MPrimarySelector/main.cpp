#include "stdafx.h"
// Std
#include <algorithm>
// ...
#include "main.h"
// BaseLib
#include "Module.hpp"
#include "Socket.hpp"
#include "Property.hpp"
#include "PropertyCreator.hpp"
#include "Description.hpp"
#include "RandomAcces.hpp"
#include "SequentialAcces.hpp"
#include "Exception.hpp"
#include "LimitedReportProperty.hpp"

PrimarySelector::PrimarySelector()
	: mPrimaryAtt(nullptr), mSequentialSourceAcces(nullptr), mRandomSourceAcces(nullptr)
{
	// for now we left mOutputSocket clear ...

	// input sockets
	BaseLib::Sockets::Socket* inSocket = new BaseLib::Sockets::Socket("Input", "", BaseLib::Interfaces::SequentialAcces::Identification | BaseLib::Interfaces::RandomAcces::Identification);
	mInputSockets.push_back(inSocket);

	// properties
	std::vector<std::string> empty;
	mPrimaryAtt = dynamic_cast<BaseLib::Properties::IntSelectionReport*>(BaseLib::Properties::CreateIntSelectionReport("Primary name: ", "Name of attribute that becomes primary", mPrimaryAttIndex, empty, this) );
	mProperties.push_back(mPrimaryAtt);
}

void PrimarySelector::UpdateProperty()
{	// update property selectable values ..
	if (GetSocket(0) == nullptr)
	{	// clear values for selection
		mPrimaryAtt->GetValues().clear();
		mPrimaryAttIndex = -1;
		// update primary .. 
		mNewPrimary.Name = "";
		mNewPrimary.Att.Level = -1;
		mNewPrimary.Att.Type = 0;
		return;
	}
	// we have socket .. so prepare data ... 
	auto newSourceSocket = GetSocket(0)->GetOutputSocket();

	// check if source socket changed
	if (mPrimarySourceSocket.GetPrimary().Name == newSourceSocket.GetPrimary().Name &&
		mPrimarySourceSocket.GetPrimary().Att.Level == newSourceSocket.GetPrimary().Att.Level &&
		mPrimarySourceSocket.GetPrimary().Att.Type == newSourceSocket.GetPrimary().Att.Type &&
		mPrimarySourceSocket.GetData() == newSourceSocket.GetData() )
	{	// ouput socket of our sorce is the same as we have stored
		return;
	}

	// otherwise .. copy data from new socket .. 
	mPrimarySourceSocket.Copy( GetSocket(0)->GetOutputSocket() );

	// create new representation .. 
	mPrimaryAtt->GetValues().clear();
	BaseLib::Properties::IntSelectionReport::ValuesType& sellectableValues = mPrimaryAtt->GetValues();
	
	// add primary
	{
		std::stringstream ss;
		ss << mPrimarySourceSocket.GetPrimary().Att.Level << '.' << mPrimarySourceSocket.GetPrimary().Name;
		// add
		sellectableValues.push_back(ss.str());
	}
	// add secondary
	std::for_each(std::begin(mPrimarySourceSocket.GetData()), std::end(mPrimarySourceSocket.GetData()),
		[&] (BaseLib::Sockets::Socket::DataType::const_reference named)
		{
			std::for_each(std::begin(named.second), std::end(named.second),
				[&] (BaseLib::Sockets::Socket::DataType::value_type::second_type::const_reference item)
				{	// create name
					std::stringstream ss;
					ss << item.Level << '.' << named.first;
					// add
					sellectableValues.push_back(ss.str());
				}
			);
		}
	);

// determine new value for mPrimaryAttIndex

	// as selected mark 0
	mPrimaryAttIndex = 0; // -> on this place is primary 
	// selection change 
	mNewPrimary = mPrimarySourceSocket.GetPrimary(); // so set active as a primary
	// is called from OutputSocketValidation ... socket update will continue .. 
}

void PrimarySelector::UpdatePrimary()
{	// set mNewPrimary
	int level;
	char dot;
	std::string name;

	std::stringstream ss;
	ss << mPrimaryAtt->GetValues().at(mPrimaryAttIndex);

	ss >> level;
	ss >> dot;
	ss >> name;
	// now we have to find type .. 
	int type = 0;

	// primary
	auto primary = mPrimarySourceSocket.GetPrimary();
	if (primary.Name == name && primary.Att.Level == level)
	{	// primary 
		mNewPrimary = primary;
		return;
	}

	if (mPrimarySourceSocket.GetData().count(name) == 0)
	{	// invalid name -> use old primary
		ReportMessage(BaseLib::iReportable::Warning, "Wrong new primary name.");
		mNewPrimary = primary;
		return;
	}

	// secondary
	std::for_each(std::begin(mPrimarySourceSocket.GetData().at(name)), std::end(mPrimarySourceSocket.GetData().at(name)),
		[&] (BaseLib::Sockets::Socket::DataType::value_type::second_type::const_reference item)
		{	
			if (item.Level == level)
			{	// we've got it 
				mNewPrimary.Name = name;
				mNewPrimary.Att = item;
				return;
			}
		}
	);
	// in mNewPrimary is new primary .. 
}

//									BaseLib::iModule											//

const int PrimarySelector::GetOutputInterfaces()
{	// pass source interface .. 
	if (GetSocket(0) == nullptr)
	{
		return 0;
	}
	else
	{
		return GetSocket(0)->GetOutputSocket().GetInterface() & 
			( BaseLib::Interfaces::SequentialAcces::Identification | BaseLib::Interfaces::RandomAcces::Identification );
	}
}

void* PrimarySelector::AccessInterface(const int& interfaceId)
{	// we allow acces to both interfaces, but check according to mode .. 
	if (interfaceId == BaseLib::Interfaces::SequentialAcces::Identification)
	{
		return dynamic_cast<BaseLib::Interfaces::SequentialAcces*>(this);
	}
	if (interfaceId == BaseLib::Interfaces::RandomAcces::Identification)
	{
		return dynamic_cast<BaseLib::Interfaces::RandomAcces*>(this);
	}
	return nullptr;
}

bool PrimarySelector::PrepareForConnection()
{	// we need connected source
	if (GetSocket(0) == nullptr)
	{
		return false;
	}
	else
	{
		return GetSocket(0)->PrepareForConnection();
	}
}

bool PrimarySelector::CheckIntegrity()
{	// we know that we have valid input socket and we have valid selected item .. 
	if (GetSocket(0) == nullptr)
	{
		return false;
	}
	else
	{
		return GetSocket(0)->CheckIntegrity();
	}	
}

void PrimarySelector::innerOpen()
{	// test that we hase socket set ( for safety )
	if (GetSocket(0) == nullptr)
	{
		ReportMessage(BaseLib::iReportable::Error, "Source socket has value nullptr.");
		throw std::exception();
	}
	BaseLib::ExceptionHolder ex;
	GetSocket(0)->Open(ex);
	ex.Rethrow();

	// get interfaces .. 
	if ((GetSocket(0)->GetOutputSocket().GetInterface() & BaseLib::Interfaces::SequentialAcces::Identification )
		 == BaseLib::Interfaces::SequentialAcces::Identification )
	{
		mSequentialSourceAcces = reinterpret_cast<BaseLib::Interfaces::SequentialAcces*>
			(GetSocket(0)->AccessInterface(BaseLib::Interfaces::SequentialAcces::Identification) );
	}
	else
	{	// must be Random
		mRandomSourceAcces = reinterpret_cast<BaseLib::Interfaces::RandomAcces*>
			(GetSocket(0)->AccessInterface(BaseLib::Interfaces::RandomAcces::Identification) );
	}
	// and prepare mPrimaryAttName
	std::stringstream ss;
	ss << mNewPrimary.Att.Level << '.' << mNewPrimary.Name;
	mPrimaryAttName = ss.str();
}

void PrimarySelector::innerPrepare()
{
	if (GetSocket(0) == nullptr)
	{
		ReportMessage(BaseLib::iReportable::Error, "Source socket has value nullptr.");
		throw std::exception();
	}
	BaseLib::ExceptionHolder ex;
	GetSocket(0)->Prepare(ex);
	ex.Rethrow();
}

void PrimarySelector::innerClose()
{
	if (GetSocket(0) != nullptr)
	{
		GetSocket(0)->Close();
	}
	else
	{	// on closing we don't report anything ( errors )
	}
	mSequentialSourceAcces = nullptr;
	mRandomSourceAcces = nullptr;
}

void PrimarySelector::ValidateOutputSocket()
{	// update property, and value if need, source could changed .. 
	UpdateProperty();
	UpdatePrimary();

	// now ack to work .. 
	if (GetSocket(0) == nullptr)
	{	// we know that mOutputSocketAdd is empty
		mOutputSocketPublic = mOutputSocketAdd;
	}
	else
	{
		mOutputSocketPublic.Copy( GetSocket(0)->GetOutputSocket() );
		// add old primary into ordinary data .. 
		BaseLib::Sockets::Socket::PrimaryType oldPrim = mOutputSocketPublic.GetPrimary();
		mOutputSocketPublic.AddData(oldPrim.Name , oldPrim.Att.Level, oldPrim.Att.Type);

		// remove future primary data from secondary 
		BaseLib::Sockets::Socket::DataType::value_type::second_type& named = mOutputSocketPublic.GetDataFull()[mNewPrimary.Name];
		auto newEnd = std::remove(std::begin(named), std::end(named), mNewPrimary.Att);
		named.erase(newEnd, std::end(named));

		// mark new main data
		mOutputSocketPublic.SetPrimary(mNewPrimary.Name, mNewPrimary.Att.Level, mNewPrimary.Att.Type);

		// output interfaces ..
		mOutputSocketPublic.SetInterface( mOutputSocketPublic.GetInterface() & 
			(BaseLib::Interfaces::SequentialAcces::Identification | BaseLib::Interfaces::RandomAcces::Identification) );
	}
}

void PrimarySelector::PropertyLoadSecondWave()
{	// update property values
	UpdateProperty();
}

//							BaseLib::Interfaces::SequentialAcces								//

bool PrimarySelector::Next(BaseLib::ExceptionHolder& ex)
{	
	if ( mSequentialSourceAcces->Next(ex) )
	{	// ok we have prepared data
		ex.Rethrow();
		return true;
	}
	else
	{
		ex.Rethrow();
		return false;
	}
}

BaseLib::Objects::Object* PrimarySelector::GetObject(BaseLib::ExceptionHolder& ex)
{
	// we will rehtrow if exception occure
	BaseLib::Objects::Object* obj = mSequentialSourceAcces->GetObject(ex);
	ex.Rethrow();

	// set main, we have exact name .. 
	obj->SetMainPure(mPrimaryAttName);

	return obj;
}

//							BaseLib::Interfaces::RandomAcces									//

size_t PrimarySelector::GetCount() const
{
	return mRandomSourceAcces->GetCount();
}

BaseLib::Objects::Object* PrimarySelector::GetObject(const size_t& index, BaseLib::ExceptionHolder& ex)
{
	// we will rehtrow if exception occure
	BaseLib::Objects::Object* obj = mRandomSourceAcces->GetObject(index, ex);
	ex.Rethrow();

	// set main, we have exact name .. 
	obj->SetMainPure(mPrimaryAttName);

	return obj;
}

void PrimarySelector::ClearOutState()
{	
	mRandomSourceAcces->ClearOutState();
}
	
//							BaseLib::Properties::Reportable										//

void PrimarySelector::OnChange(BaseLib::Properties::BaseProperty* prop)
{	
	// validate socket .. 
	ValidateOutputSocket();
	// and report change
	EmitTargetControl();
}

//										Module access											//

_DYNLINK void* CreateModule() 
{ 
	return new PrimarySelector(); 
}

_DYNLINK void DeleteModule(void* module) 
{
	PrimarySelector* convModule = reinterpret_cast<PrimarySelector*>(module);
	delete convModule;
}

BaseLib::Description locDescription(std::pair<int, int>(1,0), 
	"PrimarySelector",
	"Primary selector enable changing main attribute.",
	"Primary selector enable changing main attribute to any of source specified attribute."
	);

_DYNLINK const BaseLib::Description* GetDescription() 
{ 
	return &locDescription; 
}