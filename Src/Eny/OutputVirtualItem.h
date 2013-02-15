#ifndef eny_outputVirtualItem_h
#define eny_outputVirtualItem_h

/**
 * @author 
 * @version 1.0
 * @date 20.5.2012
 */

// Qt includes
///
#include <qlist.h>
#include <qcolor.h>

// Local includes
///
#include "OutputBaseItem.h"

namespace DataModels {
namespace Output {

/**
 * Is designed to organized OutputModel structure. 
 * VirtualItem as a child is owned by VirtualItem and he is responsible for his
 * deleting, virtual item also delete all child items include DataItem.
 */
class VirtualItem : public BaseItem
{
public:
	enum {
		Identification = 2
	};
public:
	VirtualItem(const QString& name, VirtualItem* parent);
	~VirtualItem();
public:
	/**
	 * Set object name.
	 */
	void SetName(const QString& name);
	/**
	 * @return object name
	 */
	const QString& GetName() const;
	/**
	 * Set object and all child color.
	 */
	void SetColor(const QColor& color);
	/**
	 * Retake object from item.
	 * @param[in] object that data will be retake from
	 */
	void RetakeItems(VirtualItem* source);
	/**
	 * Remove all Virtual items from mChild.
	 * Child of virtual item are add to mChild.
	 */
	void FlatSubItems();
	/**
	 * Remove item from child. Item still exist.
	 * @param[in] item to remove
	 */
	void RemoveChild(BaseItem* item);
	/**
	 * Return object index. 
	 * @param[in] item to which calculate index
	 * @return index
	 */
	int GetObjectRow(BaseItem* item);
	/**
	 * Give up all pointers to DataItem, VirtualItems are deleted, 
	 * call hierarchically on all sub items. 
	 */
	void DecayStructure();
public: // LibraryBaseItem
	virtual void AppendChild(BaseItem *child);
	virtual BaseItem* GetChild(int row);
	virtual int GetChildCount() const;
	virtual int GetColumnCount() const;
	virtual int GetRow() const;
	virtual QVariant GetData(int role) const;
public:
	virtual void SetColor(const QColor* color);
	virtual int GetType() const;
	virtual void Serialize(const std::string& key, std::ostream& stream);
	virtual void SetVisible(bool visibility);
	virtual bool GetVisible() const;
	virtual const QColor& GetColor() const;
private:
	/**
	 * Showed text.
	 */
	QString mName;
	/**
	 * Children.
	 */
	QList<BaseItem*> mChilds;
	/**
	 * Item and subitem color.
	 */
	QColor mColor;
};

} }

#endif // eny_outputVirtualItem_h