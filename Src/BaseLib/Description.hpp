#ifndef baseLib_description_h
#define baseLib_description_h

/**
 * Contains class for module description.
 *
 * @author
 * @version 1.0
 * @date 1.5.2012
 */

// Standart library
///
#include <vector>

namespace BaseLib {

/**
 * Module descripton class.
 */
struct Description
{
public:
	/**
	 * Version <major, minor>
	 */
	std::pair<int, int> Version;
	/**
	 * Module name.
	 */
	std::string Name;
	/**
	 * Short description.
	 */
	std::string ShortDescription;
	/**
	 * Long description. Can be HTML.
	 */
	std::string LongDescription;
public:
	/**
	 * @param[in] version Version.
	 * @param[in] name Module name.
	 * @param[in] shortDesc Short description.
	 * @param[in] longDesc Long description.
	 * @param[in] addHtmlCode If true longDesc is considered to be just an HTML body and additional HTML code is added.
	 */
	Description(std::pair<int, int> version, const std::string& name, const std::string& shortDesc, const std::string& longDesc, bool addHtmlCode = true);
};

}

#endif // baseLib_description_h
