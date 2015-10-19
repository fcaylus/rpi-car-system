#ifndef MENUDIALOG_H
#define MENUDIALOG_H

#include <ilixiGUI.h>

/*
 * This is a special activity that allow to launch the others
 * The tiles layout is as follow:
 *  X  X  X     X: activity tile
 *  X  X  X     B: back to previous activity
 *  B     S     S: settings
 */
class MenuDialog : public ilixi::Dialog
{
    public:
        MenuDialog(ilixi::Widget* parent = 0);

        void addMenuTile(const int row, const int column, const std::string& iconName,
                         const std::string& title, const std::string& activityName, bool disabled = false);

        void setRequestedActivity(std::string activity);
        std::string requestedActivity() const;

    private:
        ilixi::GridLayout *_mainLayout;
        std::string _requestedActivity = "";
};

class MenuTile : public ilixi::ToolButton
{
    public:
        MenuTile(const std::string& iconName, const std::string& title,
                 const std::string& activityName, MenuDialog* parent);

        ilixi::Size preferredSize() const;

    private:
        std::string _activityName;
};

class MenuIcon : public ilixi::Icon
{
    public:
        MenuIcon(ilixi::Widget* parent = 0);
        ilixi::Size preferredSize() const;
};

#endif // MENUDIALOG_H
