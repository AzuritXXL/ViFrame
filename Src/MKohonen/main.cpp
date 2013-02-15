#include "stdafx.h"
// Std
#include <cstdlib> 
#include <ctime>
#include <hash_set>
// BaseLib
#include "Description.hpp"
#include "LimitedProperty.hpp"
//...
#include "kohonenMethod.h"
#include "main.h"


/**
 * Calculate params for kohonen maps.
 */
class cParams
{
private:
	double mOriginChangeRate;

	double mChangeRate;

	int mStep;
public:
	cParams() : mOriginChangeRate(1), mChangeRate(1), mStep(0) { }; 
public:
	// report initial kohonen map size
	void SetSize(size_t x, size_t y)
	{
		mChangeRate = mOriginChangeRate = std::max(x / 2, y / 2);
	}

	// report single learning step
 	void LearnStep()
	{
		// increase step counter
		mStep++;
		// decrase learning step
		double decraseRate = std::exp( -((double)mStep) );
		mChangeRate = mOriginChangeRate * decraseRate;
	}

	// return distance
 	double LearnPower(double x0, double y0, double x1, double y1)
	{
		double change = std::exp(- std::sqrt( std::pow(x0 - x1, 2) + std::pow(y0 - y1, 2)) );
		return change * mChangeRate;
	}
};


/**
 * Class provide functor, to init Kohonen network.
 * After set Dimension, MaxValue and MinValue return operator() 
 * randomly initialed vector from range Max/Min Value.
 */
class cInit
{
public:
	/**
	 * Dimension of data.
	 */
	size_t Dimnesion;
	/**
	 * Max value for each dimension.
	 */
	std::vector<double> MaxValue;
	/**
	 * Min value for each dimension.
	 */
	std::vector<double> MinValue;
public:
	void operator() (Kohohen<double, cParams>::DataType& data)
	{
		// set size of data
		data.resize(Dimnesion);
		// set random value between MaxValue and MinValue
		size_t position = 0;
		for (size_t i = 0; i < Dimnesion; ++i)
		{
			// start with minumum value
			data[i] = MinValue[i];
			// randomly add up to MaxValue + 10 percent out of values .. 
			data[i] += ((double)std::rand() / RAND_MAX) * (MaxValue[i] - MinValue[i]) * 1.1;
		}
	}
};

/**
 * Calculate distance between two nodes. Used square euclidean distance.
 * @param[in] node
 * @param[in] node
 * @return distance
 */
double MatchFunction(const std::vector<double>& left, const std::vector<double>& right)
{	// Euclidean distances

	double distance = 0;

	std::vector<double>::const_iterator iterLeft = left.cbegin();
	std::vector<double>::const_iterator iterRight = right.cbegin();

	for( ; iterLeft != left.end() && iterRight != right.end(); ++iterLeft, ++iterRight)
	{
		double val = (*iterLeft) - (*iterRight);
		distance += val * val;
	}
	distance = std::sqrt(distance);

	return distance;
}

// ....

ModuleKohonen::ModuleKohonen()
	: Visualiser("Kohonen_Out"), mLearningSetSize(0.3), mNetworkSize(0.2)
{
	mProperties.push_back(
		BaseLib::Properties::CreateLimited("Learning set size: ", 
		"Size of learning set for kohonene network as a percent from input data size.", mLearningSetSize, 0, 1));

	mProperties.push_back(
		BaseLib::Properties::CreateLimited("Network size: ", 
		"Size of kohonene network as a percent from input data size.", mNetworkSize, 0, 10));
}

