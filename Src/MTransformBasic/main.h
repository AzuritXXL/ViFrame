#ifndef basicTransform_h
#define basicTransform_h

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
#include "Variable.hpp"
#include "RandTransformer.hpp"

typedef BaseLib::Modules::RandTransformer<BaseLib::Objects::VDouble, BaseLib::Objects::VDouble> BaseClass;

/**
 * Class provide basic data transformation.
 *
 */
class BasicTransform : public BaseClass
{
public:
	BasicTransform();
protected:
	virtual void Transform();
protected:
	/**
	 * True if data should be centered.
	 */
	bool mCenter;
	/**
	 * True if data should by devide with standart deviation.
	 */
	bool mScale;
};

#endif // basicTransform_h