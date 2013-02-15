#ifndef dialogSetMain_h
#define dialogSetMain_h

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
#include "ui_DialogSetMain.h"

// Local includes
///
#include "OutputPolicy.h"

namespace Dialog {

class SetMain : public QDialog
{
	Q_OBJECT
public:
	/**
	 * @param[in] names of selectable values
	 * @param[in] parent
	 */
	SetMain(QList<QString>& attNames, QWidget* parent);
public:
	/**
	 * Get policy.
	 * @return policy
	 */
	const Output::Policy& GetPolicy() const;
protected slots:
	/**
	 * React on accept dialog. Gather data and store them into mPolicy.
	 */
	void slAccept();
private:
	/**
	 * Graphical interface.
	 */
	Ui::DialogSetMain ui;
	/**
	 * Policy with main value setting.
	 */
	Output::Policy mPolicy;
};

}

#endif // dialogSetMain_h