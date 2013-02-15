#ifndef eny_objectModel_h
#define eny_objectModel_h

/**
 * @author 
 * @version 1.0
 * @date 20.5.2012
 */

// Qt includes
///
#include <QtGui\qtableview.h>

// BaseLib includes
///
#include "Object.hpp"

namespace DataModels {
namespace Object {

/**
 * Wrap BaseLib::Object to Qt view model.
 */
class Model : public QAbstractTableModel
{
	Q_OBJECT
public:
    Model(BaseLib::Objects::Object& object, QObject* parent);
public: // QAbstractTableModel
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
private:
	BaseLib::Objects::Object& mObject;
};

} }

#endif // eny_objectModel_h