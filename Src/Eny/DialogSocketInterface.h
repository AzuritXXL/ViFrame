#ifndef eny_dialogSocketInterface_h
#define eny_dialogSocketInterface_h

/**
 * @author 
 * @version 1.0
 * @date 20.5.2012
 */

// Qt includes
///
#include "qdialog.h"

// Generated includes
///
#include "ui_DialogSocketInterface.h"

// Local includes
///
#include "SocketModel.h"

// Baselib includes
///
#include "Socket.hpp"

namespace Dialog {
	
class SocketInterface : public QDialog
{
public:
	SocketInterface(const BaseLib::Sockets::Socket::DataType& data, const BaseLib::Sockets::Socket::PrimaryType& primary, int socketInterface, const std::string& socketName, QWidget* parent);
private:
	Ui::DialogSocketInterface ui;
	/**
	 * Data model.
	 */
	DataModels::Socket::Model mModel;
};

}

#endif // eny_dialogSocketInterface_h