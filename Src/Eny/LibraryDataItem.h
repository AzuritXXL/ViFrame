#ifndef eny_libraryDataItem_h
#define eny_libraryDataItem_h

/**
 * @author 
 * @version 1.0
 * @date 20.5.2012
 */

// Qt includes
///
#include <qvariant.h>

// Local includes
///
#include "Library.h"
#include "LibraryBaseItem.h"

namespace DataModels {
namespace Library {

class LibraryDataItem : public LibraryBaseItem
{
public:
	LibraryDataItem(Logic::Library& library, QString& name, LibraryBaseItem* parent);
public:
	/**
	 * @return refrence to holded library
	 */
	Logic::Library& GetLibrary();
public: // LibraryBaseItem
	virtual void AppendChild(LibraryBaseItem *child);
	virtual LibraryBaseItem* GetChild(int row);
	virtual int GetChildCount() const;
	virtual int GetColumnCount() const;
	virtual int GetRow() const;
	virtual QVariant GetData(int role) const;
	virtual void Load();
	virtual void UnLoad();
	virtual int GetChildRow(const LibraryBaseItem* item) const;
	virtual bool GetHoldLibrary() const;
private:
	/**
	 * Holded library.
	 */
	Logic::Library& mLibrary;
	/**
	 * Showed text.
	 */
	QString mName;
	/**
	 * Item tool tip.
	 */
	QString mToolTip;
};

} }

#endif // eny_libraryDataItem_h