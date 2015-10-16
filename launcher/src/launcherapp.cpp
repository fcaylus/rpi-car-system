#include "launcherapp.h"
#include "utility.h"
#include "soundmanager.h"

#include "easylogging++.h"

#include "activities/musicactivity.h"
#include "activities/menuactivity.h"

using namespace ilixi;

LauncherApp LauncherApp::_instance = LauncherApp(&ProgramArgs::argc, &ProgramArgs::argv);

// Instance getter
LauncherApp& LauncherApp::instance()
{
    return _instance;
}

// Private constructor
LauncherApp::LauncherApp(int *argc, char ***argv): ilixi::Application(argc, argv)
{
    _executableDir = DirUtility::executableDir();

    // Set theme files
    setBackgroundImage(_executableDir + std::string("/theme/bg.png"));

    const std::string fontPath = _executableDir + std::string("/theme/fonts.xml");
    if(!setFontPack(fontPath.c_str()))
        LOG(ERROR) << "Failed to load the font pack file";

    //const std::string iconsPath = _executableDir + std::string("/theme/icons.xml");
    //if(!setIconPack(iconsPath.c_str()))
    //    LOG(ERROR) << "Failed to load the icon pack file";

    const std::string palettePath = _executableDir + std::string("/theme/palette.xml");
    if(!setPaletteFromFile(palettePath.c_str()))
        LOG(ERROR) << "Failed to load the palette file";

    const std::string stylePath = _executableDir + std::string("/theme/style.xml");
    if(!setStyleFromFile(stylePath.c_str()))
        LOG(ERROR) << "Failed to load the style file";

    LOG(INFO) << "Theme initialized !";

    // Set the toolbar
    _toolBar = new LauncherToolBar();
    setToolbar(_toolBar, false);

    _menuButton = new ToolButton("", _toolBar);
    _menuButton->setIcon(loadLauncherIcon("home"));
    _menuButton->setCheckable(false);
    _toolBar->addWidget(_menuButton);

    _menuButton->sigClicked.connect([this](){
        showActivity("act_menu");
    });

    // Add activities
    setLayout(new VBoxLayout);

    MenuActivity *menuAct = new MenuActivity();
    registerActivity(new MusicActivity());
    menuAct->addMenuTile(0, 0, "music", "Ma Musique", "act_music");

    registerActivity(menuAct);

    showActivity("act_music");

    // Play a test sound
    SoundManager::instance().playFromFile("/home/fabien/test.mp3");
}

StylistBase* LauncherApp::stylist() const
{
    return _toolBar->stylist();
}

// Static methods
Icon* LauncherApp::loadLauncherIcon(const std::string& name)
{
    return LauncherApp::loadIcon(name, LauncherApp::instance().stylist()->defaultParameter(StyleHint::ToolBarButtonHeight)*2);
}

Icon* LauncherApp::loadIcon(const std::string& name, const int width)
{
    Icon* icon = new Icon(iconPathForName(name));
    icon->setSize(Size(width, width));
    return icon;
}

std::string LauncherApp::iconPathForName(const std::string &name)
{
    // On rpi, use SVG
#ifdef READY_FOR_CARSYSTEM
    return DirUtility::executableDir() + std::string("/theme/icons/svg/") + name + std::string(".svg");
#else
    return DirUtility::executableDir() + std::string("/theme/icons/png/") + name + std::string(".png");
#endif
}

void LauncherApp::registerToolBarWidget(const std::string &activityName, Widget *widget)
{
    if(_toolBarWidgetsMap.count(activityName) != 0)
    {
        LOG(ERROR) << "The activity " << activityName << " had already registered a widget !";
        return;
    }

    widget->setVisible(false);
    _toolBarWidgetsMap.insert(std::make_pair(activityName, widget));
    _toolBar->addWidget(widget);
    LOG(INFO) << "Successfully register a widget for " << activityName << "!";
}

void LauncherApp::showActivity(const std::string &activityName)
{
    LOG(INFO) << "1" << activityName;
    if(_activitiesMap.count(activityName) == 0)
        return;

    // Check previous activity and hide it
    if(_launchedActivity != "")
    {
        // Re-show the toolbar if the previous activity is the menu
        if(_launchedActivity == "act_menu")
            _toolBar->setVisible(true);

        _activitiesMap.at(_launchedActivity)->setVisible(false);
        if(_toolBarWidgetsMap.count(_launchedActivity) != 0)
            _toolBarWidgetsMap.at(_launchedActivity)->setVisible(false);
    }

    // Hide the toolbar in the menu
    if(activityName == "act_menu")
        _toolBar->setVisible(false);
    LOG(INFO) << "2" << activityName;

    // Show new activsity
    _activitiesMap.at(activityName)->setVisible(true);
    if(_toolBarWidgetsMap.count(activityName) != 0)
        _toolBarWidgetsMap.at(activityName)->setVisible(true);
    LOG(INFO) << "3" << activityName;

    _previousActivity = _launchedActivity;
    LOG(INFO) << "4" << activityName;
    //TODO: activityName is modified just up there !! WHY ????????ss
    _launchedActivity = activityName;
    LOG(INFO) << "Activity " << activityName << " shown !";
    LOG(INFO) << "Activity " << _previousActivity << " -- previous";
}

void LauncherApp::showPreviousActivity()
{
    showActivity(_previousActivity);
}

// Private
void LauncherApp::registerActivity(BaseActivity *act)
{
    _activitiesMap.insert(std::make_pair(act->name(), act));
    addWidget(act);
}







