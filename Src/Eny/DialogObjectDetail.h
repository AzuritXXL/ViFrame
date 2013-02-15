#ifndef eny_dialogObjectDetail_h
#define eny_dialogObjectDetail_h

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
#include "ui_DialogObjectDetail.h"

// BaseLib includes
///
#include "Object.hpp"
#include "ObjectModel.h"

namespace Dialog {

class ObjectDetail : public QDialog
{
public:
	ObjectDetail(BaseLib::Objects::Object& object, QWidget* parent);
private:
	/**
	 * Model for data presentation.
	 */
	DataModels::Object::Model mModel;
	/**
	 * Graphics interface.
	 */
	Ui::DialogObjectDetail ui;
};

}

#endif // eny_dialogObjectDetail_h