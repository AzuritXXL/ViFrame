#ifndef eny_libraryModel_h
#define eny_libraryModel_h

/**
 * @author 
 * @version 1.0
 * @date 20.5.2012
 */

// Qt includes
///
#include <QDir>
#include <QtGui\qtreewidget.h>

namespace DataModels {
namespace Library {

// Forward declaration
///
class LibraryVirtualItem;

class LibraryModel : public QAbstractItemModel
{
public:
	LibraryModel(QObject *parent = 0);
	LibraryModel(LibraryVirtualItem* root, QObject *parent = 0);
	~LibraryModel();
public:
	LibraryVirtualItem* GetRoot();
public: // Read-Only access
    Qt::ItemFlags flags(const QModelIndex &index) const;
	QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
public: // Hierarchical models
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
public: // Drag and drop
	QMimeData *mimeData(const QModelIndexList &indexes) const;
	// Used to return a list of MIME types that can be decoded and handled by the model. 
    QStringList mimeTypes() const;
	Qt::DropActions supportedDragActions() const;
	// we support drop, but only partialy, we accept certain type of data but dont do anything with them
	bool dropMimeData(const QMimeData *mimeData, Qt::DropAction action, int row, int column, const QModelIndex &parent);
	Qt::DropActions supportedDropActions() const;
private:
	/**
	 * Root item.
	 */
	LibraryVirtualItem* mRoot;
};

} }

#endif // eny_libraryModel_h