void ModuleKohonen::Visualise()
{
	// check size
	size_t dataSize = DataSize();
	if (dataSize == 0)
	{	// no data
		return;
	}

	size_t learnCount = size_t(dataSize * mLearningSetSize);
	// we calculate a side lenght of data network
	size_t kohonenSize = size_t(std::sqrt(dataSize * mNetworkSize));
	
	if (learnCount == 0)
	{
		ReportMessage(BaseLib::iReportable::Error, "Too small sample data subset, increace 'Learning set size' property or input data count.");
		throw BaseLib::Exception();
	}
	if (kohonenSize == 0)
	{	// network will be too small to do something
		ReportMessage(BaseLib::iReportable::Error, "Too small network, increace 'Network size' propeorty or input data count.");
		throw BaseLib::Exception();
	}
	
	std::stringstream ss;
	ss << "Network size: " << kohonenSize << " Learning set size: " << learnCount;
	ReportMessage(BaseLib::iReportable::Information, ss.str());

	// determine max progress
	size_t progressSize = learnCount + dataSize;
	ReportProgress(0, progressSize, 1);

	// we prepare initCall to initial Kohonen network
	cInit initClass;
	initClass.Dimnesion = AccesInData(0).Data.size();
	initClass.MaxValue = initClass.MinValue = AccesInData(0).Data;
	// determine min and max value
	Visualiser::StorageType& innerStorage = AccesStorage();
	std::for_each(std::begin(innerStorage), std::end(innerStorage),
		[&] (Visualiser::StorageType::reference item)
		{
			std::vector<double>& dataVector = dynamic_cast<BaseLib::Objects::VDouble*>(item.Object->GetMain())->Data;
			for (size_t i = 0 ; i < dataVector.size(); ++i)
			{
				if (initClass.MinValue[i] > dataVector[i])
				{
					initClass.MinValue[i] = dataVector[i];
				}
				if (initClass.MaxValue[i] < dataVector[i])
				{
					initClass.MaxValue[i] = dataVector[i];
				}				
			}
		}
	);

	// kohonen
	Kohohen<double, cParams> kohonen;
	kohonen.CreateNetwork(kohonenSize, kohonenSize, initClass);
	kohonen.SetMatchFunction(MatchFunction);

	ReportMessage(BaseLib::iReportable::Information, "Learning phase.");

	// learn on samples, we will prevent repeating the same selection
	srand((unsigned)time(0)); 	
	size_t randIndex;
	std::hash_set<size_t> selectedIndexes;
	for (size_t i = 0; i < learnCount; ++i)
	{	// take randomly selected object, prevent slection of once selected
		do
		{
			// select random in from (0, dataCount)
			randIndex = (size_t)((double) std::rand() / RAND_MAX * learnCount);
		}
		while(selectedIndexes.count(randIndex) != 0);
		// learn
		kohonen.Learn(AccesInData(randIndex).Data);
		ReportProgress(0, progressSize, i);
	}

	// space between nodes
	double nodeDistance = 10.0;
	int progress = progressSize;
	int progressNext = progress + (dataSize / 50);

	// calculate new positions	
	Kohohen<double, cParams>::NetworkType::const_iterator activeIter;
	for (size_t i = 0; i < dataSize; ++i)
	{	// we obtain new position
		activeIter = kohonen.Classify(AccesInData(i).Data);
		// we set up position
		std::vector<double>& data = AccesOutData(i).Data;
		data.resize(2);
		data[0] = activeIter->X * nodeDistance;
		data[1] = activeIter->Y * nodeDistance;
		// report progress
		++progress;
		if (progress > progressNext)
		{
			progressNext += (dataSize / 50);
			ReportProgress(0, progressSize, progress);
		}
	}

	// work done
	ReportProgress(0, progressSize, progressSize);
}

//										Module access											//

_DYNLINK void* CreateModule() 
{ 
	return new ModuleKohonen(); 
}

_DYNLINK void DeleteModule(void* module) 
{
	ModuleKohonen* convModule = reinterpret_cast<ModuleKohonen*>(module);
	delete convModule;
}

BaseLib::Description locDescription(std::pair<int, int>(1,0), 
	"Kohonen",
	"Provide kohonen (SOM) algorithm implementation.",
	"Provide kohonen (SOM) algorithm implementation. Kohonen network is implemented as 2 dimensional, so only "
	"reduction to 2 dimensional space can be done with this module."
	"Size of learning set and size of kohonen network can be set from module properties."
	"Learning data subset is selected randomly from all data."
	);

_DYNLINK const BaseLib::Description* GetDescription() 
{ 
	return &locDescription; 
}