#include "stdafx.h"
// ..
#include "mdsMethod.h"

MultidimensionalScaling::MultidimensionalScaling() 
	: mInputDataSize(1)
{ }

MultidimensionalScaling::~MultidimensionalScaling()
{	// for all cases just do clean up
	CleanUp();
}

void MultidimensionalScaling::Init(int dataSize, int inputDimension, int outputDimension)
{	// create matrixes
	mInputDataSize = dataSize;
	mDistances = Eigen::MatrixXd(dataSize, dataSize); // distances need no more init will be se by user
	mBMatrix = Eigen::MatrixXd(dataSize, dataSize); // B matrix will be computer in iteration
	mCoordinatesMatrix = Eigen::MatrixXd(dataSize, outputDimension); // set by user
	mTmpCoordinatesMatrix = Eigen::MatrixXd(dataSize, outputDimension); // new data will be used in iteration
}

void MultidimensionalScaling::AddDistance(int first, int other, double distance)
{	// store distances
	mDistances(first, other) = distance;
	mDistances(other, first) = distance;
}

void MultidimensionalScaling::AddFirstPosition(int index, const std::vector<double>& vector) 
{
	if (index < 0 || index >= mInputDataSize)
	{	// index out of range -> ignore
		return;
	}
	// set matrix coordinates
	int maxI = mCoordinatesMatrix.cols();
	// if maxI is higher than vector size, use vector size instead
	if (maxI > vector.size())
	{
		maxI = vector.size();
	}
	// set values
	for (int i = 0; i < maxI; ++i)
	{
		mCoordinatesMatrix(index, i) = vector[i];
	}
}

void MultidimensionalScaling::DoIteration()
{	// just call inner method 
	Iter();
}

std::vector<double> MultidimensionalScaling::CreateOutVector(int index)
{
	std::vector<double> result;
	// set size, even when we return data out of range, they will have good size
	result.resize(mCoordinatesMatrix.cols(), 0);

	std::stringstream Oss;
	Oss << mCoordinatesMatrix;
	std::string Ostr = Oss.str();

	if (index < 0 || index >= mInputDataSize)
	{	// index out of range
		return result;
	}

	int maxI = mCoordinatesMatrix.cols();
	for (int i = 0; i < maxI; ++i)
	{
		result[i] = mCoordinatesMatrix(index, i);
	}

	return result;
}

void MultidimensionalScaling::PrepareBMatrix()
{
	int maxRow = mBMatrix.rows();
	int maxCol = mBMatrix.cols();
	for (int row = 0; row < maxRow; ++row)
	{
		for (int col = 0; col < maxCol; ++col)
		{
			if ( row != col)
			{	// -1 * orginal distance * new distance 
				double actualDistance = 1;

				for (int i = 0; i < mCoordinatesMatrix.cols(); ++i)
				{
					double tempVal = mCoordinatesMatrix(row,i) - mCoordinatesMatrix(col,i);
					actualDistance += tempVal * tempVal;
				}
				actualDistance = std::sqrt(actualDistance);

				if (actualDistance == 0)
				{	// is zero
					mBMatrix(row, col) = 0;
				}
				else
				{
					double distance = mDistances(row, col);
					mBMatrix(row, col) = -distance / actualDistance;
				}
			}
			else
			{	// diagonal require data from other colums, so we will calculate it later
			}
		}
	}

	// matrix is squared .. now is the right time for computing dialgonal values -
	for (int i = 0; i < maxCol; ++i)
	{	// get refrence, for faster acces
		double& value = mBMatrix(i, i);
		value = 0;
		for (int j = 0; j < maxCol; ++j)
		{
			if ( j != i)
			{
				value += -mBMatrix(i, j);
			}
		}
	}
	// matrix is ready ... 
}

void MultidimensionalScaling::Iter()
{
	// preapare new BMatrix
	PrepareBMatrix();

	std::stringstream Bss;
	Bss << mBMatrix;
	std::string Bstr = Bss.str();

	assert(mInputDataSize != 0);
	// perform single step
	mTmpCoordinatesMatrix = (1/mInputDataSize)*( mBMatrix*mCoordinatesMatrix );

	std::stringstream Oss;
	Oss << mCoordinatesMatrix;
	std::string Ostr = Oss.str();

	std::stringstream Nss;
	Nss << mTmpCoordinatesMatrix;
	std::string Nstr = Nss.str();
	
	// move new data to old one, so actual data are keep in mCoordinatesMatrix
	std::swap(mTmpCoordinatesMatrix, mCoordinatesMatrix);
}

void MultidimensionalScaling::CleanUp()
{	// delete all matrixes
	mDistances = Eigen::MatrixXd();
	mBMatrix = Eigen::MatrixXd();
	mCoordinatesMatrix = Eigen::MatrixXd();
	mTmpCoordinatesMatrix = Eigen::MatrixXd();
}
