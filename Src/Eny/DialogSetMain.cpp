#include "stdafx.h"
// ...
#include "DialogSetMain.h"


namespace Dialog {

SetMain::SetMain(QList<QString>& attNames, QWidget* parent)
	: QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
{
	ui.setupUi(this);
	// add items
	ui.ComboAttributeName->addItems(attNames);
}

const Output::Policy& SetMain::GetPolicy() const
{
	return mPolicy;
}

//											Slots												//

void SetMain::slAccept()
{	// store values from dialog to mPolicy
	mPolicy.Name = ui.ComboAttributeName->currentText().toStdString();
	mPolicy.FirstColumn = ui.SpinFirst->value();
	mPolicy.SecondColumn = ui.SpinSecond->value();
	mPolicy.UseFirstAviable = ui.CheckUseFirst->isChecked();
	mPolicy.UseSecondAviable = ui.CheckUseSecond->isChecked();
	// on the end call accept
	accept();
}

}