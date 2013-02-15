#include "stdafx.h"
// ...
#include "Object.hpp"

namespace BaseLib {
namespace Objects {

Object::Object(const std::string& name)
	: mPassCounter(0), mName(name), mData(), mMainData(0)
{
}

Object::~Object()
{	// delete old data
	std::for_each(std::begin(mData), std::end(mData),
		[] (const std::pair<std::string, Variant*>& item)
		{
			delete item.second;
		}
	);
	mData.clear();
	// main data are part of mData, so its content has already been deleted
	mMainData = nullptr;
}

Object::Object(const Object& data)
{
}

const Object& Object::operator= (const Object& data)
{
	return *this;
}

const std::string& Object::GetName()
{
	return mName;
}

void Object::SetMain(const std::string& name)
{	
	std::stringstream ss;
	ss << mPassCounter << "." << name;
	std::string fullName = ss.str();
	// use fullName
	mMainData = mData[fullName];
}

void Object::SetMainPure(const std::string& name)
{
	mMainData = mData[name];
}

Variant* Object::GetMain()
{
	return mMainData;
}

const Object::DataType& Object::GetData() const
{
	return mData;
}

void Object::Add(const std::string& name, Variant* data)
{	// create data name
	std::stringstream ss;
	ss << mPassCounter << "." << name;
	std::string fullName = ss.str();
	// add data
	mData[fullName] = data;
}

void Object::AddPure(const std::string& name, Variant* data)
{	// add data
	mData[name] = data;
}

void Object::Add(const std::string& name, Variant* data, std::string& fullName)
{
	std::stringstream ss;
	ss << mPassCounter << "." << name;
	fullName = ss.str();
	// add data
	mData[fullName] = data;
}

void Object::Remove(const std::string& name)
{
	if(mData.count(name) == 0)
	{
		// there is no data with that name
	}
	else
	{
		// delete data
		delete mData[name];
		// erase data from mData
		mData.erase(name);
	}
}

void Object::GiveUp()
{	// release all pointers, do not delete data 
	mData.clear();
	// main data are part of mData,  so its content has already been deleted
	mMainData = nullptr;
}

void Object::Pass()
{
	mPassCounter++;
}

void Object::SetPass(int value)
{
	mPassCounter = value;
}

std::string Object::Find(const std::string& name, int type) const
{
	auto iter = std::begin(mData);
	auto iterEnd = std::end(mData);
	for (;iter != iterEnd; ++iter)
	{	// find identical key
		if (iter->second->GetType() == type)
		{	// check name
			if (iter->first.substr(iter->first.find_first_of('.') + 1) == name)
			{	// name match -> return full name
				return iter->first;
			}
		}
	}
	return "";
}

} }