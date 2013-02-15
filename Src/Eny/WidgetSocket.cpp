#include "stdafx.h"
// ..
#include "WidgetSocket.h"
#include "DialogSocketInterface.h"

namespace Widget {

Socket::Socket(const BaseLib::Sockets::Socket& socket, int number, QWidget* parent)
	: QWidget(parent), mSocket(socket)
{
	ui.setupUi(this);
	// setup values
	ui.LabelNumber->setText(QString::number(number));
	ui.LabelName->setText(mSocket.GetName().c_str());
	QString strDescription = mSocket.GetDescription().c_str();
	if (strDescription.isEmpty())
	{	// no description
		ui.label_description->setVisible(false);
		ui.TextDescription->setVisible(false);
	}
	else
	{
		ui.TextDescription->setText(strDescription);
	}
	// connect signal
	connect(ui.BtnDetail, SIGNAL(clicked()), this, SLOT(slShowSocketDetail()));
}

//											Slots												//

void Socket::slShowSocketDetail()
{	// show dialog with socket interface
	Dialog::SocketInterface dlg(mSocket.GetData(), mSocket.GetPrimary(), mSocket.GetInterface(), mSocket.GetName(), this);
	dlg.exec();
}

}