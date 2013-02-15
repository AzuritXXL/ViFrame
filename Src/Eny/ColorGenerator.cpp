#include "stdafx.h"
// ...
#include "ColorGenerator.h"

namespace Utility {

ColorGenerator::ColorGenerator(eType type, int count)
	: mType(type), mCount(count)
{ }

QColor ColorGenerator::GenerateColor(int index)
{
	switch(mType)
	{
	case eType::FluentChange:
		return FluentGenerator(index);
	default:
	case eType::Base:
		return BaseGenerator(index);
	}
}

QColor ColorGenerator::BaseGenerator(int index)
{
	index = index % 14;
	switch(index)
	{
	case 0:
		return QColor(254,  0,  0);
	case 1:
		return QColor(  0,  0,254);
	case 2:
		return QColor(  0,254,254);
	case 3:
		return QColor(254,254,  0);
	case 4:
		return QColor(254,  0,254);
	case 5:
		return QColor(  0,254,254);
	case 6:
		return QColor(254,127,  0);
	case 7:
		return QColor(254,  0,127);
	case 8:
		return QColor(  0,254,127);
	case 9:
		return QColor(  0,127,  0);
	case 10:
		return QColor(127,127,  0);
	case 11:
		return QColor(127,  0,127);
	case 12:
		return QColor(  0,127,127);
	case 13:
		return QColor(254,254,254);
	}
}

QColor ColorGenerator::FluentGenerator(int index)
{
	// if index == 0 -> change 1
	// if index >= mCount -> change 0

	double change = (double)(mCount - index) / (double)mCount;

	QColor begin = Qt::red;
	QColor end = Qt::blue;

	int ra = (change * begin.red());

	int red = (change * begin.red()) + ((1 - change) * end.red());
	int green = (change * begin.green()) + ((1 - change) * end.green());
	int blue = (change * begin.blue()) + ((1 - change) * end.blue());

	return QColor(red, green, blue);
}

}