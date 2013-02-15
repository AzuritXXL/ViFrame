#include "stdafx.h"
// ..
#include "AssemblyItemModule.h"
#include "AssemblyItemConnection.h"

namespace Assembly {

ItemConnection::ItemConnection(ItemModule* source, ItemModule* target, size_t socket, QGraphicsScene* scene)
	: QGraphicsItemGroup(0, scene), mSource(source), mTarget(target), mSockets(), mChildrens()
{
	mSockets.push_back(socket);

	// set z value
	setZValue(5.0);
	// set identification data
	setData(0, Identification);

	// create connection lines
	CreateConnection();
	UpdateConnection();
}

ItemConnection::~ItemConnection()
{
	// delete child items
	DeleteConnection();
	// remove us from scene
	if (scene() != nullptr)
	{
		scene()->removeItem(this);
	}
}

ItemModule* ItemConnection::GetSource() const
{
	return mSource;
}

ItemModule* ItemConnection::GetTarget() const
{
	return mTarget;
}

void ItemConnection::AddSocket(const size_t& socket)
{
	auto result = std::find( std::begin(mSockets), std::end(mSockets), socket);
	if (result == std::end(mSockets))
	{	
		mSockets.push_back(socket);
	}
	else
	{
		// we already have such value .. don't add it again		
	}
}

bool ItemConnection::RemoveSocket(const size_t& socket)
{
	auto result = std::find( std::begin(mSockets), std::end(mSockets), socket);
	if (result == std::end(mSockets))
	{	// we can't find connection ... 
	}
	else
	{	// we found connection
		std::swap(mSockets.back(), *result);
		mSockets.pop_back();

	}

	// if we have some other connection return true
	return mSockets.size() == 0;
}

void ItemConnection::UpdateConnection()
{
	// we known LeftTop1 < RightBottom1
	QPointF LeftTop1 = mSource->pos();
	QPointF RightBottom1(mSource->pos().x() + mSource->boundingRect().width(),
		mSource->pos().y() + mSource->boundingRect().height());

	// we known LeftTop2 < RightBottom2
	QPointF LeftTop2 = mTarget->pos();
	QPointF RightBottom2(mTarget->pos().x() + mTarget->boundingRect().width(),
		mTarget->pos().y() + mTarget->boundingRect().height());
		
	int sector = 0;
	// sectors ( x = 5 )
	// 1  2  3
	// 4  X  6
	// 7  8  9

	if ( RightBottom1.x() < LeftTop2.x() )
	{	// sectors 1,4,7
		sector = 1;
	}
	else if ( LeftTop1.x() > RightBottom2.x() )
	{	// sectors 3,6,9
		sector = 3;
	}
	else
	{	// sectors 2,5,8
		sector = 2;
	}

	if ( RightBottom1.y() < LeftTop2.y() )
	{	// sectors 1,2,3
	}
	else if ( LeftTop1.y() > RightBottom2.y() )
	{	// sectors 7,8,9
		sector += 6;
	}
	else
	{	// sectors 4,5,6
		sector += 3;
	}

	int x1,y1,x2,y2,x3,y3;
	switch(sector)
	{
	case 1:
		{
			x1 = LeftTop1.x() + mSource->boundingRect().width() / 2;
			y1 = LeftTop1.y() + mSource->boundingRect().height() / 2;
			x3 = LeftTop2.x();
			y3 = LeftTop2.y() + mTarget->boundingRect().height() / 2;
			x2 = x1;
			y2 = y3;

			mChildrens[0]->setLine(x1, y1, x2, y2);
			mChildrens[1]->setLine(x2, y2, x3, y3);
			mChildrens[2]->setLine(x3 - 5, y3 - 5, x3, y3);
			mChildrens[3]->setLine(x3 - 5, y3 + 5, x3, y3);
			break;
		}
	case 2:
		{
			if ( LeftTop1.x() > LeftTop2.x())
			{
				x2 = LeftTop1.x() - ( (LeftTop1.x() - RightBottom2.x()) / 2);
			}
			else
			{
				x2 = LeftTop2.x() - ( (LeftTop2.x() - RightBottom1.x()) / 2);
			}
				
			y2 = LeftTop1.y() + mSource->boundingRect().height() / 2;
			x3 = x2;
			y3 = LeftTop2.y();

			mChildrens[0]->setLine(x2, y2, x2, y2);
			mChildrens[1]->setLine(x2, y2, x3, y3);
			mChildrens[2]->setLine(x3 - 5, y3 - 5, x3, y3);
			mChildrens[3]->setLine(x3 + 5, y3 - 5, x3, y3);
			break;
		}
	case 3:
		{
			x1 = LeftTop1.x() + mSource->boundingRect().width() / 2;
			y1 = LeftTop1.y() + mSource->boundingRect().height() / 2;
			x3 = RightBottom2.x();
			y3 = LeftTop2.y() + mSource->boundingRect().height() / 2;
			x2 = x1;
			y2 = y3;

			mChildrens[0]->setLine(x1, y1, x2, y2);
			mChildrens[1]->setLine(x2, y2, x3, y3);
			mChildrens[2]->setLine(x3 + 5, y3 - 5, x3, y3);
			mChildrens[3]->setLine(x3 + 5, y3 + 5, x3, y3);
			break;
		}
	case 4:
		{
			x2 = RightBottom1.x();

			if ( LeftTop1.y() > LeftTop2.y())
			{
				y2 = LeftTop1.y() - ( (LeftTop1.y() - RightBottom2.y()) / 2);
			}
			else
			{
				y2 = LeftTop2.y() - ( (LeftTop2.y() - RightBottom1.y()) / 2);
			}

			x3 = LeftTop2.x();
			y3 = y2;

			mChildrens[0]->setLine(x2, y2, x2, y2);
			mChildrens[1]->setLine(x2, y2, x3, y3);
			mChildrens[2]->setLine(x3 - 5, y3 - 5, x3, y3);
			mChildrens[3]->setLine(x3 - 5, y3 + 5, x3, y3);
			break;
		}
	case 5:
		{
			// no draw .. they cover
			break;
		}
	case 6:
		{
			x2 = LeftTop1.x();

			if ( LeftTop1.y() > LeftTop2.y())
			{
				y2 = LeftTop1.y() - ( (LeftTop1.y() - RightBottom2.y()) / 2);
			}
			else
			{
				y2 = LeftTop2.y() - ( (LeftTop2.y() - RightBottom1.y()) / 2);
			}

			x3 = RightBottom2.x();
			y3 = y2;


			mChildrens[0]->setLine(x2, y2, x2, y2);
			mChildrens[1]->setLine(x2, y2, x3, y3);
			mChildrens[2]->setLine(x3 + 5, y3 - 5, x3, y3);
			mChildrens[3]->setLine(x3 + 5, y3 + 5, x3, y3);
			break;
		}
	case 7:
		{
			x1 = LeftTop1.x() + mSource->boundingRect().width() / 2;
			y1 = LeftTop1.y();
			x3 = LeftTop2.x();
			y3 = LeftTop2.y() + mTarget->boundingRect().height() / 2;
			x2 = x1;
			y2 = y3;

			mChildrens[0]->setLine(x1, y1, x2, y2);
			mChildrens[1]->setLine(x2, y2, x3, y3);
			mChildrens[2]->setLine(x3 - 5, y3 - 5, x3, y3);
			mChildrens[3]->setLine(x3 - 5, y3 + 5, x3, y3);
			break;
		}
	case 8:
		{
			if ( LeftTop1.x() > LeftTop2.x())
			{
				x2 = LeftTop1.x() - ( (LeftTop1.x() - RightBottom2.x()) / 2);
			}
			else
			{
				x2 = LeftTop2.x() - ( (LeftTop2.x() - RightBottom1.x()) / 2);
			}
				
			y2 = LeftTop1.y();
			x3 = x2;
			y3 = RightBottom2.y();

			mChildrens[0]->setLine(x2, y2, x2, y2);
			mChildrens[1]->setLine(x2, y2, x3, y3);
			mChildrens[2]->setLine(x3 - 5, y3 + 5, x3, y3);
			mChildrens[3]->setLine(x3 + 5, y3 + 5, x3, y3);
			break;
		}
	case 9:
		{
			x1 = LeftTop1.x() + mSource->boundingRect().width() / 2;
			y1 = LeftTop1.y();
			x3 = RightBottom2.x();
			y3 = LeftTop2.y() + mTarget->boundingRect().height() / 2;
			x2 = x1;
			y2 = y3;

			mChildrens[0]->setLine(x1, y1, x2, y2);
			mChildrens[1]->setLine(x2, y2, x3, y3);
			mChildrens[2]->setLine(x3 + 5, y3 - 5, x3, y3);
			mChildrens[3]->setLine(x3 + 5, y3 + 5, x3, y3);
			break;
		}
	}
}

const std::vector<size_t>& ItemConnection::GetSockets() const
{
	return mSockets;
}

void ItemConnection::CreateConnection()
{	// create lines to draw connections
	QPen pen(Qt::red, 2, Qt::SolidLine);
	QGraphicsLineItem* line;
	for (int i = 0; i < 4 ; ++i)
	{
		line = new QGraphicsLineItem(this, scene());
		line->setPen(pen);
		line->setZValue(1.0);
		mChildrens.push_back(line);
	}
}

void ItemConnection::DeleteConnection()
{	// delete subitems
	std::for_each(std::begin(mChildrens), std::end(mChildrens), 
		[&] (QGraphicsItem* item) 
		{
			// remove from scene, group and then delete
			removeFromGroup(item);
			if (scene() != nullptr)
			{
				scene()->removeItem(item);
			}
			delete item;
			item = 0;
		}
	);
	mChildrens.clear();
}

}