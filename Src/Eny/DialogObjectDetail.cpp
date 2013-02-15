#include "stdafx.h"
// ...
#include "DialogObjectDetail.h"

namespace Dialog {

ObjectDetail::ObjectDetail(BaseLib::Objects::Object& object, QWidget* parent)
	: QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint), mModel(object, this)
{
	ui.setupUi(this);

	// set model
	ui.TableData->setModel(&mModel);

	// resize 
	ui.TableData->resizeColumnsToContents();
	int x = ui.TableData->horizontalHeader()->length() + 30;
	int y = ui.TableData->verticalHeader()->length() + 80;
	// size limitation
	x = x < 800 ? x : 800;
	y = y < 600 ? y : 600;
	resize(x, y);
}

}
