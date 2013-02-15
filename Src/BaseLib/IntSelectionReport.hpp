#ifndef baseLib_properties_intSelectionReport_h
#define baseLib_properties_intSelectionReport_h

/**
 * Contains class for int property, values 
 * are determined using an index to std::vector<std::string>, report changes.
 *
 * @author
 * @version 1.0
 * @date 1.5.2012
 */

// Standart library includes
// 
#include <vector>

// Local includes
/// 
#include "BaseProperty.hpp"
#include "Property.hpp"
#include "ReportProperty.hpp"
#include "IntSelection.hpp"

namespace BaseLib {
namespace Properties {

/**
 * Class subclas IntSelection and add report 
 * function.
 */
class IntSelectionReport : public IntSelection
{
public:
	virtual bool SetProperty(const int& data, std::string& log);
public:
	friend BaseProperty* CreateIntSelectionReport(const std::string& name, const std::string& description, int& data, IntSelection::ValuesType& values, Reportable *report);
	friend BaseProperty* CreateIntSelectionReport(const std::string& name, const std::string& description, int& data, const IntSelection::ValuesType& values, Reportable *report);
protected:
	/**
	 * Pointer to report class.
	 */
	Reportable* mReport;
protected:
	/**
	 * Ctor swap data from values.
	 * @param[in] type Property type.
	 * @param[in] name Property name.
	 * @param[in] description Property description.
	 * @param[in] data Associated value.
	 * @param[in] values Values from which to select.
	 * @param[in] report Report class.
	 */
	IntSelectionReport(const std::string& name, const std::string& description, int& data, ValuesType& values, Reportable *report);
	/**
	 * Ctor copy data from values.
	 * @param[in] type Property type.
	 * @param[in] name Property name.
	 * @param[in] description Property description.
	 * @param[in] data Associated value.
	 * @param[in] values Values from which to select.
	 * @param[in] report Report class.
	 */
	IntSelectionReport(const std::string& name, const std::string& description, int data, const ValuesType& values, Reportable *report);
};

BaseProperty* CreateIntSelectionReport(const std::string& name, const std::string& description, int& data, IntSelection::ValuesType& values, Reportable *report);
BaseProperty* CreateIntSelectionReport(const std::string& name, const std::string& description, int& data, const IntSelection::ValuesType& values, Reportable *report);

} }

#endif // baseLib_properties_intSelectionReport_h