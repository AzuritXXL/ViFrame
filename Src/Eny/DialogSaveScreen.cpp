#include "stdafx.h"
// Qt
#include <QFontDialog>
// ..
#include "DialogSaveScreen.h"

namespace Dialog {

SaveScreen::SaveScreen(QWidget* parent, bool forceUseRoot)
	: QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
{ 
	ui.setupUi(this);

	if (forceUseRoot)
	{	// we are forced to use root groups 
		ui.CheckRootGroups->setChecked(true);
		ui.CheckRootGroups->setEnabled(false);
	}
}
	 
int SaveScreen::GetWidth()
{
	return ui.SpinWidth->value();
}

int SaveScreen::GetHeight()
{
	return ui.SpinHeight->value();
}

SaveScreen::eLegend SaveScreen::GetLegend()
{
	if (ui.RadioLB->isChecked())
	{
		return LeftBottom;
	}
	else if (ui.RadioLT->isChecked())
	{
		return LeftTop;
	}
	else if (ui.RadioRB->isChecked())
	{
		return RightBottom;
	}
	else if (ui.RadioRT->isChecked())
	{
		return RightTop;
	}
	else
	{	// none
		return None;
	}
}

const QFont& SaveScreen::GetFont()
{
	return mFont;
}

bool SaveScreen::GetUseRoot()
{
	return ui.CheckRootGroups->isChecked();
}

//											Slots												//

void SaveScreen::slSetFont()
{
	bool ok;
	QFont font = QFontDialog::getFont(&ok, this);
	if (ok) 
	{	// dialog accepr
		mFont = font;
	}
}

}
