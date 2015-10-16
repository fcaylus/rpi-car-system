#ifndef LAUNCHERTOOLBAR_H
#define LAUNCHERTOOLBAR_H

#include "ilixiGUI.h"

class LauncherToolBar : public ilixi::ToolBar
{
    public:
        LauncherToolBar(ilixi::Widget* parent = 0);

        ilixi::Size preferredSize() const;
};

#endif // LAUNCHERTOOLBAR_H
