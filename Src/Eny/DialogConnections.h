#ifndef eny_dialogConnection_h
#define eny_dialogConnection_h

/**
 * @author 
 * @version 1.0
 * @date 20.5.2012
 */

// Qt includes
///
#include <qdialog.h>

// Generated includes
///
#include "ui_DialogConnections.h"

// Local includes
///
#include "AssemblyItemModule.h"

namespace Dialog {

/**
 * Dialog manage connection between two module.
 */
class Connections : public QDialog
{
	Q_OBJECT
public:
	Connections(Assembly::ItemModule& source, Assembly::ItemModule& target, QWidget* parent);
private:
	void InitGui();
protected slots:
	void slShowInterface();
private:
	/**
	 * Source item.
	 */
	Assembly::ItemModule& mSource;
	/**
	 * Target item.
	 */
	Assembly::ItemModule& mTarget;
	/**
	 * Graphic interface.
	 */
	Ui::DialogConnections ui;
};

}

#endif // eny_dialogConnection_h
