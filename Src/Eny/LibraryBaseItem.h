#ifndef eny_libraryBaseItem_h
#define eny_libraryBaseItem_h

/**
 * @author 
 * @version 1.0
 * @date 20.5.2012
 */

// Qtincludes
/// 
#include <qstring.h>

namespace DataModels {
namespace Library {	

/**
 * Basic tree item to organize Library item in ModelView.
 */
class LibraryBaseItem
{
public:
	/**
	 * @param parent item
	 */
	LibraryBaseItem(LibraryBaseItem* parent);
	virtual ~LibraryBaseItem();
public:
	/**
	 * Append children. 
	 * @param[in] child to appedn
	 */
	virtual void AppendChild(LibraryBaseItem *child) = 0;
	/**
	 * Get child on row.
	 * @param[in] row
	 */
	virtual LibraryBaseItem* GetChild(int row) = 0;
	/**
	 * @return number of childs
	 */
	virtual int GetChildCount() const = 0;
	/**
	 * @return colums count
	 */
	virtual int GetColumnCount() const = 0;
	/**
	 * @return item row in parent
	 */
	virtual int GetRow() const = 0;
	/**
	 * @return parent
	 */
	LibraryBaseItem *GetParent();
	/**
	 * Return data dependent on passed role.
	 * @param[in] role
	 * @return object data
	 */
	virtual QVariant GetData(int role) const = 0;
	/**
	 * Load item and all sub items libraries.
	 * @exception std::exception propagate from Library
	 */
	virtual void Load() = 0;
	/**
	 * Unload item and all sub items libraries.
	 * @exception std::exception propagate from Library
	 */
	virtual void UnLoad() = 0;
	/**
	 * Return item row.
	 * @return item row of -1 if item is not child
	 */
	virtual int GetChildRow(const LibraryBaseItem* item) const = 0;
	/**
	 * @return true if object hold library
	 */
	virtual bool GetHoldLibrary() const = 0;
protected:
	/**
	 * Objects parent.
	 */
	LibraryBaseItem* mParent;
};

} }

#endif // eny_libraryBaseItem_h