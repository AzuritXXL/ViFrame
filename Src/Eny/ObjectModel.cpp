#include "stdafx.h"
// ...
#include "ObjectModel.h"

namespace DataModels {
namespace Object {

Model::Model(BaseLib::Objects::Object& object, QObject* parent)
	: QAbstractTableModel(parent), mObject(object)
{ }

//										QAbstractTableModel										//

int Model::rowCount(const QModelIndex &parent) const
{
	return mObject.GetData().size();
}

int Model::columnCount(const QModelIndex &parent) const
{
	return 2;
}

QVariant Model::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
		return QVariant();
	}

	// we increase size by +1, becouse of Primary data
	if (index.row() >= mObject.GetData().size() || index.row() < 0)
	{
		return QVariant();
	}

	switch(role)
	{
	case Qt::DisplayRole: // The key data to be rendered in the form of text. (QString)
		{
			// get item on row
			int targetRow = index.row();
			auto iter = mObject.GetData().cbegin();
			for (size_t i = 0; i < targetRow; ++i, ++iter);

			// switch by column
			switch(index.column())
			{
			case 0:
				return iter->first.c_str();
			case 1:
				return iter->second->ToString().c_str();
			}
			break;
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
			return tr("Name");
        case 1:
            return tr("Value");
        default:
           return QVariant();
        }
    }
	return QVariant();
}


} }