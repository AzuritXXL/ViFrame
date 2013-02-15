#include "stdafx.h"
// ...
#include "main.h"
// BaseLib
#include "Module.hpp"
#include "Socket.hpp"
#include "Property.hpp"
#include "PropertyCreator.hpp"
#include "Description.hpp"
#include "SequentialAcces.hpp"
#include "Exception.hpp"
#include "LimitedReportProperty.hpp"

Merger::Merger()
	: mSocketCount(3), mNameMapper(), mPassCount(0)
{
	// set specification for output socket
	mOutputSocketAdd.SetInterface(BaseLib::Interfaces::SequentialAcces::Identification);

	// set input socket specification	
	mNameMapper.resize(3);
	mInputSockets.push_back(new BaseLib::Sockets::Socket("Input", "Input to merge, require sequential acces.", BaseLib::Interfaces::SequentialAcces::Identification));
	mInputSockets.push_back(new BaseLib::Sockets::Socket("Input", "Input to merge, require sequential acces.", BaseLib::Interfaces::SequentialAcces::Identification));
	mInputSockets.push_back(new BaseLib::Sockets::Socket("Input", "Input to merge, require sequential acces.", BaseLib::Interfaces::SequentialAcces::Identification));
	// prefixes
	mProperties.push_back(BaseLib::Properties::CreateLimitedReport("Number of inputs: ", "Number of input sockets.", mSocketCount, 1, 1024, this));

	// validate output socket
	ValidateOutputSocket();
}

int Merger::CountConnected() const
{
	int count = 0;
	// add 1 for each connected socket
	for (size_t i = 0; i < mInputSockets.size(); ++i)
	{
		count += GetSocket(i) == nullptr ? 0 : 1;
	}
	return count;
}

inline std::string Merger::CreateName(int level, const std::string& name) const
{
	std::stringstream ss;
	ss << level << '.' << name;
	return ss.str();
}

void Merger::MergeSecondary(const BaseLib::Sockets::Socket::DataType& inSocket, MergerSocket& outSocket, Merger::InnerNameMapperType& translationMap) const
{
	std::for_each(std::begin(inSocket), std::end(inSocket),
		[&] (BaseLib::Sockets::Socket::DataType::const_reference named)
		{	// we now hold group of named types
			std::for_each(std::begin(named.second), std::end(named.second), 
				[&] (BaseLib::Sockets::Socket::DataType::value_type::second_type::const_reference item)
				{	// item hold std::pair<level, type >

					if (outSocket.count(named.first) == 0)
					{	// we dont know name, create representation ( 1, type )
						outSocket[named.first].push_back( Merger::InnerMergerSocket(1, item.Type) );
						// translation
						translationMap[CreateName(item.Level, named.first)] = "1." + named.first;
						return;
					}
					else
					{	// we already know name, check if we know type
						auto iter = std::begin(outSocket[named.first]);
						auto iterEnd = std::end(outSocket[named.first]);
						for (;iter != iterEnd; ++iter)
						{
							if ( (*iter).Type == item.Type)
							{	// type match -> is in named mote with there types  ?								
								auto innerIter = std::begin(outSocket[named.first]);
								for (;innerIter != iterEnd; ++innerIter)
								{	
									if (iter->Type == innerIter->Type && iter != innerIter)
									{	// we found data with same type -> add as new
										outSocket[named.first].push_back( Merger::InnerMergerSocket(outSocket[named.first].size() + 1, item.Type) );
										// translation
										translationMap[CreateName(item.Level, named.first)] = 
											CreateName(outSocket[named.first].size(), named.first);
										return;
									}
								}
								// no data with same type -> add under exist -> increase number of usage 
								(*iter).NumberOfUsage++;
								// translation
								translationMap[CreateName(item.Level, named.first)] = 
									CreateName((*iter).Level, named.first);
								return;
							}
						}
						// type is unknow, add on (level + 1, type )
						outSocket[named.first].push_back( Merger::InnerMergerSocket(outSocket[named.first].size() + 1, item.Type) );
						// translation
						translationMap[CreateName(item.Level, named.first)] = 
							CreateName(outSocket[named.first].size(), named.first);
						return;
					}
				}
			);
		}
	);
}

