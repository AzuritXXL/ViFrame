#ifndef eny_libraryDirCreator_h
#define eny_libraryDirCreator_h

/**
 * @author 
 * @version 1.0
 * @date 20.5.2012
 */

// Qt includes
///
#include <qdir.h>

// Local includes
///
#include "LibraryVirtualItem.h"

namespace Logic {

// Forward declaration
///
class LibraryStorage;

/**
 * Class is used to create root item for LibraryModel
 * reflecting file structure.
 */
class LibraryDirCreator
{
public:
	/**
	 * Build tree according to file structure. 
	 * @param root
	 * @param LibraryStorage 
	 * @param path to root dir of application
	 * @param actual dir
	 * @param filter
	 */
	static void Create(DataModels::Library::LibraryVirtualItem* root, LibraryStorage& manager, QDir& rootDir, QDir& dir, const QString& fileFilter);
private:
	LibraryDirCreator();
};

}

#endif // eny_libraryDirCreator_h