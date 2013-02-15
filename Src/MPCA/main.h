#ifndef pca_h
#define pca_h

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
 * Wrap PCA algorithm to module class.
 */
class PCA : public BaseClass
{
public:
	PCA();
protected: // BaseLib::Modules::Visualiser
	virtual void Visualise();
protected:
	/**
	 * Target dimension.
	 */
	int mTargetDimension;
};

#endif // pca_h