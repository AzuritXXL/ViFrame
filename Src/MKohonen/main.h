#ifndef moduleKohonen_h
#define moduleKohonen_h

/**
 * Contains main module class definition. It's ment to be used
 * as a part of visualisation pathway.
 *
 * @author 
 * @version 1.0
 * @date 1.5.2012
 */

// BaseLib includes
///
#include "Vector.hpp"
#include "Visualiser.hpp"

typedef BaseLib::Modules::Visualiser<BaseLib::Objects::VDouble, BaseLib::Objects::VDouble> BaseClass;

/**
 * Wrap Kohonen (SOM) algorithm to module class.
 */
class ModuleKohonen : public BaseClass
{
public:
	ModuleKohonen();
protected: // BaseLib::Modules::Visualiser
	virtual void Visualise();
protected:
	/**
	 * Size of learning set in percentage of source size.
	 */
	double mLearningSetSize;
	/**
	 * Size of network set in percentage of source size.
	 */
	double mNetworkSize;
};

#endif // pca_h