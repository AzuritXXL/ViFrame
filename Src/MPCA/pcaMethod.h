#ifndef principalComponentAnalysis_h
#define principalComponentAnalysis_h

/**
 * Contains template that implents principal component analysis 
 * algorithm.
 *
 * @author 
 * @version 1.0
 * @date 1.5.2012
 *
 * Eigen (http://eigen.tuxfamily.org/index.php?title=Main_Page) library used.
 * Eigen is LGPL-licensed ( http://www.gnu.org/copyleft/lesser.html ).
 */

// Standart library includes
//
#include <vector>
#include <algorithm>
#include <fstream>

// Eigen library includes
///
#include "Eigen/Dense"

/**
 * Implement PCA algortihm, if outputDimension < inputDimension can be used to dimension reduction.
 * 
 * void TypeToVector(const DataType& input, Eigen::VectorXd& output)
 * void VectorToType(const Eigen::VectorXd& output, DataType& input)
 * 
 * GenerateMatrix and Transform used methods (TypeToVector and VectorToType) to convert object to Eigen::VectorXd and back.
 *
 * When pca_not_require_mean is defined PCA center data, otherwise PCA 
 * realy on source data to be centred.
 */
template<typename DataType> class PrincipalComponentAnalysis
{
public:
	PrincipalComponentAnalysis() : mInputDimension(0), mOutputDimension(0), mTransformMatrix(0)
	{
	}
	~PrincipalComponentAnalysis()
	{
		Clear();
	}
public:
	/**
	 * Set basic property.
	 * @param[in] inputDimension Input dimension size.
	 * @param[in] outputDimension Output dimension size.
	 * @exception std::exception exception
	 */
	void Init(const size_t& inputDimension, const size_t& outputDimension)
	{
		mInputDimension = inputDimension;
		mOutputDimension = outputDimension;
		if (mInputDimension < mOutputDimension)
		{
			throw std::exception("PrincipalComponentAnalysis::Init : mInputDimension > mOutputDimension");
		}
	}
	/**
	 * Generate transformation matrix.
	 * @param[in] begin Iterator begin of verticies.
	 * @param[in] end Iterator end of verticies.
	 * @param[in] TypeToVector Pointer to conversion function void TypeToVector(const DataType& input, Eigen::VectorXd& output) .
	 * @exception std::exception exception
	 */
	template<typename TTV> void GenerateMatrix( typename std::vector<DataType>::const_iterator& begin, 
							typename std::vector<DataType>::const_iterator& end,
							TTV TypeToVector)
	{
		// calculate mean (if need) and data count
		size_t inputDataSize = 0;
		
#ifdef pca_not_require_mean
		mMean = Eigen::VectorXd::Zero(mInputDimension);
#endif
		for(std::vector<DataType>::const_iterator iter = begin; iter != end; ++iter)
		{	
			inputDataSize++;
#ifdef pca_not_require_mean
			TypeToVector( (*iter), mTempVector);
			mMean += mTempVector;
#endif
		}
#ifdef pca_not_require_mean
		mMean /= (double)inputDataSize;
#endif

		if (inputDataSize == 0)
		{
			throw std::exception("PrincipalComponentAnalysis::GenerateMatrix : No input data to create transformation Matrix!");
		}

		// create covariance matrix
		Eigen::MatrixXd *covarianceMatrix = new Eigen::MatrixXd((int)mInputDimension, (int)mInputDimension);
		// for each postion in matrix
		for(size_t y = 0; y < mInputDimension; ++y)
		{
			for(size_t x = y; x < mInputDimension; ++x)
			{	
				// calculate covariance for value ( data are not centred, we have to substract mean before use )
				double value = 0;
				size_t inputDataSize = 0;

				for(typename std::vector<DataType>::const_iterator iter = begin; iter != end; ++iter)
				{
					TypeToVector((*iter), mTempVector);

					auto xV = mTempVector[x];
					auto yV = mTempVector[y];

					// center used values and then use them
#ifdef pca_not_require_mean
					value += (mTempVector[x] - mMean(x)) * (mTempVector[y] - mMean(y));
#else
					value += mTempVector[x] * mTempVector[y];
#endif
					// count number of objects
					inputDataSize++;
				}
				value /= inputDataSize;

				// tha matrix is symetric
				(*covarianceMatrix)(x,y) = value;
				(*covarianceMatrix)(y,x) = value;
			}
		}

		// calculate eigen vectors and values
		Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> solver(*covarianceMatrix); // Solve .. 
		Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd>::RealVectorType eigens = solver.eigenvalues();
		Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd>::MatrixType eigenVectors =  solver.eigenvectors();	

		// find largest eigen values
		std::vector<int> highest;
		for(size_t d = 0 ; d < mOutputDimension; ++d)
		{	// start with first value			
			size_t maxIndex = 0;
			
			auto containMax = std::find(std::begin(highest), std::end(highest), maxIndex);
			while (containMax != std::end(highest))
			{	// max already used
				maxIndex++;
				containMax = std::find(std::begin(highest), std::end(highest), maxIndex);
			}

			double max = eigens[maxIndex];

			// go through all
			for(size_t i = 0; i < eigens.size(); ++i)
			{	// if we find greater value
				if( eigens[i] > max) // prevetn from pickup same value more times
				{	// store it as max value

					auto contain = std::find(std::begin(highest), std::end(highest), i);
					if (contain == std::end(highest))
					{	// value is not used
						max = eigens[i];
						maxIndex = i;
					}
				}
			}
			// store new min value so it will not be used again
			highest.push_back(maxIndex);
		}
		
		// create transformation matrix
		mTransformMatrix = new Eigen::MatrixXd((int)mOutputDimension, (int)mInputDimension);
		for(size_t y = 0 ; y < (*mTransformMatrix).rows(); ++y)	
		{
			for(size_t x = 0; x < (*mTransformMatrix).cols(); ++x)	
			{
				(*mTransformMatrix)(y, x) = eigenVectors(x, highest[y]);
			}
		}
		
		/*

		// for debug purpose
		std::stringstream sc;
		sc << *covarianceMatrix;
		std::string c = sc.str();

		std::stringstream se;
		se << eigenVectors;
		std::string e = se.str();

		std::stringstream st;
		st << *mTransformMatrix;
		std::string t = st.str();
		
		*/

		// release covariance matrix
		delete covarianceMatrix;
		covarianceMatrix = 0;
	}
	/**
	 * Transform single vector using transformation matrix.
	 * @param[in] input Cector to transform.
	 * @param[in] TypeToVector Pointer to convert function void TypeToVector(const DataType& input, Eigen::VectorXd& output).
	 * @param[in] VectorToType Pointer to convert function void VectorToType(const Eigen::VectorXd& output, DataType& input).
	 * @return transformed vector
	 */
	template<typename TTV, typename VTT> void Transform(
		const DataType& input, DataType& outData, TTV TypeToVector, VTT VectorToType)
	{
		assert(mTransformMatrix != 0);
		// convert to vector
		TypeToVector(input, mTempVector);
		// transform, but first substract mean
#ifdef pca_not_require_mean
		mTempVector = (*mTransformMatrix) * (mTempVector - mMean);
#else
		mTempVector = (*mTransformMatrix) * mTempVector;
#endif
		// convert back to vector
		VectorToType(mTempVector, outData);
	}
protected:
	/**
	 * Input dimension size.
	 */
	size_t mInputDimension;
	/**
	 * Output dimension size.
	 */
	size_t mOutputDimension;
	/**
	 * Transformation matrix.
	 */
	Eigen::MatrixXd *mTransformMatrix;
	/**
	 * Temporary vector, used for stored converted data.
	 */
	Eigen::VectorXd mTempVector;
#ifdef pca_not_require_mean
	/**
	 * Calculated mean.
	 */
	Eigen::VectorXd mMean;
#endif
protected:
	/**
	 * Clear data.
	 * Set dimensions to zero.
	 */
	void Clear()
	{
		mInputDimension = 0;
		mOutputDimension = 0;
		if (mTransformMatrix != nullptr)
		{
			delete mTransformMatrix;
			mTransformMatrix = nullptr;
		}
	}
};


#endif // principalComponentAnalysis_h