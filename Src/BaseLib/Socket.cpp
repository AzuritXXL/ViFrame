#include "stdafx.h"
// ..
#include "Socket.hpp"

namespace BaseLib {
namespace Sockets {

Socket::Socket()
	: mPassCounter(0), mName(""), mDescription(""), mModule(nullptr), mPrimary("", -1, 0), mInterface(0)
{
}

Socket::Socket(const std::string& name, const std::string& description, int socInterface)
	: mPassCounter(0), mName(name), mDescription(description), mModule(nullptr), mPrimary("", -1, 0), mInterface(socInterface)
{ }

int Socket::GetInterface() const
{
	return mInterface;
}

void Socket::SetInterface(int inter)
{
	mInterface = inter;
}

const std::string& Socket::GetName() const
{
	return mName;
}

const std::string& Socket::GetDescription() const
{
	return mDescription;
}

void Socket::SetPrimary(const std::string& name, int requirement)
{
	mPrimary.Name = name;	
	mPrimary.Att.Level = mPassCounter;
	mPrimary.Att.Type= requirement;
}

void Socket::SetPrimary(const std::string& name, int level, int requirement)
{
	mPrimary.Name = name;	
	mPrimary.Att.Level = level;
	mPrimary.Att.Type= requirement;
}

void Socket::AddData(const std::string& name, int requirement)
{	// add with current key 
	mData[name].push_back( DataType::value_type::second_type::value_type(mPassCounter, requirement) );
}

void Socket::AddData(const std::string& name, int level, int requirement)
{
	mData[name].push_back( DataType::value_type::second_type::value_type(level, requirement) );
}

void Socket::IncreasePassCounter()
{
	mPassCounter++;
}

int Socket::GetPassCounter() const
{
	return mPassCounter;
}

void Socket::SetPassCounter(int value)
{
	mPassCounter = value;
}

void Socket::Reset()
{
	mData.clear();
	// delete primary
	mPrimary.Name = "";
	mPrimary.Att.Type = 0;
}

void Socket::Copy(const Socket& source)
{	// copy primary and secondary data
	mPassCounter = source.mPassCounter;
	mPrimary = source.mPrimary;
	mData = source.mData;
	mInterface = source.mInterface;
}

void Socket::Add(const Socket& socket)
{	// copy secondary data	
	std::for_each(std::begin(socket.mData), std::end(socket.mData),
		[&] (DataType::const_reference named)
		{	// add prefix before name
			std::for_each(std::begin(named.second), std::end(named.second),
				[&] (Socket::DataType::value_type::second_type::const_reference item)
				{
					mData[named.first].push_back(Socket::Attribute(mPassCounter, item.Type));
				}
			);
		}
	);
	
	// primary data
	if (socket.mPrimary.Att.Type == 0)
	{	// source data not specified, original remain
	}
	else
	{	// move primary to secondary
		mData[mPrimary.Name].push_back(mPrimary.Att);
		// and set new primary, and set passCounter for him
		mPrimary = socket.mPrimary;
		mPrimary.Att.Level = mPassCounter;
	}
}

const Socket::PrimaryType& Socket::GetPrimary() const
{
	return mPrimary;
}

int Socket::GetPrimaryType() const
{
	return mPrimary.Att.Type;
}

const std::string& Socket::GetPrimaryName() const
{
	return mPrimary.Name;
}

const Socket::DataType& Socket::GetData() const
{
	return mData;
}

Socket::DataType& Socket::GetDataFull()
{
	return mData;
}

bool Socket::Match(const Socket& source, const Socket& target) throw()
{
	// source must have set up primary data, if it doesn't ... we can't match
	if (source.mPrimary.Att.Type == 0)
	{
		return false;
	};

	// if the target has some requirments, we check them, we don't care about mPrimaryKey (primary is accessed throug function not key)
	if (target.mPrimary.Att.Type != 0 && source.mPrimary.Att.Type != target.mPrimary.Att.Type)
	{
		return false;
	}

	if ( (source.mInterface & target.mInterface) == 0)
	{	// interface has to match
		return false;
	}
	
	bool allMatch = true;
	// check secondary conditions
	std::for_each(std::begin(target.mData), std::end(target.mData),
		[&] (DataType::const_reference vector)
		{	// add prefix in front of name
			std::for_each(std::begin(vector.second), std::end(vector.second),
				[&] (Socket::DataType::value_type::second_type::const_reference item)
				{				
					bool match = false;
					auto iter = std::begin(source.mData.at(vector.first));
					auto iterEnd = std::end(source.mData.at(vector.first));
					for (;iter != iterEnd; ++iter)
					{	// check key
						if (iter->Type == item.Type)
						{	// match found
							match = true;
							break;
						}
					}
					// ...
					if (!match)
					{	// we need all match
						allMatch &= match;
					}
				}
			);
		}
	);	

	return allMatch;
}

} }