//									BaseLib::iModule											//

bool Merger::Connect(iModule* module, size_t socket)
{
	if (BaseLib::iModule::Connect(module, socket))
	{	// something changed 
		if (module == nullptr)
		{	// last disconnect ? -> change input socket requrments
			if (CountConnected() == 0)
			{
				auto iter = std::begin(mInputSockets);
				auto iterEnd = std::end(mInputSockets);
				for (;iter != iterEnd; ++iter)
				{	// no requirments .. 
					(*iter)->SetPrimary("", 0);
				}
			}
		}	
		else
		{	// first connect ? -> change input socket requrments
			if (CountConnected() == 1)
			{	// we know who it was .. 
				int primaryType = module->GetOutputSocket().GetPrimaryType();				
				
				auto iter = std::begin(mInputSockets);
				auto iterEnd = std::end(mInputSockets);
				for (;iter != iterEnd; ++iter)
				{	// requirments same as first connected .. 
					(*iter)->SetPrimary("primary data", primaryType);
				}				
			}
		}
		return true;
	}
	else
	{	// connection refused 
		return false;
	}
}

const int Merger::GetOutputInterfaces()
{
	return BaseLib::Interfaces::SequentialAcces::Identification;
}

void* Merger::AccessInterface(const int& interfaceId)
{
	if (interfaceId == BaseLib::Interfaces::SequentialAcces::Identification)
	{
		return dynamic_cast<BaseLib::Interfaces::SequentialAcces*>(this);
	}
	return nullptr;
}

bool Merger::PrepareForConnection()
{	// we need only one connected source
	size_t socketCount = GetInputSockets().size();
	for (size_t i = 0; i < socketCount; ++i)
	{
		if (GetSocket(i) == nullptr)
		{	// no connection
		}
		else
		{
			return true;
		}
	}
	return false;
}

bool Merger::CheckIntegrity()
{	// to be valid we need at least one source and all our source must be valit
	bool sourceFound = false;
	size_t socketCount = GetInputSockets().size();
	for (size_t i = 0; i < socketCount; ++i)
	{
		if (GetSocket(i) == nullptr)
		{	// no connection
		}
		else
		{
			if (GetSocket(i)->CheckIntegrity())
			{	// alright
				sourceFound = true;
			}
			else
			{	// something wrong ..
				return false;
			}
		}
	}

	// input sockets types are ok, we check them through time .. 
	
	if (sourceFound)
	{
		return true;
	}
	else
	{
		ReportMessage(BaseLib::iReportable::Error, "Atleast one source must be connected.");
		return false;
	}
}

void Merger::innerOpen()
{	// open sources
	BaseLib::ExceptionHolder ex;
	size_t socketCount = GetInputSockets().size();
	for (size_t i = 0; i < socketCount; ++i)
	{
		if (GetSocket(i) == nullptr)
		{	// no connection
		}
		else
		{	// open and if exception occure rethrow
			GetSocket(i)->Open(ex);
			ex.Rethrow();
		}
	}
	// and we set active module to zero
	mActiveModule = 0;
}

void Merger::innerPrepare()
{	// preapare sources
	BaseLib::ExceptionHolder ex;
	size_t socketCount = GetInputSockets().size();
	for (size_t i = 0; i < socketCount; ++i)
	{
		if (GetSocket(i) == nullptr)
		{	// no connection
		}
		else
		{	// prepare and if exception occure rethrow
			GetSocket(i)->Prepare(ex);
			ex.Rethrow();
		}
	}
}

void Merger::innerClose()
{	// close sources
	size_t socketCount = GetInputSockets().size();
	for (size_t i = 0; i < socketCount; ++i)
	{
		if (GetSocket(i) == nullptr)
		{	// no connection
		}
		else
		{
			GetSocket(i)->Close();
		}
	}
}

