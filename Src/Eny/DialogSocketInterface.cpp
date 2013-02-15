#include "stdafx.h"
// ...
#include "DialogSocketInterface.h"
// BaseLib
#include "RandomAcces.hpp"
#include "SequentialAcces.hpp"

using namespace BaseLib::Interfaces;

namespace Dialog {

SocketInterface::SocketInterface(const BaseLib::Sockets::Socket::DataType& data, const BaseLib::Sockets::Socket::PrimaryType& primary, int socketInterface, const std::string& socketName, QWidget* parent)
	: QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint), mModel(data, primary, this)
{
	ui.setupUi(this);
	// ..
	ui.tableView->setModel(&mModel);
	// we will set LabelInterface 
	QString text = tr("Interface: ");
	if ((socketInterface & RandomAcces::Identification) == RandomAcces::Identification)
	{
		text += "random ";
	}
	if ((socketInterface & SequentialAcces::Identification) == SequentialAcces::Identification)
	{
		text += "sequential ";
	}
	ui.LabelInterface->setText(text);

	// resize 
	resize(ui.tableView->horizontalHeader()->length() + 30, 
		ui.tableView->verticalHeader()->length() + 80);
}

}