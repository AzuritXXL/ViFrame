#include "stdafx.h"
// ...
#include "SocketModel.h"

namespace DataModels {
namespace Socket {

Model::Model(const BaseLib::Sockets::Socket::DataType& data, const BaseLib::Sockets::Socket::PrimaryType& primary, QObject* parent)
	: QAbstractTableModel(parent), mPrimary(primary.Att.Type), mPrimaryKey(primary.Name)
{	// transport data into mData
	mData.reserve(data.size() + 1);
	// primary data
	{
		std::stringstream ss;
		ss << primary.Att.Level << "." << primary.Name;
		std::string fullName = ss.str();
		mData.push_back(DataType(fullName, primary.Att.Type) );
	}

	// secondary data
	auto iter = std::begin(data);
	auto iterEnd = std::end(data);
	for (;iter != iterEnd; ++iter)
	{
		auto innerIter = std::begin(iter->second);
		auto innerIterEnd = std::end(iter->second);
		// add value for each
		for (;innerIter != innerIterEnd; ++innerIter)
		{	// construct name
			std::stringstream ss;
			ss << innerIter->Level << "." << iter->first;
			std::string fullName = ss.str();
			// store
			mData.push_back(DataType(fullName, innerIter->Type) );
		}
	}
}

//										QAbstractTableModel										//

int Model::rowCount(const QModelIndex &parent) const
{
	return mData.size();
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
	if (index.row() >= mData.size() || index.row() < 0)
	{
		return QVariant();
	}

	// new index is substracted by one
	switch(role)
	{
	case Qt::DisplayRole: // The key data to be rendered in the form of text. (QString)
		{
			// switch by column
			switch(index.column())
			{
			case 0:
				return mData[index.row()].first.c_str();
			case 1:
				// convert type to string description
				switch(mData[index.row()].second)
				{
				case 1: return "int";
				case 2: return "double";
				case 3: return "string";
				case 4: return "vector of ints";
				case 5: return "vector of doubles";
				}
			}
			break;
		}
	case Qt::ForegroundRole:
		if (index.row() == 0)
		{
			return Qt::blue;
		}
		else
		{
			return QVariant();
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
			return tr("Key");
        case 1:
            return tr("Value type");
        default:
           return QVariant();
        }
    }
	return QVariant();
}

} }