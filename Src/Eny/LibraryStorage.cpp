#include "stdafx.h"
// ...
#include "LibraryStorage.h"

namespace Logic {

LibraryStorage::LibraryStorage()
	: mLibraries()
{ }

LibraryStorage::~LibraryStorage()
{
	// call delete on each libary
	std::for_each(std::begin(mLibraries), std::end(mLibraries), 
		[] (std::pair<std::string, Library*> item) 
		{
			try
			{
				delete item.second;
				item.second = nullptr;
			}
			catch (...)
			{	// if something failed .. we can't do anything about it, just continue
			}
		}
	);
	
	// clear content
	mLibraries.clear();
}

Library& LibraryStorage::Create(const std::string& filePath)
{
	// we use file path as id .. 
	mLibraries[filePath] = new Library(filePath, filePath);
	// and we return refrence to created library
	return *mLibraries[filePath];
}

Library* LibraryStorage::GetLibrary(const std::string& id)
{
	if (mLibraries.count(id) == 0)
	{
		return nullptr;
	}
	else
	{
		// return refrence to class
		return mLibraries[id];
	}
}

}