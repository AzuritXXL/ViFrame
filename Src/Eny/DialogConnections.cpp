#include "stdafx.h"
// ...
#include "DialogConnections.h"
#include "DialogSocketInterface.h"
#include "WidgetConnection.h"

namespace Dialog {

Connections::Connections(Assembly::ItemModule& source, Assembly::ItemModule& target, QWidget* parent)
	: QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint), mSource(source), mTarget(target)
{
	ui.setupUi(this);
	InitGui();

	// resize ... ( only height )
	int height = ui.LayoutConnections->sizeHint().height() + 80;
	height = height > maximumHeight() ? maximumHeight() : height; // size limitation
	resize(size().width(), height);
}

void Connections::InitGui()
{
	// labels
	ui.LabelSource->setText(tr("Source module: ") + mSource.GetHolder().GetId().c_str());
	ui.LabelTarget->setText(tr("Target module: ") + mTarget.GetHolder().GetId().c_str());
	// sockets 
	size_t iEnd = mTarget.GetModule()->GetInputSockets().size();
	for (size_t i = 0; i < iEnd; ++i)
	{
		ui.LayoutConnections->addWidget(new Widget::Connection(mSource, mTarget, i, this));
	}
	// add spacer
	ui.LayoutConnections->addItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));
	// slots
	connect(ui.BtnInterface, SIGNAL(clicked()), this, SLOT(slShowInterface()));
}

//											Slots												//

void Connections::slShowInterface()
{
	auto socket = mSource.GetModule()->GetOutputSocket();
	Dialog::SocketInterface dlg(socket.GetData(), socket.GetPrimary(), socket.GetInterface(), socket.GetName(), this);
	dlg.exec();
}


}
