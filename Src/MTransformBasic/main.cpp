#include "stdafx.h"
// ..
#include "main.h"
// BaseLib
#include "Description.hpp"
#include "BaseProperty.hpp"
#include "PropertyCreator.hpp"

BasicTransform::BasicTransform()
	: BaseClass("Transformed"), mCenter(true), mScale(true)
{
	mProperties.push_back(BaseLib::Properties::PropertyCreator::Create("Center: ", "True if data should be centred ( substract mean ).", mCenter));
	mProperties.push_back(BaseLib::Properties::PropertyCreator::Create("Scale: ", "True if divide data with standart deviation.", mScale));
}

//								BaseLib::Modules::RandAdder										//

void BasicTransform::Transform()
{
	// count expected value
	int dataSize = DataSize();

	if (dataSize == 0)
	{
		return;
	}

	// calculate mean 
	std::vector<double> mean;
	mean.resize(AccesInData(0).Data.size(), 0);
	int meanSize = mean.size();
	for (int i = 0; i < dataSize; ++i)
	{
		// add to mean
		const std::vector<double>& data = AccesInData(i).Data;		
		for (int i = 0; i < meanSize; ++i)
		{
			mean[i] += data[i];
		}
	}
	// 1/n
	std::for_each(std::begin(mean), std::end(mean),
		[&] (double& value) { value /= dataSize; } );

	// calculate standartDeviation
	std::vector<double> standardDeviation;
	standardDeviation.resize(AccesInData(0).Data.size(), 0);
	int deviationSize = standardDeviation.size();
	for (int i = 0; i < dataSize; ++i)
	{
		const std::vector<double>& data = AccesInData(i).Data;		
		for (int i = 0; i < deviationSize; ++i)
		{
			standardDeviation[i] += (data[i] - mean[i]) * (data[i] - mean[i]);
		}
	}
	
	// sqrt 
	std::for_each(std::begin(standardDeviation), std::end(standardDeviation),
		[&] (double& value) 
		{ 
			value = std::sqrt(value); 
			// if standart deviation is zero .. then we will be dividing by zero on next line .. 
			// so use value 1
			if (value == 0)
			{
				value = 1;
			}
		} 
	);

	// modify output ... this can be litle bit tricky .. 
	for (int i = 0; i < dataSize; ++i)
	{	
		const std::vector<double>& inData = AccesInData(i).Data;
		std::vector<double>& outData = AccesOutData(i).Data;

		size_t vectorSize = inData.size();
		// check output data size
		if (outData.size() == vectorSize)
		{	// size match ok
		}
		else
		{	// we have to resize
			outData.resize(vectorSize, 0);
		}

		// inData and outData can be the same ..
		for (size_t j = 0; j < vectorSize; ++j)
		{
			// use input data as output ( if no transformation, just copy data )
			outData[j]  = inData[j];

			if (mCenter)	// center
			{
				outData[j] = outData[j] - mean[j];
			}

			if (mScale)		// scale ( we use output data )
			{
				outData[j] = outData[j] / standardDeviation[j];
			}
		}
	}
}

//										Module access											//

_DYNLINK void* CreateModule() 
{ 
	return new BasicTransform(); 
}

_DYNLINK void DeleteModule(void* module) 
{
	BasicTransform* convModule = reinterpret_cast<BasicTransform*>(module);
	delete convModule;
}

BaseLib::Description locDescription(std::pair<int, int>(1,0), 
	"Scaler",
	"Offer data scaling by centring or standardization.",
	"Offer data scaling by centring or standardization."
	"Can do only centring or standardization, but order of them cant be switched."
	"Also can be set if change main data, add as new main data, or as a secondary data."
	);

_DYNLINK const BaseLib::Description* GetDescription() 
{ 
	return &locDescription; 
}