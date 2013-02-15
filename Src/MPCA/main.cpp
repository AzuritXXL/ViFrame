#include "stdafx.h"
// BaseLib
#include "Description.hpp"
#include "LimitedProperty.hpp"
//...
#include "pcaMethod.h"
#include "main.h"

/**
 * Eigen (http://eigen.tuxfamily.org/index.php?title=Main_Page) library used.
 * Eigen is LGPL-licensed ( http://www.gnu.org/copyleft/lesser.html ).
 */


// transport out name from Visualise into VectorToType
std::string FullOutName;

/**
 * Convert @a input do @a output
 * @param[in] input data 
 * @param[out] output data
 */
void TypeToVector(const BaseClass::ObjectType& input, Eigen::VectorXd& output)
{
	std::vector<double>& data = dynamic_cast<BaseLib::Objects::VDouble* >(input.Object->GetMain())->Data;

	size_t size = data.size();
	output.resize(size, 1);
	for (size_t i = 0; i < size; ++i)
	{
		output[i] = data[i];
	}
}

/**
 * Convert @a input do @a output
 * @param[in] input data 
 * @param[out] output data
 */
void VectorToType(const Eigen::VectorXd& input, BaseClass::ObjectType& output)
{	// we use output data
	std::vector<double>& data = dynamic_cast<BaseLib::Objects::VDouble* >(output.Object->GetData().at(FullOutName))->Data;

	size_t size = input.size();
	data.resize(size);
	for (size_t i = 0; i < size; ++i)
	{
		data[i] = input[i];
	}
}

PCA::PCA()
	: Visualiser("PCA_Out"), mTargetDimension(2)
{
	mProperties.push_back(
		BaseLib::Properties::CreateLimited("Target dimension: ", "Output main data dimension. Must be less then input data dimension.", mTargetDimension, 1, 1048576)
	);
}

void PCA::Visualise()
{
	// check size
	size_t dataSize = DataSize();
	if (dataSize == 0)
	{	// no data
		return;
	}

	// get with FullOutName ( used in VectorToType )
	FullOutName = GetFullOutName();

	// get source dimension .. we will hope that lengh of all vector is equal .. 
	size_t sourceDimension = AccesInData(0).Data.size();

	// construct and set pca method
	PrincipalComponentAnalysis<BaseClass::ObjectType> pca;

	Visualiser::StorageType& innerStorage = AccesStorage();
	try
	{
		// init PCA
		pca.Init(sourceDimension, mTargetDimension);

		ReportMessage(BaseLib::iReportable::Information, "Generating transformation matrix ...");

		// generate transformation matrix
		pca.GenerateMatrix(innerStorage.cbegin(), innerStorage.cend(), TypeToVector);

		ReportMessage(BaseLib::iReportable::Information, "Transforming ...");

		// transform data
		auto iter = std::begin(innerStorage);
		auto iterEnd = std::end(innerStorage);
		// we will need both index as well as 
		for (; iter != iterEnd; ++iter)
		{
			// input output objects are the same
			pca.Transform((*iter), (*iter), TypeToVector, VectorToType);
		}
	}
	catch(std::exception ex)
	{
		ReportMessage(BaseLib::iReportable::Error, "Exception: " + std::string(ex.what()) );
		throw std::exception();
	}
}

//										Module access											//

_DYNLINK void* CreateModule() 
{ 
	return new PCA(); 
}

_DYNLINK void DeleteModule(void* module) 
{
	PCA* convModule = reinterpret_cast<PCA*>(module);
	delete convModule;
}

BaseLib::Description locDescription(std::pair<int, int>(1,0), 
	"PCA",
	"Module provide implementation of PCA.",
	"Module provide inmplementation of PCA (Principal component analysys.)"
	"for dimension reduction. Size of output data dimension can be set in module properties.</br></br>"
	"Module uses Eigen (http://eigen.tuxfamily.org/index.php?title=Main_Page),"
	"which is LGPL-licensed ( http://www.gnu.org/copyleft/lesser.html )."
	);

_DYNLINK const BaseLib::Description* GetDescription() 
{ 
	return &locDescription; 
}