#ifndef eny_dialogExport_h
#define eny_dialogExport_h

/**
 * @author 
 * @version 1.0
 * @date 20.5.2012
 */

// Qt includes
///
#include <qlist.h>
#include <qdialog.h>
#include <QStringListModel>

// Generated includes
///
#include "ui_DialogExport.h"

namespace Dialog {

/**
 * Let user choose attributes to export. 
 * Add atribute name and parent. 
 *	name - objects name
 *  parent - name of object parent, empty for root
 */
class Export : public QDialog
{
public:
	Export(QList<QString> attributesName, QWidget* parent, const std::string& objectName, const std::string& parentName);
public:
	/**
	 * @return choosen separator
	 */
	QString GetSeparator() const;
	/**
	 * @return list of selected attributes
	 */
	QList<QString> GetSelection() const;
private:
	/**
	 * Data model with all atributes.
	 */
	QStringListModel* mAll;
	/**
	 * Data model with only selected attributes.
	 */
	QStringListModel* mSelected;
	/**
	 * Dialog user interface.
	 */
	Ui::DialogExport ui;
};

}

#endif // eny_dialogExport_h