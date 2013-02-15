#include "stdafx.h"
// ...
#include <algorithm>
///
#include "main.h"
// BaseLib
#include "Description.hpp"
#include "BaseProperty.hpp"
#include "LimitedProperty.hpp"
#include "PropertyCreator.hpp"

Separator::Separator()
	: BaseClass("Groups"), mGroups(2), mUseValue(false)
{
	mProperties.push_back(BaseLib::Properties::CreateLimited("Groups: ", "Number of created groups.", mGroups, 1, 1024));
	mProperties.push_back(BaseLib::Properties::PropertyCreator::Create("Use values:", 
		"Checked: split according to values Unchecked: split into groups with same size", mUseValue));
}

//								BaseLib::Modules::RandAdder										//

void Separator::Transform()
{
	// count expected value
	int dataSize = DataSize();

	if (dataSize == 0)
	{
		return;
	}

	if(mUseValue)
	{	// use value
		// calculate min and max
		double max, min;
		max = min = AccesInData(0).Data;
		for (int i = 1; i < dataSize; ++i)
		{
			double value = AccesInData(i).Data;
			max = value > max ? value : max;
			min = value < min ? value : min;
		}

		// single step
		double step = (max - min) / (double)mGroups;

		// separate .. 
		for (int i = 0; i < dataSize; ++i)
		{
			double value = AccesInData(i).Data - min;
			int group = 0;
			// go and increase group index .. 
			for (;value > step && group < mGroups; value -= step, group++);
			// add information 
			AccesOutData(i) = group + 1;
		}
	}
	else
	{
		// we have got list with values
		std::vector<double> tempList;
		for (int i = 0; i < dataSize; ++i)
		{
			tempList.push_back(AccesInData(i).Data);
		}
		std::sort(std::begin(tempList), std::end(tempList));
		// now we select delimeters
		std::vector<double> delimeters;
		int step = dataSize / mGroups; // we know how many to use 

		for (int i = 0; i < dataSize; ++i)
		{
			if (i % step == 0)
			{
				delimeters.push_back(tempList[i]);
			}
		}
		// in delimeters we have values now split 

		for (int i = 0; i < dataSize; ++i)
		{
			int classValue = 0;
			double data = AccesInData(i).Data;
			// try to find how big we are
			while ( classValue < delimeters.size())
			{
				if ( data < delimeters[classValue])
				{	// we are lowe then class end
					break;
				}
				// to the higher class
				++classValue;
			}
			// in classValue we have our class
			AccesOutData(i) = classValue;
		}
	}

	// done .. 
}

//										Module access											//

_DYNLINK void* CreateModule() 
{ 
	return new Separator(); 
}

_DYNLINK void DeleteModule(void* module) 
{
	Separator* convModule = reinterpret_cast<Separator*>(module);
	delete convModule;
}

BaseLib::Description locDescription(std::pair<int, int>(1,0), 
	"Separator",
	"Create groups according value.",
	"Module take main data attribute and use it to split data items in to groups according his value. </br>"
	"Groups are created as a attribute called 'Groups', where values determine group. Number of  "
	"created group can be set in module property."
	);

_DYNLINK const BaseLib::Description* GetDescription() 
{ 
	return &locDescription; 
}