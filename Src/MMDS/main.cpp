#include "stdafx.h"
// BaseLib
#include "Description.hpp"
#include "LimitedProperty.hpp"
//...
#include "mdsMethod.h"
#include "main.h"

MDS::MDS()
	: Visualiser("MDS_Out"), mTargetDimension(2), mIterCount(40)
{
	mProperties.push_back(
		BaseLib::Properties::CreateLimited("Target dimension: ", "Output main data dimension.", mTargetDimension, 1, 1048576)
		);

	mProperties.push_back(
		BaseLib::Properties::CreateLimited("Number of iterations: ", "Using high number of iterations can significantly slow down computation.", mIterCount, 1, 1048576)
		);
}

void MDS::Visualise()
{
	size_t dataSize = DataSize();
	if (dataSize == 0)
	{	// no data
		return;
	}
	// get source dimension .. we will hope that lengh of all vector is equal .. 
	size_t sourceDimension = AccesInData(0).Data.size();

	// construct and set mds method
	MultidimensionalScaling mds;
	// init mds
	mds.Init(dataSize, sourceDimension, mTargetDimension);
	
	Visualiser::StorageType& innerStorage = AccesStorage();
	// set distances
	double distance;
	auto iterIn = innerStorage.begin();
	auto iterOut = iterIn;
	auto iterEnd = innerStorage.end();
	// we go through all pairs
	for (size_t i = 0;iterOut != iterEnd; ++iterOut, ++i)
	{
		iterIn = iterOut;
		iterIn++;
		for (size_t j = i + 1;iterIn != iterEnd; ++iterIn, ++j)
		{
			// compute Euclidean distance 
			std::vector<double>& first = (dynamic_cast<BaseLib::Objects::VDouble*>(iterOut->Object->GetMain()))->Data;
			std::vector<double>& other = (dynamic_cast<BaseLib::Objects::VDouble*>(iterIn->Object->GetMain()))->Data;
			int vectorSize = first.size();
			distance = 0;
			for (size_t k = 0; k < vectorSize; ++k)
			{
				distance += (first[k] - other[k]) * (first[k] - other[k]);
			}
			distance = std::sqrt(distance);
			// add distance
			mds.AddDistance(i, j, distance);
		}
	}

	// set initial position
	iterIn = innerStorage.begin();
	for (size_t i = 0;iterIn != iterEnd; ++iterIn, ++i)
	{	// we use first two coordinates
		mds.AddFirstPosition(i, (dynamic_cast<BaseLib::Objects::VDouble*>(iterIn->Object->GetMain()))->Data);
	}

	// iterate ...
	for (size_t i = 0; i < mIterCount; ++i)
	{
		mds.DoIteration();
		// report progress
		ReportProgress(0, mIterCount, i);
	}

	// get output data 
	for (size_t i = 0; i < dataSize; ++i)
	{	// store content 
		auto data = mds.CreateOutVector(i);
		std::swap(AccesOutData(i).Data, data);
	}
}

//										Module access											//

_DYNLINK void* CreateModule() 
{ 
	return new MDS(); 
}

_DYNLINK void DeleteModule(void* module) 
{
	MDS* convModule = reinterpret_cast<MDS*>(module);
	delete convModule;
}

BaseLib::Description locDescription(std::pair<int, int>(1,0), 
	"MDS",
	"Provide implementation of squared MDS.",
	"Provide implementation of squared MDS. "
	"<span style=\"color:red\">Module can consume large amout of memory to store matrix of distances. </span>"
	"Setting of number of MDS iteration is posible in module propoerty. Is it not possible to determine"
	"number of iteration from stress level.</br></br>"
	"Module uses Eigen (http://eigen.tuxfamily.org/index.php?title=Main_Page),"
	"which is LGPL-licensed ( http://www.gnu.org/copyleft/lesser.html )."
	);

_DYNLINK const BaseLib::Description* GetDescription() 
{ 
	return &locDescription; 
}