void Merger::ValidateOutputSocket()
{	// we start witch one of source socket

	bool start = true;
	size_t socketCount = GetInputSockets().size();

	// data structures for merge 
	std::pair<std::string, int> primary;
	MergerSocket outSocketSecondary; 
	
	for (size_t i = 0; i < socketCount; ++i)
	{
		if (GetSocket(i) == nullptr)
		{	// no connection
		}
		else
		{
			const BaseLib::Sockets::Socket& inSocket = GetSocket(i)->GetOutputSocket();
			// validate out source socket ? no -> module is responsible to keep Socket valid if any change occure
			if (start)
			{	// first socket, get data ..				
				start = false;
				// from primary just name and type .. pass is not in our concenr .. 
				primary.first = inSocket.GetPrimary().Name;
				primary.second = inSocket.GetPrimary().Att.Type;
				// mapper
				mNameMapper[i][CreateName(inSocket.GetPrimary().Att.Level, primary.first)] = "0." + primary.first;
				mNameMapper[i]["_PRIMARY"] = primary.first; // just name, 0. will be whne use to mark data
				// secondary data ..
				MergeSecondary(inSocket.GetData(), outSocketSecondary, mNameMapper[i]);
			}
			else
			{	// another socket
				const BaseLib::Sockets::Socket& inSocket = GetSocket(i)->GetOutputSocket();
				// check just primary type 
				if (inSocket.GetPrimary().Att.Type != primary.second)
				{	// specification miss match ..
					primary.first = "";
					primary.second = false;
					ReportMessage(BaseLib::iReportable::Error, "Error during socket validation. Wrong primary data !");
				}
				// mapper
				mNameMapper[i][CreateName(inSocket.GetPrimary().Att.Level, primary.first)] = "0." + primary.first;
				mNameMapper[i]["_PRIMARY"] = primary.first; // just name, 0. will be whne use to mark data
				// secondary data
				MergeSecondary(inSocket.GetData(), outSocketSecondary, mNameMapper[i]);		
			}
		}
	}

	// we transfer data from outSocketSecondary to mOutputSocketPublic
	mOutputSocketPublic.Copy(mOutputSocketAdd);	// use add as a template for text
	int inputSocketCount = CountConnected();

	mPassCount = 0;	// start with zero pass count
	std::for_each(std::begin(outSocketSecondary), std::end(outSocketSecondary), 
		[&] (MergerSocket::reference named)
		{
			std::for_each(std::begin(named.second), std::end(named.second), 
				[&] (const InnerMergerSocket& item)
				{
					if (item.NumberOfUsage == inputSocketCount)
					{	// used by all sockets .. we can specify on output
						mOutputSocketPublic.AddData(named.first, item.Level, item.Type);
					}
				}
			);
			// number of children determine min mPassCount, no mather if data are used in output socket
			mPassCount = mPassCount < named.second.size() ? named.second.size() : mPassCount;
		}
	);
	// add primary
	mOutputSocketPublic.SetPrimary(primary.first, primary.second);
	// set pass count and interface
	mOutputSocketPublic.SetPassCounter(mPassCount);
	mOutputSocketPublic.SetInterface(BaseLib::Interfaces::SequentialAcces::Identification);

	// we increase mPassCount, becouse while Socket::Socket will be increased before next use, Objcets::Object
	// will not so this will erase the diference 
	mPassCount++;
}

//							BaseLib::Interfaces::SequentialAcces								//

bool Merger::Next(BaseLib::ExceptionHolder& ex)
{	// find non null socket, or stay on active one
	while ( GetSocket(mActiveModule) == nullptr )
	{
		mActiveModule++;
		if (mActiveModule == mSocketCount)
		{	// we have no more socket to try
			return false;
		}
	}

	try 
	{
		auto seq = reinterpret_cast<BaseLib::Interfaces::SequentialAcces*>( 
			GetSocket(mActiveModule)->AccessInterface(BaseLib::Interfaces::SequentialAcces::Identification) );

		if ( seq->Next(ex) )
		{	// ok we have prepared data
			ex.Rethrow();
			return true;
		}
		else
		{
			if (mActiveModule + 1< mSocketCount)
			{	// we can move on next socket
				mActiveModule++;
				// we call Next recursively ... that will secure finding new source
				bool result = Next(ex);
				ex.Rethrow();
				return result;
			}
			else
			{	// we running out of sockets
				return false;
			}
		}
	}
	catch(std::exception e)
	{
		ex(e);
		return false;
	}
}

