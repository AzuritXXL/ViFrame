#include "stdafx.h"
// ..
#include "Library.h"
#include "ModuleHolder.h"

namespace Logic {

ModuleHolder::ModuleHolder(Library* library, BaseLib::iModule* module)
	: mCreator(library), mModule(module)
{ }

ModuleHolder::~ModuleHolder()
{
	if (mModule != nullptr)
	{
		Release();
	}
}

const std::string& ModuleHolder::GetId() const
{
	return mModule->GetId();
}

Library* ModuleHolder::GetLibrary()
{
	return mCreator;
}

BaseLib::iModule* ModuleHolder::GetModule()
{
	return mModule;
}

void ModuleHolder::Release()
{
	mCreator->Release(this);
}


}