#ifndef separator_h
#define separator_h

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
#include "IntSelection.hpp"

typedef BaseLib::Modules::RandTransformer<BaseLib::Objects::Double, BaseLib::Objects::Int> BaseClass;

/**
 * Add to module functionality, that provide 
 * posibility to split object into groups according to value of a double attribute.
 */
class Separator : public BaseClass
{
public:
	Separator();
protected: // BaseLib::Modules::RandAdder
	virtual void Transform();
protected:
	/**
	 * Number of separation groups.
	 */
	int mGroups;
	/**
	 * True: split according to values
	 * False: split into groups with same size
	 */
	bool mUseValue;
};

#endif // separator_h