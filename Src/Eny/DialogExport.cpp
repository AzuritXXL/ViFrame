#include "stdafx.h"
// Qt
#include <QAbstractListModel>
// ..
#include "DialogExport.h"


class StringListModel : public QStringListModel
{
public:
	StringListModel(QWidget* parent)
		: QStringListModel(parent)
	{ }
	StringListModel(QList<QString> list, QWidget* parent)
		: QStringListModel(list, parent)
	{ }
public:
	virtual Qt::ItemFlags flags(const QModelIndex& index) const
	{	// just modify flags, do not allow items rewrite ( merger )
		if (index.isValid())
		{
			return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
		}
		else
		{
			return Qt::ItemIsDropEnabled;
		}
	}
};

namespace Dialog {

Export::Export(QList<QString> attributesName, QWidget* parent, const std::string& objectName, const std::string& parentName)
	: QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
{
	ui.setupUi(this);
	// add aditional atributes
	attributesName.insert(0, objectName.c_str());
	attributesName.insert(1, parentName.c_str());
	// prepare models
	mAll = new StringListModel(attributesName, this);
	mSelected = new StringListModel(this);
	// set models
	ui.listAll->setModel(mAll);
	ui.listSelected->setModel(mSelected);
}

QString Export::GetSeparator() const
{
	return ui.cmbSeparator->currentText();
}

QList<QString> Export::GetSelection() const
{
	return mSelected->stringList();
}

}