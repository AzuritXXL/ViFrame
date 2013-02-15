#include "stdafx.h"
// ...
#include "Main.hpp"
#include "Module.hpp"

namespace BaseLib {

iModule::iModule()
	: mModuleId(""), mTargets(), mOpen(false), mPrepare(false), mReportable(nullptr),
	  mProperties(), mOutputSocketAdd(), mOutputSocketPublic(), mInputSockets(), mValid(true)
{ }

iModule::~iModule()	
{
	// release properties
	std::for_each(std::begin(mProperties), std::end(mProperties),
		[] (PropertyType::reference item)
		{
			delete item;
			item = nullptr;
		}
	);
	mProperties.clear();
	// release sockets
	std::for_each(std::begin(mInputSockets), std::end(mInputSockets),
		[] (SocketsType::reference item)
		{
			delete item;
			item = nullptr;
		}
	);
	mInputSockets.clear();
}

const std::string& iModule::GetId() const
{
	return mModuleId;
}

void iModule::SetId(const std::string& id)
{
	mModuleId = id;
}

void iModule::SetReportable(iReportable* Reportable)
{
	mReportable = Reportable;
}

iReportable* iModule::GetReportable()
{
	return mReportable;
}

const iModule::SocketsType& iModule::GetInputSockets()
{
	return mInputSockets;
}

const Sockets::Socket& iModule::GetOutputSocket() const
{
	return mOutputSocketPublic;
}

const iModule::PropertyType& iModule::GetProperties()
{
	return mProperties;
}

Properties::BaseProperty* iModule::GetProperty(size_t index)
{
	if( index < 0 || index >= mProperties.size())
	{
		return nullptr;
	}
	return mProperties[index];
}

bool iModule::PrepareForConnection()
{
	// check whether all sources are connected
	SocketsType::const_iterator iter = std::begin(mInputSockets);
	SocketsType::const_iterator iterEnd = std::end(mInputSockets);
	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->mModule == nullptr)
		{
			return false;
		}
	}
	// all sources set
	return true;
}

bool iModule::Connect(iModule* module, size_t socket)
{
	if (socket < 0 || socket > mInputSockets.size())
	{	// index out of range
		ReportMessage(iReportable::Error, "Socket index is out of range.");
		return false;
	}

	if (module == nullptr)
	{	// we are disconnecting
		// report disconnect to source
		if (mInputSockets[socket]->mModule != nullptr)
		{
			mInputSockets[socket]->mModule->OnDisconnect(this);
		}
		mInputSockets[socket]->mModule = nullptr;

		// report all what we need ... 

		// revalidate output socket
		ValidateOutputSocket();

		// and report possible interface change .. 
		ReportInterfaceChanged();

		// In the end we inform other modules about our possible changes
		EmitTargetControl();

		return true;
	}

	// check requirements, if we don't ignore them ...
	if (!Sockets::Socket::Match(module->GetOutputSocket(), *mInputSockets[socket]))
	{	// requirments not met

		// create error report
		std::stringstream ss;
		ss << "Source module ( " << module->GetId();
		ss << " ) does not meet target requirements on socket " << socket;
		std::string message;
		std::getline(ss, message);
		ReportMessage(iReportable::Error, message);
		return false;
	}

	// reportable disconnect
	if (mInputSockets[socket]->mModule != nullptr)
	{
		mInputSockets[socket]->mModule->OnDisconnect(this);
	}

	//information whether the connection was sucessful
	bool result = true;
	// connect
	mInputSockets[socket]->mModule = module;
	// Reportable connect
	if (module != nullptr)
	{
		if (module->OnConnect(this, socket))
		{	// connection accept
//			result = true;
		}
		else
		{	// connection failed
			mInputSockets[socket]->mModule = nullptr;
			result = false;
		}
	}

	// revalidate output socket
	ValidateOutputSocket();
	// and report possible interface change .. 
	ReportInterfaceChanged();
	// in the end we inform other modules about our possible changes
	EmitTargetControl();

	return result;
}

bool iModule::OnConnect(iModule* module, size_t socket)
{	// base implementation just add to Reportable
	mTargets.push_back(module);
	// and then confirm connection
	return true;
}

