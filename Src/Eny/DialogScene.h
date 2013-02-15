#ifndef eny_dialogScene_h
#define eny_dialogScene_h

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
#include "ui_DialogScene.h"

namespace Dialog {

class Scene : public QDialog
{
	Q_OBJECT
public:
	Scene(double sizeX, double sizeY, double maxX, double maxY, QWidget* parent);
public:
	/**
	 * Return x size.
	 * @return x size
	 */
	double GetX() const;
	/**
	 * Return y size.
	 * @return y size
	 */
	double GetY() const;
protected:
	/**
	 * Set user interface.
	 */
	void InitGui(double sizeX, double sizeY, double maxX, double maxY);
	/**
	 * Set scales for absolute size.
	 */
	void SetAbsolutScale();
	/**
	 * Set scales for relative size.
	 */
	void SetRelativeScale();
protected slots:
	void slRadioChange();
private:
	/**
	 * Graphics interface.
	 */
	Ui::DialogScene ui;
	/**
	 * Original size x.
	 */
	double mMaxX;
	/**
	 *Original size y.
	 */
	double mMaxY;
	/**
	 * Use absolute size.
	 */
	bool mAbsolute;
};

}

#endif // eny_dialogScene_h