#ifndef eny_dialogSaveScreen_h
#define eny_dialogSaveScreen_h

/**
 * @author 
 * @version 1.0
 * @date 20.5.2012
 */

// Qt includes
//
#include <qdialog.h>

// Generated includes
///
#include "ui_DialogSaveScreen.h"

namespace Dialog {

class SaveScreen : public QDialog
{
	Q_OBJECT
public:
	enum eLegend 
	{
		None
		,LeftTop
		,LeftBottom
		,RightTop
		,RightBottom		
	};
public:
	/**
	 * @param[in] parent
	 * @param[in] true to force GetUseRoot return true
	 */
	SaveScreen(QWidget* parent, bool forceUseRoot = false);
public:
	/**
	 * @return set widtd
	 */
	int GetWidth();
	/**
	 * @return set height
	 */
	int GetHeight();
	/**
	 * @return set legend
	 */
	eLegend GetLegend();
	/**
	 * Font for legend.
	 */ 
	const QFont& GetFont();
	/**
	 *  @return true if use root groups
	 */
	bool GetUseRoot();
protected slots:
	void slSetFont();
private:
	Ui::DialogSaveScreen ui;
	/**
	 * Font for legend.
	 */
	QFont mFont;
};

}

#endif // eny_dialogSaveScreen_h