void iModule::OnDisconnect(iModule* module)
{
	const std::vector<iModule*>::iterator iter =
		std::find_if(std::begin(mTargets), std::end(mTargets),
		[&] (iModule *item) { return item == module; }
	);

	if (iter == std::end(mTargets))
	{	// module has not been found
	}
	else
	{	// remove module from target list
		std::swap(mTargets.back(), *iter);
		mTargets.pop_back();
	}
}

void iModule::OnSourceChange()
{ // recheck all connections .. if some requirements are not valid any more, disconnect it
	bool changes = false;
	SocketsType::iterator iter = std::begin(mInputSockets);
	SocketsType::iterator iterEnd = std::end(mInputSockets);
	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->mModule == nullptr)
		{	// nothing to control		
		}
		else
		{	// we are here as a target .. 
			if (Sockets::Socket::Match((*iter)->mModule->GetOutputSocket(), *(*iter)))
			{	// connection valid
			}
			else
			{	// invalid connection, disconnectr
				(*iter)->mModule->OnDisconnect(this);
				(*iter)->mModule = nullptr;
				changes = true;
			}
		}
	}

	// we also revalidate output socket
	ValidateOutputSocket();

	if (changes)
	{	// if we changed something, Reportable changes
		ReportConnectionChanged();
	}

	// and for sure we Reportable possible interface change .. 
	ReportInterfaceChanged();

	// on the end we inform other modules about our possible changes
	EmitTargetControl();
}

void iModule::CutOff()
{	// we disconect all out sockets
	SocketsType::iterator iter = std::begin(mInputSockets);
	SocketsType::iterator iterEnd = std::end(mInputSockets);
	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->mModule != nullptr)
		{
			// repor disconnect
			(*iter)->mModule->OnDisconnect(this);
			// set socket to nullptr
			(*iter)->mModule = nullptr;
		}
	}

	// then we dissconect from targets
	std::for_each(std::begin(mTargets), std::end(mTargets), 
		[&](iModule* target) { target->OnCutOff(this); } );
	// and we are cutoff so we clear our mTarget list
	mTargets.clear();
}

void iModule::OnCutOff(iModule* module)
{
	// go through out socket and look for module
	SocketsType::iterator iter = std::begin(mInputSockets);
	SocketsType::iterator iterEnd = std::end(mInputSockets);
	for (; iter != iterEnd; ++iter)
	{
		// if we found it
		if ((*iter)->mModule == module)
		{	// break connection
			(*iter)->mModule = nullptr;
		}
	}
	// we report interface change
	ReportInterfaceChanged();
}

iModule* iModule::GetSocket(const size_t& socket)
{
	if (socket < 0 || socket >= mInputSockets.size())
	{
		return nullptr;
	}

	return mInputSockets[socket]->mModule;
}

const iModule* iModule::GetSocket(const size_t& socket) const
{
	if (socket < 0 || socket >= mInputSockets.size())
	{
		return nullptr;
	}

	return mInputSockets[socket]->mModule;
}

void iModule::Open(::BaseLib::ExceptionHolder& ex)
{
	if (mOpen)
	{	// already open

	}
	else
	{
		try
		{
			innerOpen();
		}
		catch(::BaseLib::Exception e)
		{
			ex(e);
			return;
		}
		catch(std::exception e)
		{
			ex(e);
			return;
		}

		mOpen = true;
	}
}

void iModule::Prepare(::BaseLib::ExceptionHolder& ex)
{
	if (mPrepare)
	{	// already open
	}
	else
	{
		try
		{
			innerPrepare();
		}
		catch(::BaseLib::Exception e)
		{
			ex(e);
			return;
		}
		catch(std::exception e)
		{
			ex(e);
			return;
		}
		mPrepare = true;
	}
}

void iModule::Close()
{
	if (!mOpen)
	{	// already closed
	}
	else
	{
		innerClose();
		mOpen = false;
		mPrepare = false;
	}
}

