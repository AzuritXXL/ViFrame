#ifndef eny_widgetConnection_h
#define eny_widgetConnection_h

/**
 * @author 
 * @version 1.0
 * @date 20.5.2012
 */

// Qt includes
///
#include <qwidget.h>

// Generated includes
///
#include "ui_WidgetConnection.h"

// Local includes
//
#include "AssemblyItemModule.h"
#include "DialogSocketInterface.h"

namespace Widget {

class Connection : public QWidget
{
	Q_OBJECT
public:
	Connection(Assembly::ItemModule& source, Assembly::ItemModule& target, size_t socket, QWidget* parent);
protected:
	/**
	 * Init user interface.
	 */
	void InitGui();
protected slots:
	/**
	 * Show socket interface.
	 */
	void slInterface();
	/**
	 * Action button.
	 */
	void slAction();
private:
	/**
	 * Source module.
	 */
	Assembly::ItemModule& mSource;
	/**
	 * Target module.
	 */
	Assembly::ItemModule& mTarget;
	/**
	 * Socke number.
	 */
	size_t mSocket;
	/**
	 * True if source can be connected to target.
	 */
	bool mConnectable;
	/**
	 * Graphics interface.
	 */
	Ui::WidgetConnection ui;
};

}

#endif // eny_widgetConnection_h