#include "launcherapp.h"

#include "dirutility.h"
#include "soundmanager.h"
#include "easylogging++.h"
#include "activities/musicactivity.h"
#include <ui/VBoxLayout.h>

using namespace ilixi;

LauncherApp LauncherApp::_instance = LauncherApp(&ProgramArgs::p_argc, &ProgramArgs::p_argv);

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
    _menuButton->setToolButtonStyle(ToolButton::ToolButtonStyle::IconOnly);
    _menuButton->setIcon(loadLauncherIcon("home"));
    _menuButton->setCheckable(false);
    _toolBar->addWidget(_menuButton);

    _menuDialog = new MenuDialog();
    _menuDialog->sigAccepted.connect([this](){
        showActivity(_menuDialog->requestedActivity());
    });

    _menuButton->sigClicked.connect([this](){
        _menuDialog->execute();
    });

    // Add activities
    VBoxLayout *mainLayout = new VBoxLayout();
    mainLayout->setHorizontalAlignment(Alignment::Horizontal::Center);
    setLayout(mainLayout);
    registerActivity(new MusicActivity());

    _menuDialog->addMenuTile(0, 0, "music", "Ma Musique", "act_music");
    _menuDialog->addMenuTile(0, 1, "radio", "Radio", "act_radio", true);
    _menuDialog->addMenuTile(0, 2, "video", "Mes vidéos", "act_video", true);
    _menuDialog->addMenuTile(1, 0, "phone", "Appeler", "act_phone", true);
    _menuDialog->addMenuTile(1, 2, "upload", "Transférer", "act_upload", true);
    _menuDialog->addMenuTile(2, 2, "settings", "Paramètres", "act_settings");

    showActivity("act_music");

    // Play a test sound
    //SoundManager::instance().playFromFile("/home/fabien/test.mp3");
}

// Public destructor
LauncherApp::~LauncherApp()
{
    delete _toolBar;
    _toolBar = nullptr;

    delete _menuDialog;
    _menuDialog = nullptr;

    LOG(INFO) << "LauncherApp deleted !";
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

void LauncherApp::showActivity(std::string activityName)
{
    if(_activitiesMap.count(activityName) == 0 || _launchedActivity == activityName)
        return;

    // Check previous activity and hide it
    if(_launchedActivity != "")
    {
        _activitiesMap.at(_launchedActivity)->setVisible(false);
        if(_toolBarWidgetsMap.count(_launchedActivity) != 0)
            _toolBarWidgetsMap.at(_launchedActivity)->setVisible(false);
    }

    // Show new activsity
    _activitiesMap.at(activityName)->setVisible(true);
    if(_toolBarWidgetsMap.count(activityName) != 0)
        _toolBarWidgetsMap.at(activityName)->setVisible(true);

    _launchedActivity = activityName;
    LOG(INFO) << "Activity " << activityName << " shown !";
}

// Private
void LauncherApp::registerActivity(BaseActivity *act)
{
    _activitiesMap.insert(std::make_pair(act->name(), act));
    addWidget(act);
}







