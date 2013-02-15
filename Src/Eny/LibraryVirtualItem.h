#ifndef eny_libraryVirtualItem_h
#define eny_libraryVirtualItem_h

/**
 * @author 
 * @version 1.0
 * @date 20.5.2012
 */

// Qt incldues
///
#include <qlist.h>

// Local includes
///
#include "Library.h"
#include "LibraryBaseItem.h"

namespace DataModels {
namespace Library {

class LibraryVirtualItem : public LibraryBaseItem
{
public:
	LibraryVirtualItem(QString& name, LibraryBaseItem* parent);
	~LibraryVirtualItem();
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
	 * Showed text.
	 */
	QString mName;
	/**
	 * Children.
	 */
	QList<LibraryBaseItem*> mChilds;
};

} }

#endif // eny_libraryVirtualItem_h