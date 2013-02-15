#include "stdafx.h"
// ...
#include "OutputModelSelected.h"
#include "OutputDataItem.h"

namespace DataModels {
namespace Output {

ModelSelected::ModelSelected(QObject* parent)
	: QAbstractListModel(parent)
{ }

void ModelSelected::Append(DataItem* data)
{
	beginInsertRows(QModelIndex(), mDataItems.size(), mDataItems.size());
	// append data
	mDataItems.push_back(data);

	endInsertRows();
}

void ModelSelected::Remove(DataItem* data)
{
	auto iter = std::begin(mDataItems);
	auto iterEnd = std::end(mDataItems);
	size_t index = 0;

	for (;iter != iterEnd; ++index, ++iter)
	{
		if (*iter == data)
		{	// remove
			beginRemoveRows(QModelIndex(), index, index);

			// swap content
			std::swap(*iter, mDataItems.back());
			// release data
			mDataItems.pop_back();

			endRemoveRows();
			break;
		}
	}
}

void ModelSelected::SetData(DataItemsType& data)
{
	beginResetModel();

	// retake data
	std::swap(mDataItems, data);

	endResetModel();
}

DataItem* ModelSelected::AccesData(int row)
{
	if (row < 0 || row >= mDataItems.size())
	{
		return nullptr;
	}
	return mDataItems[row];
}

const ModelSelected::DataItemsType& ModelSelected::AccesData() const
{
	return mDataItems;
}

//										QAbstractTableModel										//

int ModelSelected::rowCount(const QModelIndex &parent) const
{
	return mDataItems.size();
}

QVariant ModelSelected::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
		return QVariant();
	}

	// we increase size by +1, becouse of Primary data
	if (index.row() >= mDataItems.size() || index.row() < 0)
	{
		return QVariant();
	}

	// return data from items
	return mDataItems[index.row()]->GetData(role);
}

QVariant ModelSelected::headerData(int section, Qt::Orientation orientation, int role) const
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
        default:
           return QVariant();
        }
    }
	return QVariant();
}

} }