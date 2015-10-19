#include "menudialog.h"

#include "launcherapp.h"
#include "easylogging++.h"

using namespace ilixi;

MenuDialog::MenuDialog(Widget* parent): Dialog("", ButtonOption::NoButtonOption, parent)
{
    _mainLayout = new GridLayout(3, 3, this);
    setLayout(_mainLayout);

    addMenuTile(2, 0, "back", "Retour", "back");
    _mainLayout->addWidget(new Spacer(Orientation::Horizontal, this), 2, 1);

    _titleSize = Size(0,0);
}

void MenuDialog::addMenuTile(const int row, const int column, const std::string &iconName,
                               const std::string &title, const std::string &activityName, bool disabled)
{
    MenuTile *tile = new MenuTile(iconName, title, activityName, this);
    if(disabled)
        tile->setDisabled();
    _mainLayout->addWidget(tile, row, column);
}

void MenuDialog::setRequestedActivity(std::string activity)
{
    _requestedActivity = activity;
}

std::string MenuDialog::requestedActivity() const
{
    return _requestedActivity;
}

/***********************************/
/***** MenuTile Class **************/
/***********************************/

MenuTile::MenuTile(const std::string &iconName, const std::string &title,
                   const std::string &activityName, MenuDialog *parent): ToolButton(title, parent)
{
    setToolButtonStyle(ToolButtonStyle::IconAboveText);
    setSingleLine(true);

    // Increase the font size
    Font* customFont = new Font(*font());
    customFont->setSize(font()->size() + 3);
    setFont(customFont);

    // Load the icon
    MenuIcon *icon = new MenuIcon(this);
    icon->setImage(LauncherApp::iconPathForName(iconName));
    setIcon(icon);

    _activityName = activityName;

    sigClicked.connect([this, parent]() {

        // Reject if go back, else accept
        if(_activityName == "back")
                parent->reject();
        else
        {
            parent->setRequestedActivity(_activityName);
            parent->accept();
        }
    });
}

Size MenuTile::preferredSize() const
{
    const int windowWidth = LauncherApp::instance().width();
    const int windowHeight = LauncherApp::instance().height();

    const int width = (windowWidth / 3) - (windowHeight * .07);
    const int height = (windowHeight / 3) - (windowHeight * .07);

    return Size(width, height);
}

/***********************************/
/***** MenuIcon Class **************/
/***********************************/

MenuIcon::MenuIcon(Widget *parent): Icon(parent)
{

}

Size MenuIcon::preferredSize() const
{
    if(parent())
    {
        MenuTile *parentTile = static_cast<MenuTile*>(parent());
        const int height = parentTile->height() - parentTile->textExtents().height() - (parentTile->height() * .25);
        return Size(height, height);
    }

    return Size();
}





