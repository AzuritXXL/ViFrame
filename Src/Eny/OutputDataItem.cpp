#include "stdafx.h"
// Std
#include <assert.h>
// Qt
#include <qgraphicsitem.h>
#include <qgraphicsscene.h>
#include <qgraphicsrectitem>
#include <QGraphicsEllipseItem>
// ..
#include "OutputDataItem.h"
#include "OutputVirtualItem.h"
// BaseLib
#include "Vector.hpp"
#include "Variable.hpp"

namespace DataModels {
namespace Output {

DataItem::DataItem(BaseLib::Objects::Object& object, VirtualItem* parent)
	: BaseItem(parent), mObject(object), mGraphics(nullptr), mGraphicsVisible(true)
{ }

void DataItem::CreateGraphic(QGraphicsScene* scene, int index, double& x, double& y)
{
	// mGraphics = new QGraphicsRectItem(0, 0, 1, 1, nullptr, scene);
	mGraphics = new QGraphicsEllipseItem(0, 0, 1, 1, nullptr, scene);	

	mGraphics->setBrush(QBrush(Qt::black));
	mGraphics->setToolTip(mObject.GetName().c_str());
	// set flags
	mGraphics->setFlag(QGraphicsItem::ItemIsSelectable, true);
	// set identification index
	mGraphics->setData(0, index);
	// and we try to set show key grom mObject main key ( we use "default" policy )	
	SetShowKey(mObject.GetMain(), ::Output::Policy(), x, y);
}

bool DataItem::SetShowKey(const ::Output::Policy& policy, double& x, double& y)
{
	if (mObject.GetData().count(policy.Name) == 0)
	{	// no such data
		x = y = 0;
		// so we ignore them and set to zero .. 
		mGraphics->setPos(0, 0);
		return false;
	}
	else
	{	// we have data .. so set
		return SetShowKey(mObject.GetData().at(policy.Name), policy, x, y);
	}
}

BaseLib::Objects::Object& DataItem::GetObject()
{	
	return mObject;
}

void DataItem::SetSize(double size)
{
	mGraphics->setRect(0, 0, size, size);
}

int DataItem::GetIndex() const
{	// should be called only after GraphicsRep was created
	assert(mGraphics != nullptr);
	if (mGraphics == nullptr)
	{
		return -1;
	}
	return mGraphics->data(0).toInt();
}

void DataItem::ScalePosition(double x, double y)
{
	mGraphics->setPos(mPosition.first * x, mPosition.second * y);
}

QPointF DataItem::GetPosition() const
{
	return mGraphics->pos();
}

void DataItem::SetPosition(const QPointF& pos)
{
	mGraphics->setPos(pos);
	mPosition.first = pos.x();
	mPosition.second = pos.y();
}

int DataItem::GetDataSize(const std::string& key) const
{
	if (mObject.GetData().at(key) == nullptr)
	{	// no such data
		return -1;
	}
	else
	{
		BaseLib::Objects::Variant* dataVariant = mObject.GetData().at(key);
		switch (dataVariant->GetType())
		{
		case BaseLib::Objects::Variant::Int:
		case BaseLib::Objects::Variant::Double:		
		case BaseLib::Objects::Variant::String:
			// all has size 1
			return 1;
		case BaseLib::Objects::Variant::VectorInt:
			return (dynamic_cast<BaseLib::Objects::VInt*>(dataVariant))->Data.size();
		case BaseLib::Objects::Variant::VectorDouble:
			return (dynamic_cast<BaseLib::Objects::VDouble*>(dataVariant))->Data.size();
		}
	}
	// unknown data type
	return -1;
}

bool DataItem::IsSelected() const
{
	return mGraphics->isSelected();
}

bool DataItem::SetShowKey(BaseLib::Objects::Variant* data, const ::Output::Policy& policy, double& x, double& y)
{
	bool result = true;
	// there is data .. so use another params from policy, also there are few cases which can spare us work .. 
	switch(data->GetType())
	{
	case BaseLib::Objects::Variant::Int:
		{	// simple int, there is no place to work with indexes .. 
			x = dynamic_cast<BaseLib::Objects::Int*>(data)->Data;
			y = 0;			
			break;
		}
	case BaseLib::Objects::Variant::Double:
		{	// same as before no place for index games .. 
			x = dynamic_cast<BaseLib::Objects::Double*>(data)->Data;
			y = 0;
			break;
		}
	case BaseLib::Objects::Variant::VectorInt:		// vector are solved in another function
		result = SetShowKeyVector(dynamic_cast<BaseLib::Objects::VInt*>(data)->Data, policy, x, y);
		break;
	case BaseLib::Objects::Variant::VectorDouble:	// vector are solved in another function
		result = SetShowKeyVector(dynamic_cast<BaseLib::Objects::VDouble*>(data)->Data, policy, x, y);
		break;
	}
	
	// set position, and store values
	mGraphics->setPos(x, y);
	mPosition.first = x;
	mPosition.second = y;

	return result;
}

template <typename TYPE> bool DataItem::SetShowKeyVector(const std::vector<TYPE>& data, const ::Output::Policy& policy, double& x, double& y) const
{
	bool result = true;
	// we need to find out indexes that we use
	if (policy.UseFirstAviable)
	{
		if (policy.UseSecondAviable)
		{	// we have to find index for both first and second data ...
			switch(data.size())
			{
			case 0:	// no data .. use zeros
				x = y = 0;
				result = false;
				break;
			case 1:	// data only for the first one
				x = (double)data[0];
				result = false;
				y = 0;
				break;
			default: // data for both ..
				x = (double)data[0];				
				y = (double)data[1];
				break;
			}
			return true;
		}
		else
		{	// we look for index only for first value
			if (data.size() == 0)
			{	// no data 
				x = 0;
				y = 0;
				return false; // and we end here
			}
			else
			{	// we have data for the first
				x = (double)data[0];
			}

			if (policy.SecondColumn == -1)
			{	// second value is zero
				y = 0;
			}
			else if (policy.SecondColumn < data.size())
			{	// we can use second index ...
				y = (double)data[policy.SecondColumn];
			}
			else
			{	// second index is out of range ...
				y = 0;
				result = false;
			}
		}
	}
	else
	{
		if (policy.UseSecondAviable)
		{	// first index is set .. we will search second one that has to be greater then the first one
			if (policy.FirstColumn == -1)
			{	// first is zero .. 
				x = 0;
				if (data.size() == 0)
				{	// no data for second
					y = 0;
					result = false;
				}
				else
				{	// we use first valid .. 
					y = (double)data[0];
				}
			}
			else if (policy.FirstColumn >= data.size())
			{	// index is out of range .. -> both zero 
				x = y = 0;
				result = false;
			}
			else
			{	// index is valid .. 
				x = data[policy.FirstColumn];
				if (policy.FirstColumn + 1 < data.size())
				{	// and there is also free index for second value
					y = (double)data[policy.FirstColumn + 1];
				}
				else
				{	// no one left for second one .. so give him zero value
					y = 0;
					result = false;
				}
			}
		}
		else
		{	// we have all index set, we dont look for anything
			if (policy.FirstColumn == -1)
			{	// second value is zero
				x = 0;
			}
			else if (policy.FirstColumn < data.size())
			{	// we can use first index ...
				x = (double)data[policy.FirstColumn];
			}
			else
			{	// first index is out of range ...
				x = 0;
				result = false;
			}
			// set second
			if (policy.SecondColumn == -1)
			{	// second value is zero
				y = 0;
			}
			else if (policy.SecondColumn < data.size())
			{	// we can use second index ...
				y = (double)data[policy.SecondColumn];
			}
			else
			{	// second index is out of range ...
				y = 0;
				result = false;
			}
		}
	}

	// position will be set in our caller
	return result;
}

//											BaseItem											//

void DataItem::AppendChild(BaseItem *child)
{
	assert(false);
}

BaseItem* DataItem::GetChild(int row)
{
	assert(false);
	return nullptr;
}

int DataItem::GetChildCount() const
{
	return 0;
}

int DataItem::GetColumnCount() const
{
	return 1;
}

int DataItem::GetRow() const
{
    if (mParent != nullptr)
	{
		return mParent->GetObjectRow(const_cast<DataItem*>(this));
	}
    return 0;
}

QVariant DataItem::GetData(int role) const
{
	switch(role)
	{
	case Qt::DisplayRole: // The key data to be rendered in the form of text. (QString)
		return mObject.GetName().c_str();
	case Qt::ForegroundRole: // The foreground brush (text color, typically) used for items rendered with the default delegate. (QBrush)
		return *mColor;
	default:
		break;
	}
	// return empty variant
	return QVariant();
}

void DataItem::SetColor(const QColor* color)
{
	mColor = color;
	if (mGraphics != nullptr)
	{
		mGraphics->setPen(QPen(*color));
		mGraphics->setBrush(QBrush(*color));
	}
}

int DataItem::GetType() const
{
	return Identification;
}

void DataItem::SelectionChange(bool selected)
{
	assert(mGraphics != nullptr);
	mGraphics->setSelected(selected);
}

void DataItem::Serialize(const std::string& key, std::ostream& stream)
{

	if (mObject.GetData().count(key) == 0)
	{	// no data -> ignore
	}
	else
	{	// we have data
		if (mObject.GetName().empty() )
		{	// empty name use some text .. 
			stream << "no-name" << "\t";
		}
		else
		{	// use object name
			stream << mObject.GetName() << "\t";
		}
		// data
		stream << mObject.GetData().at(key)->ToString();
				
		stream << std::endl;
	}
}

void DataItem::SetVisible(bool visibility)
{
	mGraphicsVisible = visibility;
	mGraphics->setVisible(visibility);
}

bool DataItem::GetVisible() const
{
	return mGraphicsVisible;
}

const QColor& DataItem::GetColor() const
{
	return *mColor;
}

} }