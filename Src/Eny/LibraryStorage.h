#ifndef eny_libraryStorage_h
#define eny_libraryStorage_h

/**
 * @author 
 * @version 1.0
 * @date 20.5.2012
 */

// Standart libraries includes
///
#include <map>
#include <string>

// Local includes
///
#include "Library.h"

namespace Logic {

/**
 * Hold libraries and provide acces to them via id.
 * Class also owes stored Libraries and care about theire release.
 */
class LibraryStorage
{
private:
	/**
	 * Library storage.
	 */
	std::map<std::string, Library*> mLibraries;
public:
	LibraryStorage();
	/**
	 * Delete stored libraries. Is exception save, exception are supressed.
	 * Does not guaranted succesfully releasig of all Libraries.
	 */
	~LibraryStorage();
public:
	/**
	 * Create new library and return pointer to it.
	 * @param[in] path to library
	 * @param reference to new created library
	 */
	Library& Create(const std::string& filePath);
	/**
	 * Return library with associated id.
	 * @param[in] library id
	 * @return null if library has not been found
	 */
	Library* GetLibrary(const std::string& id) throw();
};

}

#endif // eny_libraryStorage_h