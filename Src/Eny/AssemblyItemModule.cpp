#include "stdafx.h"
// ..
#include "AssemblyItemModule.h"
#include "AssemblyItemConnection.h"

namespace Assembly {

ItemModule::ItemModule(Logic::ModuleHolder* module, DataModels::Message::Model& messageModel, QGraphicsScene* scene)
	: QObject(0), QGraphicsPixmapItem(0, scene), mModule(module), mMessageModel(messageModel), mPixmapValid(), mPixmapInvalid(), mConnections(),
		mConnectionDeleted(false)
{	// create graphics representation
	CreateGraphics();
	// call to set pixmap rep
	slInterfaceChanged();
	// mark us as target for report
	mModule->GetModule()->SetReportable(dynamic_cast<BaseLib::iReportable*>(this));
	// set identification data
	setData(0, Identification);
	// set z value
	setZValue(10.0);
}

ItemModule::~ItemModule()
{	// graphics content is relase in SceneDestroy 
	//	-> specialy destory graphic representation of connection
	OnSceneDestroy();
	// release all logic connection
	mModule->GetModule()->CutOff();
	// next we release logic represented by ModuleHolder ( unload module )
	mModule->Release();
}

bool ItemModule::TestConnectionDeleted()
{
	bool result = mConnectionDeleted;
	mConnectionDeleted = false;
	return result;
}

QString ItemModule::GetId() const
{
	return mModule->GetId().c_str();
}

BaseLib::iModule* ItemModule::GetModule() const
{
	return mModule->GetModule();
}

Logic::ModuleHolder& ItemModule::GetHolder() const
{
	return *mModule;
}

void ItemModule::OnPositionChanged()
{
	// report update to all our connections
	std::for_each(std::begin(mConnections), std::end(mConnections), 
		[] (ItemConnection* item)
		{
			item->UpdateConnection();
		}
	);
}

void ItemModule::AddConnection(ItemConnection* connection)
{
	mConnections.push_back(connection);
}

void ItemModule::RemoveConnection(ItemModule* otherSide, size_t socket)
{	// first we have to find connection
	const ConnectionsType::iterator connectionIter = 
		std::find_if(std::begin(mConnections), std::end(mConnections), 
		[&] (ItemConnection* item)->bool { return otherSide == item->GetSource() || otherSide == item->GetTarget(); } );
	if (connectionIter == std::end(mConnections))
	{	// we have not found connection
		return;
//. report disconneting of non existing connection ?
	}
	// remove socket from connection and delete it if neccesary
	InnerRemoveConnection((*connectionIter), socket);
}

void ItemModule::RemoveConnection(BaseLib::iModule* otherSide, size_t socket)
{	// first we have to find connection ( according to module )
	const ConnectionsType::iterator connectionIter = 
		std::find_if(std::begin(mConnections), std::end(mConnections), 
		[&] (ItemConnection* item)->bool { return otherSide == item->GetSource()->GetModule() || otherSide == item->GetTarget()->GetModule(); } );
	if (connectionIter == std::end(mConnections))
	{	// we have not found connection
		return;
//. report disconneting of non existing connection ?
	}
	// remove socket from connection and delete it if neccesary
	InnerRemoveConnection((*connectionIter), socket);
}

void ItemModule::RepairConnection(std::map<std::string, ItemModule*>& modules)
{	// we remove all existing connections, where we are target
	ItemConnection* connection;
	for (size_t i = 0; i < mConnections.size(); ++i)
	{
		if (mConnections[i]->GetTarget() == this)
		{	// release this connection
			connection = mConnections[i];
			// remove connection from sotrage of ItemModule
			std::remove(std::begin(connection->GetSource()->mConnections), std::end(connection->GetSource()->mConnections), connection);
			std::remove(std::begin(connection->GetTarget()->mConnections), std::end(connection->GetTarget()->mConnections), connection);
			// delete connection
			scene()->removeItem(connection);
			delete connection;
		}
	}
	// we add new connections where we are as a target
	BaseLib::iModule* moduleSource;
	BaseLib::iModule* moduleTarget = mModule->GetModule();
	size_t iMax = moduleTarget->GetInputSockets().size();
	// we go through socket and look if someone is connected
	for (size_t s = 0; s < iMax; ++s)
	{		
		moduleSource = moduleTarget->GetSocket(s);
		if (moduleSource == nullptr)
		{	// no connection
		}
		else
		{	// connection, we have to foun oposite side in modules
			
			if (modules.count(moduleSource->GetId()) == 0)
			{	// we have no prepresentation for this module, so no connetion
//. Report no representation ?
			}
			else
			{
				connection = new ItemConnection(modules[moduleSource->GetId()], this, s, scene());
				// add to storage of ItemModule
				mConnections.push_back(connection);
				modules[moduleSource->GetId()]->mConnections.push_back(connection);
			}
		}
	}
}

void ItemModule::OnSceneDestroy()
{	// release all graphics connection, so we start by going through all our connection
	// delete graphics representation of connections
	ConnectionsType::iterator iter = std::begin(mConnections);
	ConnectionsType::iterator iterEnd = std::end(mConnections);
	for (; iter != iterEnd; ++iter)
	{	// remove connection from other side of connection
		// we dont remove on our side, that would broke iterators
		ConnectionsType* connections = nullptr;
		if ( (*iter)->GetSource() == this)
		{	// we are source
			connections = &(*iter)->GetTarget()->mConnections;
		}
		else
		{	// we are target
			connections = &(*iter)->GetSource()->mConnections;
		}

		// once we have set connection removing is the same from source as from target
		auto newEnd = std::remove(std::begin(*connections), std::end(*connections), (*iter));
		connections->erase(newEnd, std::end(*connections));

		// remove from scene and delete
		if (scene() != nullptr)
		{
			scene()->removeItem((*iter));
		}

		delete (*iter);
		(*iter) = nullptr;
	}
	// clear mConnections
	mConnections.clear();
	// on the end we remove our self from scene ( if there exist scene in which we are )
	if (scene() != nullptr)
	{
		scene()->removeItem(this);
	}
}

ItemConnection* ItemModule::FindConnection(ItemModule* target)
{
	const ConnectionsType::iterator connectionIter = 
		std::find_if(std::begin(mConnections), std::end(mConnections), 
		[&] (ItemConnection* item)->bool { return target == item->GetTarget(); } );
	if (connectionIter == std::end(mConnections))
	{	// we have not found connection
		return nullptr;
	}
	else
	{	// we have one 
		return *connectionIter;
	}
}

void ItemModule::CreateGraphics()
{
	QFont font;
	font.setStyleStrategy(QFont::ForceOutline);
	// create pixmap
	QString text = mModule->GetId().c_str();
	// calculate text size
	QFontMetrics metric(font);
    QSize size = metric.size(Qt::TextSingleLine, text);

	// create image
    QImage image(size.width() + 12, size.height() + 12, QImage::Format_ARGB32_Premultiplied);
    
	// valid
	image.fill(qRgba(0, 0, 0, 0));
	QLinearGradient gradient(0, 0, 0, image.height()-1);
    gradient.setColorAt(0.0, Qt::white);
    gradient.setColorAt(0.2, QColor(200, 200, 255));
    gradient.setColorAt(0.8, QColor(200, 200, 255));
    gradient.setColorAt(1.0, QColor(127, 127, 200));
    QPainter painter;
    painter.begin(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(gradient);
    painter.drawRoundedRect(QRectF(0.5, 0.5, image.width() - 1, image.height() - 1), 25, 25, Qt::RelativeSize);
    painter.setFont(font);
    painter.setBrush(Qt::black);
    painter.drawText(QRect(QPoint(6, 6), size), Qt::AlignCenter, text);
    painter.end();
	mPixmapValid = QPixmap::fromImage(image);

	// inValid
	image.fill(qRgba(0, 0, 0, 0));
    gradient.setColorAt(0.0, Qt::white);
    gradient.setColorAt(0.2, QColor(100, 100, 155));
    gradient.setColorAt(0.8, QColor(100, 100, 155));
    gradient.setColorAt(1.0, QColor( 77,  77, 100));
    painter.begin(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(gradient);
    painter.drawRoundedRect(QRectF(0.5, 0.5, image.width() - 1, image.height() - 1), 25, 25, Qt::RelativeSize);
    painter.setFont(font);
    painter.setBrush(Qt::black);
    painter.drawText(QRect(QPoint(6, 6), size), Qt::AlignCenter, text);
    painter.end();
	mPixmapInvalid = QPixmap::fromImage(image);
}

void ItemModule::InnerRemoveConnection(ItemConnection* connection, size_t socket)
{
	if (!connection->RemoveSocket(socket))
	{	// there is another connection, we dont need to remove connection
		return;
	}
	InnerRemoveConnection(connection);
}

void ItemModule::InnerRemoveConnection(ItemConnection* connection)
{
	// remove from source
	ConnectionsType& sourceConnections = connection->GetSource()->mConnections;
	const auto iterS = std::remove(std::begin(sourceConnections), std::end(sourceConnections), connection);
	sourceConnections.erase(iterS, std::end(sourceConnections));
	
	// remove from target
	ConnectionsType& targetConnections = connection->GetTarget()->mConnections;
	const auto iterT = std::remove(std::begin(targetConnections), std::end(targetConnections), connection);
	targetConnections.erase(iterT, std::end(targetConnections));

	// and finaly delete conneciton
	delete connection;
}

void ItemModule::OnCalculationEnd()
{	// call interfaceChange, he handle it
	slInterfaceChanged();
}

//											iReportable											//

void ItemModule::Message(BaseLib::iModule* sender, BaseLib::iReportable::eMessageType type, std::string message)
{
	// create message
	DataModels::Message::Item dataMessage;
	dataMessage.Module = QString(sender->GetId().c_str());	
	dataMessage.Message = message.c_str();
	dataMessage.Type = type;
	// report message
	QMetaObject::invokeMethod(&mMessageModel, "slAddMessage", Qt::QueuedConnection, Q_ARG(DataModels::Message::Item, dataMessage) );
};

void ItemModule::Progress(BaseLib::iModule* sender, int min, int max, int value)
{
	// calculate progress
	double progress = (double)value / (double)(max - min);
	// check progress boundaries
	if (progress < 0)
	{
		progress = 0;
	}
	else if (progress > 1)
	{
		progress = 1;
	}
	// check change size
	if (std::abs(mLastProgress - progress) < 0.01)
	{	// minor change -> ignore
	}
	else
	{
		// emit progress change .. 
		QMetaObject::invokeMethod(this, "slProgressChange", Qt::QueuedConnection, Q_ARG(double, progress));
	}
}

void ItemModule::ConnectionChanged(BaseLib::iModule* sender)
{	// connection change, call from OnSourceCheck -> we expect mainly disconnecting on sources .. 
	for (int i = mConnections.size() - 1; i >= 0; --i)
	{	// check if logic connection exist ... 
		ItemConnection* connection = mConnections[i];

		if (connection->GetTarget()->GetModule() == mModule->GetModule())
		{	// target == this.module .. so continue check
		}
		else
		{	// sender is not == this.module, so some else will care about this connection
			continue; // skip it
		}

		// check if all socket in connection are valid .. 
		auto iter = connection->GetSockets().cbegin();
		auto iterEnd = connection->GetSockets().cend();

		for (;iter != iterEnd; ++iter)
		{	// check if graphics rep. is valid ( has logic eq. )
			if (mModule->GetModule()->GetSocket(*iter) == nullptr)
			{	// we have stored connection .. but it's not valid .. 
				
				// disconnecting
				mConnectionDeleted = true;
				
				// remove connection on given socket
				if (connection->RemoveSocket(*iter))
				{	// connection has no valid socket left .. so we will remove it
					InnerRemoveConnection(connection);
					// end control for current i value
					break;
				}
			}
		}

	}
};

void ItemModule::InterfaceChanged(BaseLib::iModule* sender)
{
	QMetaObject::invokeMethod(this, "slInterfaceChanged", Qt::QueuedConnection);
};

//												Slots											//

void ItemModule::slInterfaceChanged()
{
	if (mModule->GetModule()->PrepareForConnection())
	{
		setPixmap(mPixmapValid);
	}
	else
	{
		setPixmap(mPixmapInvalid);
	}
}

void ItemModule::slProgressChange(double progress)
{
	mLastProgress = progress;

	// render new image
	
	// copy base imgae from mPixmapValid
	QImage image(mPixmapValid.toImage());
	QPainter painter(&image);

	// gradient background
	QLinearGradient gradient(0, 0, 0, image.height()-1);
    gradient.setColorAt(0.0, QColor( 95, 237, 66, 150));
    gradient.setColorAt(0.5, QColor( 68, 214, 39, 150));
    gradient.setColorAt(1.0, QColor( 95, 237, 66, 150));

	// render progress
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(gradient);
	painter.drawRoundedRect(QRectF(0.5, 0.5, (image.width() - 1) * progress, image.height() - 1), 25, 25, Qt::RelativeSize);
	
	painter.end();
	// set as active pixmap 
	setPixmap(QPixmap::fromImage(image));
}

}