#include "stdafx.h"
// ..
#include "DialogSocketInterface.h"
#include "WidgetConnection.h"
#include "AssemblyItemConnection.h"
// BaseLib
#include "Socket.hpp"
// Qt
#include <qmessagebox.h>

namespace Widget {

Connection::Connection(Assembly::ItemModule& source, Assembly::ItemModule& target, size_t socket, QWidget* parent)
	: QWidget(parent), mSource(source), mTarget(target), mSocket(socket)
{
	ui.setupUi(this);
	
	// module can be connected if specification match and if is complete
	mConnectable = BaseLib::Sockets::Socket::Match(
		mSource.GetModule()->GetOutputSocket(), 
		*mTarget.GetModule()->GetInputSockets()[socket]) && source.GetModule()->PrepareForConnection();

	InitGui();
}

void Connection::InitGui()
{
	if (mTarget.GetModule()->GetSocket(mSocket) == nullptr)
	{	// no connection
		if (mConnectable)
		{
			ui.BtnAction->setText(tr("Connect"));
		}
		else
		{	// no connection and we cant connect source
			ui.BtnAction->setText(tr("..."));
			ui.BtnAction->setEnabled(false);
		}
	}
	else
	{	// connection
		ui.BtnAction->setText(tr("Disconnect"));
		ui.LineConnected->setText(mTarget.GetModule()->GetSocket(mSocket)->GetId().c_str());
	}
	// socket name
	ui.LabelName->setText(mTarget.GetModule()->GetInputSockets()[mSocket]->GetName().c_str());
	// slots & signals
	connect(ui.BtnInterface, SIGNAL(clicked()), this, SLOT(slInterface()));
	connect(ui.BtnAction, SIGNAL(clicked()), this, SLOT(slAction()));
	// description
	std::string desc = mTarget.GetModule()->GetInputSockets()[mSocket]->GetDescription();
	if (desc.empty())
	{	// no description		
	}
	else
	{	// has description
		auto font = ui.LabelName->font();
		font.setUnderline(true);
		ui.LabelName->setFont(font);
		ui.LabelName->setToolTip(desc.c_str());
	}

}

//											Slots												//

void Connection::slInterface()
{
	auto socket = mTarget.GetModule()->GetInputSockets()[mSocket];
	Dialog::SocketInterface dlg(socket->GetData(), socket->GetPrimary(), socket->GetInterface(), socket->GetName(), this);
	dlg.exec();
}

void Connection::slAction()
{
	if (mTarget.GetModule()->GetSocket(mSocket) == nullptr)
	{	// we will connect us as our source

		// but first check cycle
		if (mSource.GetModule()->FindConnectionFromModule( mTarget.GetModule() ))
		{	// cycle
			QMessageBox::critical(this, "Cycle detected", "Cycle detected, connection will not be established.", QMessageBox::Ok, QMessageBox::Ok);
			return;
		}

		if (mTarget.GetModule()->Connect(mSource.GetModule(), mSocket))
		{	// connection succesfull
			if (mTarget.GetModule()->GetSocket(mSocket) == nullptr)
			{	// module say that connection was ok .. but it is not .. he cheat us .. 
//. Write to creator of module something ... ?
				return;
			}

			ui.BtnAction->setText(tr("Disconnect"));
			ui.LineConnected->setText(mTarget.GetModule()->GetSocket(mSocket)->GetId().c_str());
			// add graphic representation ( use scene from mSource )
			Assembly::ItemConnection* connection = nullptr;
			connection = mSource.FindConnection(&mTarget);
			if (connection == nullptr)
			{	// first conneciton between moduls
				connection = new Assembly::ItemConnection(&mSource, &mTarget, mSocket, mSource.scene());
				mSource.AddConnection(connection);
				mTarget.AddConnection(connection);
			}
			else
			{	// just add socket
				connection->AddSocket(mSocket);
			}			
		}
		else
		{	// connection failed
			QMessageBox::critical(this, "Error", "Connection failed.", QMessageBox::NoButton);
		}
	}
	else
	{	// we will disconnect 
		auto connectedOne = mTarget.GetModule()->GetSocket(mSocket);

		if (mTarget.GetModule()->Connect(nullptr, mSocket))
		{	// disconnecting complete
			ui.LineConnected->setText(tr("")); // set label of connected module to ''
			// remove graphic representation			
			mTarget.RemoveConnection(connectedOne, mSocket);

			if (mConnectable)
			{
				ui.BtnAction->setText(tr("Connect"));
			}
			else
			{	// no connection and we cant connect source	
				ui.BtnAction->setText(tr("..."));
				ui.BtnAction->setEnabled(false);
			}
		}
		else
		{	// disconnecting failed
			QMessageBox::critical(this, tr("Error"), tr("Disconnecting failed."), QMessageBox::NoButton);
		}
	}
}

}