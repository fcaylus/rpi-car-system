#include "menuactivity.h"

#include "../launcherapp.h"

using namespace ilixi;

MenuActivity::MenuActivity(Widget* parent): BaseActivity("act_menu", parent)
{
    _mainLayout = new GridLayout(3, 3, this);
    setLayout(_mainLayout);

    addMenuTile(2, 0, "back", "Retour", "back");
}

void MenuActivity::addMenuTile(const int row, const int column, const std::string &iconName,
                               const std::string &title, const std::string &activityName)
{
    MenuTile *tile = new MenuTile(iconName, title, activityName);
    _mainLayout->addWidget(tile, row, column);
}

//**********************************
//**** MenuTile Class **************
//**********************************

MenuTile::MenuTile(const std::string &iconName, const std::string &title,
                   const std::string &activityName, Widget *parent): ToolButton(title, parent)
{
    setIcon(LauncherApp::loadLauncherIcon(iconName));
    _activityName = activityName;

    sigClicked.connect([this]() {
        if(_activityName == "back")
            LauncherApp::instance().showPreviousActivity();
        else
            LauncherApp::instance().showActivity(_activityName);
    });
}

