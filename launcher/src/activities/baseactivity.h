#ifndef BASEACTIVITY_H
#define BASEACTIVITY_H

#include "../widgets/containerwidget.h"

/*
 * Base class for all activities
 * You can set the widgets on the screen.
 * To add a widget on the toolbar, use LauncherApp methods
 *
 * Each activity has two important elements:
 * - the widgets
 * - a name. This name must be unique and is used to manage activities in the menu
 *   This name is also used when adding a widget to the toolbar
 */
class BaseActivity : public ContainerWidget
{
    public:
        BaseActivity(const std::string& name, ilixi::Widget *parent = 0);

        std::string name() const;

        static ilixi::Size availableArea();

    private:

        std::string _name;
};

#endif // BASEACTIVITY_H
