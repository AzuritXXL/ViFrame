#include "stdafx.h"
// ...
#include "Library.h"
#include "ModuleHolder.h"

namespace Logic {

Library::Library(const std::string& filepath, const std::string& id)
	:
#ifdef WIN32 
	mLibrary(0), 
#endif
	fceCreate(nullptr), fceDestroy(nullptr), fceDescription(nullptr), mIsLoaded(false), mFilePath(filepath), mId(id),
	mDependencyCount(0)
{ }

Library::~Library()
{
	//. Rise warning when mDependencyCount > 0 
	if (mIsLoaded)
	{
		try
		{
			UnLoad();
		}
		catch(...)
		{
			// we supress any exception, so dtor is exception safe
		}
	}
}

bool Library::GetIsLoaded() const
{
	return mIsLoaded;
}

const std::string& Library::GetId() const
{
	return mId;
}

void Library::Load()
{
	if (mIsLoaded)
	{	// allready loaded
		return;
	}

#ifdef WIN32	
	// load library
	mLibrary = LoadLibraryA(mFilePath.c_str());
	if(mLibrary == nullptr)
	{
		std::string message = "Failed to load dll file: " + mFilePath;
		throw std::exception(message.c_str());
	}

	// load functions
	fceCreate = (ModuleCtor)GetProcAddress( mLibrary, "CreateModule");
	fceDestroy = (ModuleDtor)GetProcAddress( mLibrary, "DeleteModule");
	fceDescription = (ModuleDescription)GetProcAddress( mLibrary, "GetDescription");
		
	if (fceCreate == nullptr || fceDestroy == nullptr || fceDescription == nullptr)
	{	// we failed, release library
		FreeLibrary(mLibrary);
		mLibrary = nullptr;
		throw std::exception("Failed to load function from dll file ( incompatible dll ).");
	}

	mIsLoaded = true;
#endif
}

void Library::UnLoad()
{
	if (!mIsLoaded)
	{
		return;
	}

#ifdef WIN32	
	// unload dll
	if(mLibrary != nullptr)
	{
		FreeLibrary(mLibrary);
	}
#endif

	mLibrary = nullptr;
	fceCreate = nullptr;
	fceDestroy = nullptr;
	fceDescription = nullptr;
	mIsLoaded = false;
}

ModuleHolder* Library::Create(const std::string& moduleId)
{
	// secure that module is loaded
	Load();
	// create module
	BaseLib::iModule* module = reinterpret_cast<BaseLib::iModule*>(fceCreate());
	module->SetId(moduleId);
	// create new module holder
	ModuleHolder* result = new ModuleHolder(this, module);
	// increase number of dependency
	mDependencyCount++;
	return result;
}

void Library::Release(ModuleHolder* holder)
{	// release data from holder if there are some
	if (holder->mModule != nullptr)
	{
		// secure that the module is loaded
		Load();
		// delete data
		fceDestroy(holder->mModule);
		holder->mModule = nullptr;
		// decrase number of dependecies
		mDependencyCount--;
	}
}

const BaseLib::Description& Library::GetDescription()
{
	// secure that module is loaded
	if (!mIsLoaded)
	{
		Load();
	}

	return *fceDescription();
}

int Library::GetDependencyCount()
{
	return mDependencyCount;
}

}