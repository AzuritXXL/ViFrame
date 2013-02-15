#ifndef eny_outputDataItem_h
#define eny_outputDataItem_h

/**
 * @author 
 * @version 1.0
 * @date 20.5.2012
 */

// Standart includes
///
#include <vector>

// Qt includes
///
#include <qlist.h>
#include <qcolor.h>

// Local includes
///
#include "OutputPolicy.h"
#include "OutputBaseItem.h"

// BaseLib includes
///
#include "Object.hpp"

// Forward declaration
///
class QGraphicsEllipseItem;
class QGraphicsScene;

namespace DataModels {
namespace Output {

// Forward declaration
///
class VirtualItem;

/**
 * Wrap BaseLib::Object and QGraphicsItem in a single class, 
 * which is used in OutputModel. DataItem does not own stored Object, 
 * and so DataItem don't delete him.
 */
class DataItem : public BaseItem
{
public:
	enum {
		Identification = 1
	};
public:
	DataItem(BaseLib::Objects::Object& object, VirtualItem* parent);
public:
	/**
	 * Create graphic representation. Index to storage is used to identify 
	 * DataItem from mGraphics. 
	 * @param[in] scene
	 * @param[in] index to storage
	 * @param[out] new x position
	 * @param[out] new y position
	 */
	void CreateGraphic(QGraphicsScene* scene, int index, double& x, double& y);
	/**
	 * Set name of value that will be used to calculate graphics rep. position.
	 * All setting are passed in policy class.
	 * @param[in] policy
	 * @param[out] new x position
	 * @param[out] new y position
	 * @return false if some values were set to zero because of lack of data
	 */
	bool SetShowKey(const ::Output::Policy& policy, double& x, double& y);
	/**
	 * @return reference to object
	 */
	BaseLib::Objects::Object& GetObject();
	/**
	 * Set grpahics representation size.
	 * @param[in] size
	 */
	void SetSize(double size);
	/**
	 * @return index from mGraphics
	 */
	int GetIndex() const;
	/**
	 * Scale actula graphics rep. position.
	 */
	void ScalePosition(double x, double y);
	/**
	 * Return graphics representation position.
	 * @return actual position.
	 */ 
	QPointF GetPosition() const;
	/**
	 * Set graphics representation position.
	 * @param[in] position
	 */
	void SetPosition(const QPointF& pos);
	/**
	 * @param[in] key to data
	 * @return size of data under certain key, or -1 if key is invalid
	 */
	int GetDataSize(const std::string& key) const;
	/**
	 * @return true if graphics rep. is marked as selected
	 */
	bool IsSelected() const;
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
	virtual void SelectionChange(bool selected);
	virtual void Serialize(const std::string& key, std::ostream& stream);
	virtual void SetVisible(bool visibility);
	virtual bool GetVisible() const;
	virtual const QColor& GetColor() const;	
protected:
	/**
	 * Set name of value that will be used to calculate graphics rep. position.
	 * All aditional setting are passed in policy class.
	 * @param[in] data
	 * @param[in] policy ( ignore name value )
	 * @param[out] new x position
	 * @param[out] new y position
	 * @return false if index was out of range 
	 */
	bool SetShowKey(BaseLib::Objects::Variant* data, const ::Output::Policy& policy, double& x, double& y);
	/**
	 * Part of SetShowKey, resolve cases when data is vector.
	 * Do not set graphics rep position or anything else, just set x, y values.
	 * @param[in] data
	 * @param[in] policy
	 * @param[out] new x position
	 * @param[out] new y position
	 * @return false if index was out of range 
	 */
	template <typename TYPE> bool SetShowKeyVector(const std::vector<TYPE>& data, const ::Output::Policy& policy, double& x, double& y) const;
private:
	/**
	 * Assigned color.
	 */
	const QColor* mColor;
	/**
	 * Associated object.
	 */
	BaseLib::Objects::Object& mObject;
	/**
	 * Associated graphics representation.
	 * Object is not owned by DataItem, so DataItem don't delete him !
	 */
	QGraphicsEllipseItem* mGraphics;
	/**
	 * Original unscaled object position.
	 */
	std::pair<double, double> mPosition;
	/**
	 * Is graphics rep. visible ?
	 */
	bool mGraphicsVisible;
};

} }

#endif // eny_outputDataItem_h