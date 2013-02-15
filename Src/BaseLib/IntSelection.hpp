#ifndef baseLib_propoerties_intSelection_h
#define baseLib_propoerties_intSelection_h

/**
 * Contains class for int property, value is 
 * stored to std::vector<std::string>.
 *
 * @author
 * @version 1.0
 * @date 1.5.2012
 */

// Standard library includes
///
#include <vector>

// Local includes
///
#include "BaseProperty.hpp"
#include "Property.hpp"

namespace BaseLib {
namespace Properties {

/**
 * Class provides int property, values of which 
 * are determined using an index to std::vector<std::string>.
 */
class IntSelection : public Property<int>
{
public:
	typedef std::vector<std::string> ValuesType;
public:
	/**
	 * Ctor swap data from values.
	 * @param[in] type Property type.
	 * @param[in] name Property name.
	 * @param[in] description Property description.
	 * @param[in] data Associated value.
	 * @param[in] values Values from which to select.
	 */
	IntSelection(const std::string& name, const std::string& description, int& data, ValuesType& values);
	/**
	 * Ctor copy data from values.
	 * @param[in] type Property type.
	 * @param[in] name Property name.
	 * @param[in] description Property description.
	 * @param[in] data Associated value.
	 * @param[in] values Values from which to select.
	 */
	IntSelection(const std::string& name, const std::string& description, int data, const ValuesType& values);
public: // IntSelection
	ValuesType& GetValues();
public: // Property
	virtual bool SetProperty(const int& data, std::string& log);
	virtual bool SetPropertyLoad(const int& data);
public:
	friend BaseProperty* CreateIntSelection(const std::string& name, const std::string& description, int& data, ValuesType& values);
	friend BaseProperty* CreateIntSelection(const std::string& name, const std::string& description, int& data, const ValuesType& values);
protected:
	/**
	 * Values that can be selected.
	 */
	ValuesType mValues;
};

BaseProperty* CreateIntSelection(const std::string& name, const std::string& description, int& data, IntSelection::ValuesType& values);
BaseProperty* CreateIntSelection(const std::string& name, const std::string& description, int& data, const IntSelection::ValuesType& values);


} }

#endif // baseLib_propoerties_intSelection_h