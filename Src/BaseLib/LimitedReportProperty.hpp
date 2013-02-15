#ifndef baseLib_properties_limitedReportProperty_h
#define baseLib_properties_limitedReportProperty_h

/**
 * Contains class for limited and repot property.
 *
 * @author
 * @version 1.0
 * @date 1.5.2012
 */

// Local includes
///
#include "LimitedProperty.hpp"
#include "ReportProperty.hpp"

namespace BaseLib {
namespace Properties {

/** 
 * Class for limited property which reports changes.
 */
template<typename TYPE> class LimitedReportProperty : public Property<TYPE>
{
public: // Property
	virtual bool SetProperty(const TYPE& data, std::string& log)
	{
		if (data < mMinValue || data > mMaxValue)
		{	// prepare error message
			std::stringstream ss;
			ss << "Value must be greater or equal than " << mMinValue << " and less or equal than " << mMaxValue;
			log = ss.str();
			return false;
		}

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
	friend BaseProperty* CreateLimitedReport(const std::string& name, const std::string& description, double& data, double min, double max, Reportable* report);
	friend BaseProperty* CreateLimitedReport(const std::string& name, const std::string& description, int& data, int min, int max, Reportable* report);
protected:
	/**
	 * Pointer to the report class.
	 */
	Reportable* mReport;
	/**
	 * Max value.
	 */
	TYPE mMaxValue;
	/**
	 * Min value.
	 */
	TYPE mMinValue;
protected:
	/**
	 * Max value should be greater than Min value, if not they are switched.
	 * @param[in] type Property type.
	 * @param[in] name Property name.
	 * @param[in] description Property description.
	 * @param[in] data Associated value.
	 * @param[in] report Class to which report changes.
	 */
	LimitedReportProperty(BaseProperty::eTypes type,  const std::string& name, const std::string& description, TYPE& data, TYPE min, TYPE max, Reportable *report)
		: Property<TYPE> (type, name, description, data), mReport(report), mMaxValue(max), mMinValue(min)
	{
		// check that min is less than max .. of not swap them
		assert(mMinValue < mMaxValue);
		if (mMinValue > mMaxValue)
		{
			std::swap(mMaxValue, mMinValue);	
		};
	}
};

BaseProperty* CreateLimitedReport(const std::string& name, const std::string& description, double& data, double min, double max, Reportable* report);
BaseProperty* CreateLimitedReport(const std::string& name, const std::string& description, int& data, int min, int max, Reportable* report);

} }

#endif  // baseLib_properties_limitedReportProperty_h