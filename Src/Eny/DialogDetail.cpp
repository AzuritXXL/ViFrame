#include "stdafx.h"
// ..
#include "Library.h"
#include "DialogDetail.h"
#include "WidgetSocket.h"

namespace Dialog {

Detail::Detail(Logic::ModuleHolder& module, QWidget* parent)
	: QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint), mModule(module)
{
	ui.setupUi(this);
	// ...
	InitDetail();
	InitInput();
}

void Detail::InitDetail()
{	
	// setup detail
	auto version(mModule.GetLibrary()->GetDescription().Version);
	QString versionTxt = QString::number(version.first) + '.' + QString::number(version.second);
	ui.LineVersion->setText(versionTxt);
	ui.TextDetail->setHtml(mModule.GetLibrary()->GetDescription().LongDescription.c_str());	
	// output socket
	ui.LayoutDetail->addWidget(new Widget::Socket(mModule.GetModule()->GetOutputSocket(), 0, this), 
		ui.LayoutDetail->rowCount(), 0, 1, -1);
}

void Detail::InitInput()
{
	auto iter = mModule.GetModule()->GetInputSockets().cbegin();
	auto iterEnd = mModule.GetModule()->GetInputSockets().cend();

	// add representation for each widget
	for (int i = 0; iter != iterEnd; ++i, ++iter)
	{
		ui.LayoutInput->addWidget(new Widget::Socket(*(*iter), i, this));
	}
	// add spacer
	ui.LayoutInput->addItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));
}

}