#include "stdafx.h"
// ...
#include "OutputExporter.h"

namespace Output {

Eporter::Eporter(const std::vector<std::string>& attributes, const std::string& objectName, const std::string& objectParent, std::string separator)
	: mAttributes(attributes), mObjectName(objectName), mObjectParent(objectParent), mSeparator(separator)
{ }

void Eporter::Open(std::string file, bool header)
{
	mStream.open(file, std::ios::out | std::ios::trunc);
	mExportet.clear();

	if (header)
	{	// write header
		auto attIter = std::begin(mAttributes);
		auto attIterEnd = std::end(mAttributes);
		mStream << *attIter;
		++attIter;
		for (;attIter != attIterEnd; ++attIter)
		{
			mStream << mSeparator << *attIter;
		}
	}
	mStream << std::endl;
}

void Eporter::Close()
{
	mStream.close();
}

void Eporter::Export(DataModels::Output::DataItem* item)
{
	if (mExportet.count(item) == 0)
	{	// add and export
		mExportet.insert(item);
	}
	else
	{	// already exportet
		return;
	}

	// export all atributes
	auto dataObject = item->GetObject().GetData();
	auto attIter = std::begin(mAttributes);
	auto attIterEnd = std::end(mAttributes);

	// first object export
	if (dataObject.count(*attIter) == 0)
	{	// attribute name has not been found .. 
		if (*attIter == mObjectName)
		{
			mStream << '"' << item->GetObject().GetName() << '"';
		}
		else if (*attIter == mObjectParent)
		{
			mStream << '"' << item->GetParent()->GetName().toStdString() << '"';
		}
	}
	else
	{	// export
		mStream << dataObject[*attIter]->ToString();
	}
	++attIter;

	for (;attIter != attIterEnd; ++attIter)
	{
		mStream << mSeparator;

		if (dataObject.count(*attIter) == 0)
		{	// attribute name has not been found .. 
			if (*attIter == mObjectName)
			{
				mStream << '"' << item->GetObject().GetName() << '"';
			}
			else if (*attIter == mObjectParent)
			{
				mStream << '"' << item->GetParent()->GetName().toStdString() << '"';
			}
		}
		else
		{	// export
			mStream << dataObject[*attIter]->ToString();
		}		
	}
	// end of object
	mStream << std::endl;
}

void Eporter::Export(DataModels::Output::VirtualItem* item)
{
	if (mExportet.count(item) == 0)
	{	// add and export
		mExportet.insert(item);
	}
	else
	{	// already exportet
		return;
	}

	for (int i = 0; i < item->GetChildCount(); ++i)
	{
		if (item->GetChild(i)->GetType() == DataModels::Output::DataItem::Identification)
		{	// data oject
			Export( dynamic_cast<DataModels::Output::DataItem*>(item->GetChild(i)) );
		}
		else
		{	// DataModels::Output::VirtualItem::Identification
			Export( dynamic_cast<DataModels::Output::VirtualItem*>(item->GetChild(i)) );
		}
	}
}

}