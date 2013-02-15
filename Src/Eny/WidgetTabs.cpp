#include "stdafx.h"
// ...
#include "WidgetTabs.h"

namespace Widget {

Tabs::Tabs(QWidget* parent)
	: QTabWidget(parent)
{ };


const QTabBar* Tabs::GetTabBar()
{
	return tabBar();
}

}
