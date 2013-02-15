#include "stdafx.h"
// ...
#include "MessageModel.h"
// BaseLib
#include "Reportable.hpp"

namespace DataModels {
namespace Message {

Model::Model(QObject* parent)
	: QAbstractTableModel(parent)
{	// load icons
	mIconError.addFile(":/Icons/Resources/IconError.png");
	mIconInformation.addFile(":/Icons/Resources/IconInfo.png");
	mIconWarning.addFile(":/Icons/Resources/IconWarning.png");
}

void Model::Clear()
{
	mMessages.clear();
}

//										QAbstractTableModel										//

int Model::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return mMessages.size();
}

int Model::columnCount(const QModelIndex &parent) const
{	// number of "column" in MessageItem
	Q_UNUSED(parent);	
	return 4;
}

QVariant Model::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
		return QVariant();
	}

	if (index.row() >= mMessages.size() || index.row() < 0)
	{
		return QVariant();
	}
	
	switch(role)
	{
	case Qt::DisplayRole: // The key data to be rendered in the form of text. (QString)
		{
			switch(index.column())
			{
			case 0:
				return mMessages[index.row()].CreatedTime;
			case 1:
				return QVariant();
			case 2:
				return mMessages[index.row()].Module;
			case 3:
				return mMessages[index.row()].Message;
			default:
				return QVariant();
			}
		} 
	case Qt::DecorationRole: // The data to be rendered as a decoration in the form of an icon. (QColor, QIcon or QPixmap)
		{
			if (index.column() == 1)
			{
				switch(mMessages[index.row()].Type)
				{
				case BaseLib::iReportable::Error:
					return mIconError;
				case BaseLib::iReportable::Warning:
					return mIconWarning;
				case BaseLib::iReportable::Information:
					return mIconInformation;
				default:
					return QVariant();
				}
			}
			else
			{
				return QVariant();
			}
		}
	}
	return QVariant();
}

QVariant Model::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
	{
		return QVariant();
	}

    if (orientation == Qt::Horizontal)
	{
		switch (section)
		{
        case 0:
			return tr("Time");
        case 1:
            return tr("");
        case 2:
            return tr("Module");
        case 3:
            return tr("Message");
        default:
           return QVariant();
        }
    }
	return QVariant();
}

//											Slots												//

void Model::slAddMessage(const DataModels::Message::Item& message)
{
	// report begining of insert
	beginInsertRows(QModelIndex(), 0, 0);
	// insert
	mMessages.insert(mMessages.begin(), message);
	// report end of insert
	endInsertRows();
}


} }
