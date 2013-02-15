#ifndef multidimensionalScaling_h
#define multidimensionalScaling_h

// Standart library includes
//
#include <vector>
// Eigen library includes
//
#include "Eigen/Dense"

/**
 * Eigen (http://eigen.tuxfamily.org/index.php?title=Main_Page) library used.
 * Eigen is LGPL-licensed ( http://www.gnu.org/copyleft/lesser.html ).
 */

/**
 * When using user call Init, after that must set all distances by calling AddDistance,
 * also must call AddFirstPosition to set initial posiion of all object.
 * Without this initialization MDS will not work properly !
 */
class MultidimensionalScaling
{
public:
	MultidimensionalScaling();
	~MultidimensionalScaling();
public:
	/**
	 * Allocate inner matrixes for MDS.
	 * @param[in] dataSize Nnumber of vector to work with.
	 * @param[in] inputDimension Size of input vectors.
	 * @param[in] outputDimension Size of output vectors.
	 */
	void Init(int dataSize, int inputDimension, int outputDimension);
	/**
	 * Set distances between two objects
	 * @param[in] first Index of first object.
	 * @param[in] other Index of other object.
	 * @param[in] distance Distance between objects.
	 */
	void AddDistance(int first, int other, double distance);
	/**
	 * Set start vector position in reduced dimension. If use with good coordinates can speed up.
	 * @param[in] index Index of vector.
	 * @param[in] vector Start vector position.
	 */
	void AddFirstPosition(int index, const std::vector<double>& vector);
	/**
	 * Do single iteration.
	 */
	void DoIteration();
	/**
	 * Create a vector of actual output coordinates of object.
	 * @param[in] index Object index.
	 * @return if index out of range empty vector is returned
	 */
	std::vector<double> CreateOutVector(int index);
private:
	/**
	 * Number of objecsts.
	 */
	double mInputDataSize;
	/**
	 * Matrix of input distances.
	 */
	Eigen::MatrixXd mDistances;
	/**
	 * Main calculation matrix, constructed in each calculation.
	 */
	Eigen::MatrixXd mBMatrix;
	/**
	 * Store object coordination.
	 */
	Eigen::MatrixXd mCoordinatesMatrix;
	/**
	 * Temporary matrix, used to temporary store coordinats.
	 */
	Eigen::MatrixXd mTmpCoordinatesMatrix;
private:
	/**
	 * Prepare mBMatrix from mCoordinatesMatrix for computating.
	 */
	void PrepareBMatrix();
	/**
	 * Do one single iteration.
	 */
	void Iter();
	/**
	 * Release all allocated resources.
	 */
	void CleanUp();
};


#endif // multidimensionalScaling_h