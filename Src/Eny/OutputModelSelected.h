#ifndef eny_outputModelSelected_h
#define eny_outputModelSelected_h

/**
 * @author 
 * @version 1.0
 * @date 20.5.2012
 */

// Standart library includes
///
#include <vector>

// Qt includes 
///
#include <QAbstractListModel>

// Local includes
///
#include "OutputVirtualItem.h"

namespace DataModels {
namespace Output {

// Forward declaration
///
class DataItem;

class ModelSelected : public QAbstractListModel
{
	Q_OBJECT
public:
	typedef std::vector<DataItem*> DataItemsType;
public:
	ModelSelected(QObject* parent);
public:
	/**
	 * Retake pointers from data and use them to create model.
	 * @param[in] data
	 */
	void SetData(DataItemsType& data);
	/**
	 * Append data on end of the model.
	 * @param[in] data to append
	 */
	void Append(DataItem* data);
	/**
	 * Remove data from model.
	 * @param[in] data to remove
	 */
	void Remove(DataItem* data);
	/**
	 * Return data on index.
	 */
	DataItem* AccesData(int row);
	/**
	 * Return const refrence to model data.
	 * @return refrence to model data
	 */
	const DataItemsType& AccesData() const;
public: // QAbstractItemModel
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
private:
	/**
	 * DataItems in module.
	 */
	DataItemsType mDataItems;
};

} }

#endif // eny_outputModelSelected_h