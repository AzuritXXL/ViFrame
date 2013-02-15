#ifndef eny_outputBaseItem_h
#define eny_outputBaseItem_h

/**
 * @author 
 * @version 1.0
 * @date 20.5.2012
 */

// Standart library includes
///
#include <ostream>

// Qt includes
///
#include <qstring.h>
#include <qvariant.h>
#include <qcolor.h>

namespace DataModels {
namespace Output {	

// Forward declaration
///
class VirtualItem;

/**
 * Base class for objects presentation.
 */
class BaseItem
{
public:
	/**
	 * @param parent item
	 */
	BaseItem(VirtualItem* parent);
	virtual ~BaseItem();
public:
	/**
	 * Append children. 
	 * @param[in] child to appedn
	 */
	virtual void AppendChild(BaseItem *child) = 0;
	/**
	 * Get child on row.
	 * @param[in] row
	 */
	virtual BaseItem* GetChild(int row) = 0;
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
	VirtualItem *GetParent();
	/**
	 * Return data dependent on passed role.
	 * @param[in] role
	 * @return object data
	 */
	virtual QVariant GetData(int role) const = 0;
	/**
	 * Set object parent.
	 * @param[in] new parent
	 */
	virtual void SetParent(VirtualItem* item);
	/**
	 * Set object color, pointer muset be valid for 
	 * whole time of object existance, or until the color is changed again.
	 * @param[in] pointer to new color
	 */
	virtual void SetColor(const QColor* color) = 0;
	/**
	 * Return object type.
	 */
	virtual int GetType() const = 0;
	/**
	 * Emit when selection change. 
	 * @parant[in] is item selected ?
	 */
	virtual void SelectionChange(bool selected);
	/**
	 * Serialize object data under certain key to stream.
	 * @param[in] key name
	 * @param[in] stream
	 */
	virtual void Serialize(const std::string& key, std::ostream& stream) = 0;
	/**
	 * Set graphics rep. visibility.
	 * @param[in] is visible ?
	 */
	virtual void SetVisible(bool visibility) = 0;
	/**
	 * @return true if object has visible status
	 */
	virtual bool GetVisible() const = 0;
	/**
	 * Return object color ( same color is used to draw text in treeView too)
	 * @return[in] object color
	 */
	virtual const QColor& GetColor() const = 0;
protected:
	/**
	 * Objects parent.
	 */
	VirtualItem* mParent;
};

} }

#endif // eny_outputBaseItem_h