bool iModule::ConnectPure(iModule* module, size_t socket)
{
	if (socket < 0 || socket > mInputSockets.size())
	{	// index out of range
		return false;
	}

	// invalide state .. 
	mValid = false;

	// disconnect old one
	if (mInputSockets[socket]->mModule != nullptr)
	{
		mInputSockets[socket]->mModule->OnDisconnect(this);
	}


	// call on Connect
	if (module != nullptr)
	{
		module->OnConnect(this, socket);
	}

	// connect
	mInputSockets[socket]->mModule = module;

	return true;
}

void iModule::ModuleValidation()
{
	if (mValid)
	{	// we are in valid state
		return;
	}

	// set valid on true here, prevent from cycling .. 
	mValid = true;

	// validate all source modules
	size_t socketCount = GetInputSockets().size();

	bool disconnecting = false;

	for (size_t i = 0; i < socketCount; ++i)
	{
		if (GetSocket(i) == nullptr)
		{	// no input module
		}
		else
		{	// validate
			if (Sockets::Socket::Match(GetSocket(i)->GetOutputSocket(), *mInputSockets[i]) )
			{	// match ok ..
			}
			else
			{	// does not match .. we have to say source goodbye
				mInputSockets[i]->mModule->OnDisconnect(this);
				mInputSockets[i]->mModule = nullptr;
				disconnecting = true;
			}
		}
	}

	// revalidate output socket
	ValidateOutputSocket();
	
	if (disconnecting)
	{	// report connection changed while disconnecting
		ReportConnectionChanged();
	}
	
	// report possible interface change .. 
	ReportInterfaceChanged();
}

bool iModule::CreateTopologyOrder(TopologicalOrderStateType& states, std::vector<iModule*>& topologyOrder)
{	
	states[this] = eTopologyStates::OpenState;

	auto iter = std::begin(mInputSockets);
	auto iterEnd = std::end(mInputSockets);
	for (;iter != iterEnd; iter++)
	{
		if ((*iter)->mModule == nullptr)
		{	// skip empty
			continue;
		}

		if (states.count((*iter)->mModule) == 0)
		{	
			if ((*iter)->mModule->CreateTopologyOrder(states, topologyOrder))
			{	// no cycle revealed .. continue
			}
			else
			{	// cycle .. !
				return false;
			}
		}
		else if (states[(*iter)->mModule] == eTopologyStates::CloseState)
		{	// already closed, do nothing
		}
		else
		{	// open -> there is a cycle
			return false;
		}
	}
	// assigning number ~ add to topologyOrder
	topologyOrder.push_back(this);
	states[this] = eTopologyStates::CloseState;

	return true;
}

bool iModule::FindConnectionFromModule(iModule* source)
{	
	auto iter = std::begin(mInputSockets);
	auto iterEnd = std::end(mInputSockets);
	for (;iter != iterEnd; iter++)
	{
		if ((*iter)->mModule == nullptr)
		{	// skip empty
			continue;
		}

		if ((*iter)->mModule == source)
		{	// we find what we were looking for
			return true;
		}

		if ((*iter)->mModule->FindConnectionFromModule(source))
		{	// conection found
			return true;
		}
		else
		{	// continue searching			
		}
	}
	return false;
}

void iModule::PropertyLoadSecondWave()
{	// in base do nothing
}

void iModule::EmitTargetControl()
{
	if (mValid)
	{	// we are in a valid state, we can emit 
	}
	else
	{	// we are in a invalid state we do not emit
		return;
	}

	// we copy targets which can disconnect in the process and thus change mTargets
	std::vector<iModule*> targets(mTargets);
	std::for_each(std::begin(targets), std::end(targets), 
		[] (iModule* item) { item->OnSourceChange(); } );
}

void iModule::ReportMessage(iReportable::eMessageType type, std::string message)
{
	if (mReportable != nullptr)
	{
		mReportable->Message(this, type, message);
	}
}

void iModule::ReportProgress(int min, int max, int value)
{
	if (mReportable != nullptr)
	{
		mReportable->Progress(this, min, max, value);
	}
}

void iModule::ReportConnectionChanged()
{
	if (mReportable != nullptr)
	{
		mReportable->ConnectionChanged(this);
	}
}

void iModule::ReportInterfaceChanged()
{
	if (mReportable != nullptr)
	{
		mReportable->InterfaceChanged(this);
	}
}

}