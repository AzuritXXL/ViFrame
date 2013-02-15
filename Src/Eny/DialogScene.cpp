#include "stdafx.h"
// ...
#include "DialogScene.h"

namespace Dialog {

Scene::Scene(double sizeX, double sizeY, double maxX, double maxY, QWidget* parent)
	: QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint), mMaxX(maxX), mMaxY(maxY), mAbsolute(true)
{
	ui.setupUi(this);

	// start with absolute size
	InitGui(sizeX, sizeY, maxX, maxY);
}

double Scene::GetX() const
{
	if (mAbsolute)
	{
		return ui.SpinBoxWeight->value();
	}
	else
	{
		return ui.SpinBoxWeight->value() * mMaxX;
	}
}

double Scene::GetY() const
{
	if (mAbsolute)
	{
		return ui.SpinBoxHeight->value();
	}
	else
	{
		return ui.SpinBoxHeight->value() * mMaxY;
	}
}

void Scene::InitGui(double sizeX, double sizeY, double maxX, double maxY)
{	
	// max values
	ui.SpinBoxWeight->setMaximum(100000);
	ui.SpinBoxHeight->setMaximum(100000);
	// set spin value ... 
	ui.SpinBoxWeight->setValue(sizeX);
	ui.SpinBoxHeight->setValue(sizeY);
	// label text
	ui.LabelSize->setText(QString::number(maxX, 'f', 0) + " x " + QString::number(maxY, 'f', 0));
	// for zero values disable
	if (sizeX == 0)
	{
		ui.SpinBoxWeight->setEnabled(false);
		ui.SpinBoxWeight->setValue(0);
	}
	if (sizeY == 0)
	{
		ui.SpinBoxHeight->setEnabled(false);
		ui.SpinBoxHeight->setValue(0);
	}
}

void Scene::SetAbsolutScale()
{	// call when changing from relative
	// we need to store set values and transform them .. 

	if (mMaxX != 0)
	{
		double x = ui.SpinBoxWeight->value() * mMaxX;
		ui.SpinBoxWeight->setValue(x);
		ui.SpinBoxWeight->setSingleStep(1.0);
	}

	if (mMaxY != 0)
	{
		double y = ui.SpinBoxHeight->value() * mMaxY;
		ui.SpinBoxHeight->setValue(y);
		ui.SpinBoxHeight->setSingleStep(1.0);
	}

	mAbsolute = true;
}

void Scene::SetRelativeScale()
{	// call when changing from absolute scale

	if (mMaxX != 0)
	{
		double x = ui.SpinBoxWeight->value() / mMaxX;
		ui.SpinBoxWeight->setValue(x);
		ui.SpinBoxWeight->setSingleStep(0.1);
	}

	if (mMaxY != 0)
	{
		double y = ui.SpinBoxHeight->value() / mMaxY;
		ui.SpinBoxHeight->setValue(y);
		ui.SpinBoxHeight->setSingleStep(0.1);
	}

	mAbsolute = false;
}

//											Slots												//

void Scene::slRadioChange()
{
	if (ui.RadioAbsolut->isChecked())
	{
		SetAbsolutScale();
	}
	else
	{
		SetRelativeScale();
	}
}

}
