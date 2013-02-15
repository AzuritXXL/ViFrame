#ifndef eny_dialogDetail_h
#define eny_dialogDetail_h

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
#include "ui_DialogDetail.h"

// BaseLib incldues
///
#include "ModuleHolder.h"

namespace Dialog {

class Detail : public QDialog
{
public:
	Detail(Logic::ModuleHolder& module, QWidget* parent);
protected:
	/**
	 * Init detail tab.
	 */
	void InitDetail();
	/**
	 * Init input socket tab.
	 */
	void InitInput();
private:
	/**
	 * Module.
	 */
	Logic::ModuleHolder& mModule;
	/**
	 * Graphic interface.
	 */
	Ui::DialogDetail ui;
};

}

#endif // eny_dialogDetail_h