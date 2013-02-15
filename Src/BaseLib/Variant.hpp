#ifndef baseLib_objects_variant_h
#define baseLib_objects_variant_h

/**
 * Contains base abstract data object template.
 *
 * @author
 * @version 1.0
 * @date 1.5.2012
 */

namespace BaseLib {
namespace Objects {

/**
 * Base abstract class for data value.
 */
struct Variant
{
public:
	/**
	 * Variant types.
	 */
	enum eTypes
	{
		Int = 1				/*!< int */
		,Double = 2			/*!< double */
		,String = 3			/*!< std::string */
		,VectorInt = 4		/*!< std::vector<int> */
		,VectorDouble = 5	/*!< std::vector<double> */
	};
public:
	/**
	 * Return object identification, value of eTypes.
	 * @return Object indentification.
	 */
	virtual int GetType() const = 0;
	/**
	 * Return string representation.
	 */
	virtual std::string ToString() const = 0;
};

} }

#endif	// baseLib_objects_variant_h