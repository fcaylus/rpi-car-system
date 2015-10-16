#ifndef MENUACTIVITY_H
#define MENUACTIVITY_H

#include "baseactivity.h"

/*
 * This is a special activity that allow to launch the others
 * The tiles layout is as follow:
 *  X  X  X     X: activity tile
 *  X  X  X     B: back to previous activity
 *  B     S     S: settings
 */
class MenuActivity : public BaseActivity
{
    public:
        MenuActivity(ilixi::Widget* parent = 0);

        void addMenuTile(const int row, const int column, const std::string& iconName,
                         const std::string& title, const std::string& activityName);

    private:
        ilixi::GridLayout *_mainLayout;
};

class MenuTile : public ilixi::ToolButton
{
    public:
        MenuTile(const std::string& iconName, const std::string& title,
                 const std::string& activityName, ilixi::Widget* parent = 0);

    private:
        std::string _activityName;
};

#endif // MENUACTIVITY_H
