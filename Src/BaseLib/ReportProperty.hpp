#ifndef baseLib_properties_reportProperty_h
#define baseLib_properties_reportProperty_h

/**
 * Contains class for property that report changes.
 *
 * @author
 * @version 1.0
 * @date 1.5.2012
 */

// Standart library includes
///
#include <assert.h>
#include <sstream>
#include <algorithm>

// Local includes
///
#include "BaseProperty.hpp"
#include "Property.hpp"

namespace BaseLib {
namespace Properties {

/**
 * Abstract interface for class to which 
 * ReportProperty can report changes.
 */
class Reportable
{
public:
	/**
	 * Called when property changed.
	 * @param[in] Changed property.
	 */
	virtual void OnChange(BaseProperty*) = 0;
};

/**
 * Template for property that report changes.
 * TYPE - data type
 */
template<typename TYPE> class ReportProperty : public Property<TYPE>
{
public: // Property
	virtual bool SetProperty(const TYPE& data, std::string& log)
	{
		bool result = Property<TYPE>::SetProperty(data, log);
		if (result)
		{	// report change 
			if (mReport != nullptr)
			{
				mReport->OnChange(this);
			}
			return true;
		}
		else
		{
			return false;
		}
	}
public:
	friend BaseProperty* CreateReport(const std::string& name, const std::string& description, bool& data, Reportable* report);
	friend BaseProperty* CreateReport(const std::string& name, const std::string& description, double& data, Reportable* report);
	friend BaseProperty* CreateReport(const std::string& name, const std::string& description, int& data, Reportable* report);
	friend BaseProperty* CreateReport(const std::string& name, const std::string& description, std::string& data, Reportable* report);
protected:
	/**
	 * Pointer to the report class.
	 */
	Reportable* mReport;
protected:
	/**
	 * Max value should be greater than Min value, if not they are switched.
	 * @param[in] type Property type.
	 * @param[in] name Property name.
	 * @param[in] description Property description.
	 * @param[in] data Associated value.
	 * @param[in] report Report class.
	 */
	ReportProperty(BaseProperty::eTypes type,  const std::string& name, const std::string& description, TYPE& data, Reportable *report)
		: Property<TYPE> (type, name, description, data), mReport(report)
	{ };
};

BaseProperty* CreateReport(const std::string& name, const std::string& description, bool& data, Reportable* report);
BaseProperty* CreateReport(const std::string& name, const std::string& description, double& data, Reportable* report);
BaseProperty* CreateReport(const std::string& name, const std::string& description, int& data, Reportable* report);
BaseProperty* CreateReport(const std::string& name, const std::string& description, std::string& data, Reportable* report);

} } // baseLib_properties_reportProperty_h


#endif
