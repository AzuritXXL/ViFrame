#ifndef eny_messageModel_h
#define eny_messageModel_h

/**
 * @author 
 * @version 1.0
 * @date 20.5.2012
 */

// Qt includes
///
#include <qlist.h>
#include <QtGui\qtableview.h>

// Local includes
///
#include "MessageItem.h"

namespace DataModels {
namespace Message {

class Model : public QAbstractTableModel
{
	Q_OBJECT
private:
	QList<Item> mMessages;
	QIcon mIconError;
	QIcon mIconWarning;
	QIcon mIconInformation;
public:
    Model(QObject* parent);
public:
	/**
	 * Detele all stored messages.
	 */
	void Clear();
public: // QAbstractTableModel
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
public slots:
	/**
	 * Add message.
	 */
	void slAddMessage(const DataModels::Message::Item& message);	
};

} }

#endif // eny_messageModel_h