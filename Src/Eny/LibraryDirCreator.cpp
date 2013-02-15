#include "stdafx.h"
// ..
#include "Library.h"
#include "LibraryStorage.h"
#include "LibraryDataItem.h"
#include "LibraryDirCreator.h"

using namespace DataModels::Library;

namespace Logic {

LibraryDirCreator::LibraryDirCreator()
{ }

void LibraryDirCreator::Create(DataModels::Library::LibraryVirtualItem* root, LibraryStorage& manager, QDir& rootDir, QDir& dir, const QString& fileFilter)
{
	// go throught dirs
	QStringList folders = dir.entryList( QDir::AllDirs | QDir::NoDotAndDotDot);
	for (QStringList::Iterator iter = std::begin(folders); iter != std::end(folders); ++iter)
	{
		dir.cd(*iter);

		LibraryVirtualItem* subRoot = new LibraryVirtualItem(*iter, root);
		// create subfolder structure
		LibraryDirCreator::Create(subRoot, manager, rootDir, dir, fileFilter);
		// add item
		root->AppendChild(subRoot);

		dir.cdUp();
	}

	// add files
	QStringList files = dir.entryList( QStringList(fileFilter), QDir::Files );
	for (QStringList::Iterator iter = std::begin(files); iter != std::end(files); ++iter)
	{		
		// add / seprator to the end of directory
		QString relPath = rootDir.relativeFilePath(dir.path() + '/' + *iter);	
		// create data item
		LibraryDataItem* dataItem = new LibraryDataItem(manager.Create(relPath.toStdString()), *iter, root);
		// add item
		root->AppendChild(dataItem);
	}
}

}

