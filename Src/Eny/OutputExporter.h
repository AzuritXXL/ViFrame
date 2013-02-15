#ifndef eny_outputExporter_h
#define eny_outputExporter_h

/**
 * @author 
 * @version 1.0
 * @date 20.5.2012
 */

// Standart libraries includes
///
#include <fstream>
#include <set>

// Local includes
///
#include "OutputBaseItem.h"
#include "OutputDataItem.h"
#include "OutputVirtualItem.h"

namespace Output
{

class Eporter
{
public:
	Eporter(const std::vector<std::string>& attributes, const std::string& objectName, const std::string& objectParent, std::string separator);
public:
	/**
	 * Open file for export. And write header.
	 * @param[in] file Output file.
	 * @param[in] header True if write down header.
	 */
	void Open(std::string file, bool header = true);
	/**
	 * Close export file.
	 */
	void Close();
	/**
	 * Export data item;
	 * @param[in] item Item to export.
	 */
	void Export(DataModels::Output::DataItem* item);
	/**
	 * Export child data in virtual item.
	 * @param[in] item Virtual item.
	 */
	void Export(DataModels::Output::VirtualItem* item);
private:
	/**
	 * Attributes name used to create output.
	 */
	const std::vector<std::string>& mAttributes;
	/**
	 * Attribute for object name.
	 */
	const std::string& mObjectName;
	/**
	 * Attribute for objects parent name.
	 */
	const std::string& mObjectParent;
	/**
	 * Data separator.
	 */
	std::string mSeparator;
	/**
	 * Output stream.
	 */
	std::ofstream mStream;
	/**
	 * Store exportet objects to prevend export same object twice.
	 */
	std::set<DataModels::Output::BaseItem*> mExportet;
};

}

#endif // eny_outputExport_h