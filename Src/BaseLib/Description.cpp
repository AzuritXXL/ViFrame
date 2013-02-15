#include "stdafx.h"
// ...
#include "Description.hpp"

namespace BaseLib {
	
Description::Description(std::pair<int, int> version, const std::string& name, const std::string& shortDesc, const std::string& longDesc, bool addHtmlCode)
	: Version(version), Name(name), ShortDescription(shortDesc)
{
	if (addHtmlCode)
	{	// add some html code .. 
		LongDescription = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"><html><body>" +
			longDesc +
			"</body></html>";
	}
	else
	{	// do not add hmtl code
		LongDescription = longDesc;
	}
}

}