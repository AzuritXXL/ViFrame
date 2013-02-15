#ifndef baseLib_propoerties_basePoperty_h
#define baseLib_propoerties_basePoperty_h

/**
 * Contains base class for modules property.
 *
 * @author
 * @version 1.0
 * @date 1.5.2012
 */

// Standart library includes
///
#include <string>

namespace BaseLib {
namespace Properties {

// Forward declaration
///
struct PropertyCreator;

/**
 * Base class for property.
 */
class BaseProperty
{
public:
	/**
	 * Types of properties.
	 */
	enum eTypes
	{
		Empty = 0
		,Int = 1
		,Double = 2
		,String = 4
		,Bool = 8
		// Extension types
		,StringFile = 4 + 16
		,IntSelection = 1 + 16
	};
public:
	/**
	 * @param[in] type Property type. Values from eTypes.
	 * @param[in] name Property name.
	 * @param[in] description Property description.
	 */
	BaseProperty(eTypes type, const std::string& name, const std::string& description);
	virtual ~BaseProperty();
public:
	/**
	 * Returns property type.
	 * @return property type
	 */
	eTypes GetType() const throw();
	/**
	 * Returns property name.
	 * @return property name
	 */
	const std::string& GetName() const throw();
	/**
	 * Returns property description.
	 * @return property description
	 */
	const std::string& GetDescription() const throw();
protected:
	/**
	 * Property type.
	 */
	eTypes mType;
	/**
	 * Property name.
	 */
	std::string mName;
	/**
	 * Property description.
	 */
	std::string mDescription;
};

} }

#endif // baseLib_propoerties_basePoperty_h