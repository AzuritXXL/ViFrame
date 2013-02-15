#ifndef eny_widgetSocket_h
#define eny_widgetSocket_h

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
#include "ui_WidgetSocket.h"

// BaseLib includes
///
#include "Socket.hpp"

namespace Widget {

/**
 * Wrap WidgetSocket functionality.
 */
class Socket : public QWidget
{
	Q_OBJECT
public:
	Socket(const BaseLib::Sockets::Socket& socket, int number, QWidget* parent);
protected slots:
	/**
	 * Show socket detail.
	 */
	void slShowSocketDetail();
private:
	/**
	 * Assign socket.
	 */
	const BaseLib::Sockets::Socket& mSocket;
	/**
	 * Graphics interface.
	 */
	Ui::WidgetSocket ui;
};

}

#endif // eny_widgetSocket_h