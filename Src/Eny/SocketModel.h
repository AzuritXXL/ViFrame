#ifndef eny_socketsModel_h
#define eny_socketsModel_h

/**
 * @author 
 * @version 1.0
 * @date 20.5.2012
 */

// Qt includes
///
#include <QtGui\qtableview.h>

// Standart library includes
///
#include <map>

// BaseLib includes
///
#include "Socket.hpp"

namespace DataModels {
namespace Socket {

/**
 * Wrap std::map<std::string, int> from Socket to Qt view model.
 */
class Model : public QAbstractTableModel
{
	Q_OBJECT
public:
    Model(const BaseLib::Sockets::Socket::DataType& data, const BaseLib::Sockets::Socket::PrimaryType& primary, QObject* parent);
public: // QAbstractTableModel
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
private:
	typedef std::pair<std::string, int> DataType;
private:
	/**
	 * Primary value.
	 */
	int mPrimary;
	/**
	 * Primary value name.
	 */
	const std::string& mPrimaryKey;
	/**
	 * Socket data.
	 */
	std::vector<DataType> mData;
};

} }

#endif // eny_socketsModel_h