BaseLib::Objects::Object* Merger::GetObject(BaseLib::ExceptionHolder& ex)
{
	if (mActiveModule < mSocketCount)
	{
		auto seq = reinterpret_cast<BaseLib::Interfaces::SequentialAcces*>( 
			GetSocket(mActiveModule)->AccessInterface(BaseLib::Interfaces::SequentialAcces::Identification) );
	
		// we will rehtrow if exception occure
		BaseLib::Objects::Object* obj = seq->GetObject(ex);
		ex.Rethrow();

		// create new object
		BaseLib::Objects::Object* newObj = new BaseLib::Objects::Object(obj->GetName());

		// move and rename data
		InnerNameMapperType& mapper = mNameMapper[mActiveModule];

		auto iter = std::begin(obj->GetData());
		auto iterEnd = std::end(obj->GetData());		
		for (;iter != iterEnd; ++iter)
		{	// move data 
			if (mapper.count(iter->first) == 0)
			{	// unspecified data, will be delete
				delete iter->second;
			}
			else
			{	// expected data .. move and rename 
				newObj->AddPure(mapper[iter->first], iter->second);
			}
		}
		// remove pointer to dat from old object
		obj->GiveUp();

		// set main ( name is store in _PRIMARY ) prefix 0. will be add
		newObj->SetMain(mNameMapper[mActiveModule]["_PRIMARY"]);
		// and passCounter 
		// ( and becouse we alreasy increase mPassCount in SocketValidation, we dont have to call Pass on Object)
		newObj->SetPass(mPassCount);

		// delete old object
		delete obj;
		// return new object
		return newObj;
	}
	else
	{	// we are out of active socket .. 
		return nullptr;
	}
}

//							BaseLib::Properties::Reportable										//

void Merger::OnChange(BaseLib::Properties::BaseProperty* prop)
{	// number of output sockets changed .. 
	if (mSocketCount == mInputSockets.size())
	{	// no change .. 
		return;
	}

	if (mSocketCount < mInputSockets.size())
	{
		// disconnect all above
		for (size_t index = mSocketCount; index < mInputSockets.size(); ++index)
		{	// disconnect .. 
			if ( GetSocket(index) == nullptr)
			{	// nothing to disconnect
			}
			else
			{
				GetSocket(index)->OnDisconnect(this);
			}
			// delete socket information
			delete mInputSockets[index];
			mInputSockets[index] = nullptr;
		}
		// fit for new size -> remove old one 
		mInputSockets.resize(mSocketCount);
	}
	else
	{	// create new		
		size_t originalSize = mInputSockets.size();
		// fit for new size
		mInputSockets.resize(mSocketCount);
		for (size_t index = originalSize; index < mSocketCount; ++index)
		{	// add data
			mInputSockets[index] = new BaseLib::Sockets::Socket("Input", "Input to merge, require sequential acces.", BaseLib::Interfaces::SequentialAcces::Identification);
		}
	}
	// and resize mapper
	mNameMapper.resize(mSocketCount);
	// validate socket .. 
	ValidateOutputSocket();
	// and report change
	EmitTargetControl();
}

//										Module access											//

_DYNLINK void* CreateModule() 
{ 
	return new Merger(); 
}

_DYNLINK void DeleteModule(void* module) 
{
	Merger* convModule = reinterpret_cast<Merger*>(module);
	delete convModule;
}

BaseLib::Description locDescription(std::pair<int, int>(1,0), 
	"Merger",
	"Merge more outputs into one. Source data must not contain attribute called _PRIMARY.",
	"Merge more outputs into one. Source data must not contain attribute called _PRIMARY."
	"When merging merger try to compare inputs and match data names and type."
	);

_DYNLINK const BaseLib::Description* GetDescription() 
{ 
	return &locDescription; 
}