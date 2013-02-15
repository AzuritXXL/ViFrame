#ifndef mds_h
#define mds_h

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
 * Wrap MDS algorithm to module class.
 */
class MDS : public BaseClass
{
public:
	MDS();
protected: // BaseLib::Modules::Visualiser
	virtual void Visualise();
protected:
	/**
	 * Target dimension.
	 */
	int mTargetDimension;
	/**
	 * Number of iterations.
	 */
	int mIterCount;
};

#endif // mds_h