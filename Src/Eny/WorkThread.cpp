#include "stdafx.h"
// ...
#include "WorkThread.h"
// BaseLib
#include "Object.hpp"
#include "Module.hpp"
#include "Reportable.hpp"
#include "RandomAcces.hpp"
#include "SequentialAcces.hpp"
#include "OutputDataItem.h"
#include "OutputVirtualItem.h"
#include "Exception.hpp"
		  
namespace Logic {

WorkThread::WorkThread()
	: mModule(nullptr), mRoot(nullptr), mKeys(), mIntegralKeys(), mDataItems()
{ }

WorkThread::~WorkThread()
{
	Clear();
}

WorkThread::DataItemTypes& WorkThread::GetDataItems()
{
	return mDataItems;
}

QList<QString>& WorkThread::GetKeys()
{
	return mKeys;
}

QList<QString>& WorkThread::GetIntegralKeys()
{
	return mIntegralKeys;
}

DataModels::Output::VirtualItem* WorkThread::GetRoot()
{
	auto tempPtr = mRoot;
	mRoot = nullptr;
	return tempPtr;
}

WorkThread::ObjectsType& WorkThread::GetObjects()
{
	return mObjects;
}

void WorkThread::SetModule(BaseLib::iModule* module)
{
	mModule = module;
}

BaseLib::iModule* WorkThread::GetModule()
{
	return mModule;
}

void WorkThread::Clear()
{	// we first delete root, becouse ModelView use data from mObjects
	// delete root
	if (mRoot != nullptr)
	{
		delete mRoot;
		mRoot = nullptr;
	}
	// delete data
	std::for_each(std::begin(mObjects), std::end(mObjects), 
		[] (BaseLib::Objects::Object*& item)
		{
			delete item;
			item = nullptr;
		}
	);
	mObjects.clear();
	mKeys.clear();
	mIntegralKeys.clear();
	mDataItems.clear();
}

void WorkThread::ReadRandom(BaseLib::Interfaces::RandomAcces* random)
{
	BaseLib::ExceptionHolder ex;
	size_t size = random->GetCount();
	// obtain data check for exceptions .. 
	for (size_t i = 0; i < size; ++i)
	{
		mObjects.push_back(random->GetObject(i, ex));
		ex.Rethrow();
	}
}

void WorkThread::ReadSequential(BaseLib::Interfaces::SequentialAcces* sequential)
{
	// obtain data check for exceptions .. 
	BaseLib::ExceptionHolder ex;
	while (sequential->Next(ex))
	{
		ex.Rethrow();		
		mObjects.push_back(sequential->GetObject(ex));
		ex.Rethrow();
	}
	// if Next failed on first time we also check for exception .. 
	ex.Rethrow();
}

void WorkThread::CreateModel()
{
	DataModels::Output::DataItem* itemModel = nullptr;
	mRoot = new DataModels::Output::VirtualItem("", 0);
	std::for_each(std::begin(mObjects), std::end(mObjects),
		[&] (ObjectsType::const_reference itemObject)
		{
			itemModel = new DataModels::Output::DataItem(*itemObject, mRoot);
			// add to items
			mDataItems.push_back(itemModel);
			// add to model
			mRoot->AppendChild(itemModel);
		}
	);
}

//											QThread												//

void WorkThread::run()
{
	// start with cleanup
	Clear();

	try
	{
		// start with checking integrity
		if (mModule->CheckIntegrity())
		{
			siAddMessage(DataModels::Message::Item(BaseLib::iReportable::Information, "ThreadWork", "CheckIntegrity  was sucessful. Work has been started ..."));
		}
		else
		{	// send message
			siAddMessage(DataModels::Message::Item(BaseLib::iReportable::Error, "ThreadWork", "CheckIntegrity failed!"));
			// end work .. 
			return;
		}

		BaseLib::ExceptionHolder ex;

		siAddMessage(DataModels::Message::Item(BaseLib::iReportable::Information, "ThreadWork", "Openning ..."));

		// open module
		mModule->Open(ex);
		ex.Rethrow();

		siAddMessage(DataModels::Message::Item(BaseLib::iReportable::Information, "ThreadWork", "Preparing ..."));

		// prepare module data
		mModule->Prepare(ex);
		ex.Rethrow();
		
		siAddMessage(DataModels::Message::Item(BaseLib::iReportable::Information, "ThreadWork", "Computing ..."));

		int interfaces = mModule->GetOutputSocket().GetInterface();
		void* iterface = nullptr;

		// we will continue due the support interface
		if ( (interfaces & BaseLib::Interfaces::RandomAcces::Identification) == BaseLib::Interfaces::RandomAcces::Identification)
		{	// try to get interface
			iterface = mModule->AccessInterface(BaseLib::Interfaces::RandomAcces::Identification);
			if (iterface == nullptr)
			{
				siAddMessage(DataModels::Message::Item(BaseLib::iReportable::Error, "ThreadWork", "Failed to get output interface."));
				mModule->Close();
				return;
			}
			// read
			ReadRandom(reinterpret_cast<BaseLib::Interfaces::RandomAcces*>(iterface));
		}
		else if ( (interfaces & BaseLib::Interfaces::SequentialAcces::Identification) == BaseLib::Interfaces::SequentialAcces::Identification)
		{	// try to get interface
			iterface = mModule->AccessInterface(BaseLib::Interfaces::SequentialAcces::Identification);
			if (iterface == nullptr)
			{
				siAddMessage(DataModels::Message::Item(BaseLib::iReportable::Error, "ThreadWork", "Failed to get output interface."));
				mModule->Close();
				return;
			}
			// read
			ReadSequential(reinterpret_cast<BaseLib::Interfaces::SequentialAcces*>(iterface));
		}
		else
		{	// no supported interface
			siAddMessage(DataModels::Message::Item(BaseLib::iReportable::Error, "ThreadWork", "Module hasn't offer any supported output interface."));
			mModule->Close();
			return;
		}

		if (mObjects.size() == 0)
		{	// no data .. there is no need to continue, so clear data and end .. 			
			siAddMessage(DataModels::Message::Item(BaseLib::iReportable::Information, "ThreadWork", "No output objects."));
			mModule->Close();
			Clear();
			return;
		}

		// create model
		siAddMessage(DataModels::Message::Item(BaseLib::iReportable::Information, "ThreadWork", "Creating data model ..."));
		CreateModel();
		
		// gather key names
		siAddMessage(DataModels::Message::Item(BaseLib::iReportable::Information, "ThreadWork", "Collect key names ..."));
		std::for_each(std::begin(mObjects), std::end(mObjects),
			[&] (BaseLib::Objects::Object* item)
			{
				std::for_each(std::begin(item->GetData()), std::end(item->GetData()),
					[&] (BaseLib::Objects::Object::DataType::const_reference item)
					{
						if (!mKeys.contains(item.first.c_str()))
						{	// new key
							mKeys.push_back(item.first.c_str());
							// test to integral type
							switch(item.second->GetType())
							{
							case BaseLib::Objects::Variant::Int:
							case BaseLib::Objects::Variant::Double:
							case BaseLib::Objects::Variant::VectorInt:
							case BaseLib::Objects::Variant::VectorDouble:
								mIntegralKeys.push_back(item.first.c_str());
							default:
								break;
							}

							
						}
					}
				);
			}
		);

		siAddMessage(DataModels::Message::Item(BaseLib::iReportable::Information, "ThreadWork", "Complete, total output object: " + QString::number(mObjects.size())));
	}
	catch (BaseLib::Exception ex)
	{
		siAddMessage(DataModels::Message::Item(BaseLib::iReportable::Error, "ThreadWork", "Eny::exception : " + QString(ex.what())) );
		// remove collected data
		Clear();
	}
	catch (std::exception ex)
	{
		siAddMessage(DataModels::Message::Item(BaseLib::iReportable::Error, "ThreadWork", "Std::exception : " + QString(ex.what())) );
		// remove collected data
		Clear();
	}

	// close main module
	mModule->Close();
}

}