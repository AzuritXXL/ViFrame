#ifndef eny_widgetTabs_h
#define eny_widgetTabs_h

/**
 * @author 
 * @version 1.0
 * @date 20.5.2012
 */

// Qt includes
///
#include <qtabwidget.h>

namespace Widget {

/**
 * Sub class provide acces to QTabBar .
 */
class Tabs : public QTabWidget
{
	Q_OBJECT
public:
	Tabs(QWidget* parent);
public:
	const QTabBar* GetTabBar();
};

}

#endif // eny_widgetTabs_h