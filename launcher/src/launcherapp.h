#ifndef LAUNCHERAPP_H
#define LAUNCHERAPP_H

#include <core/Application.h>
#include <map>
#include <string>

#include "widgets/launchertoolbar.h"
#include "activities/baseactivity.h"
#include "menudialog.h"
#include "usbmanager.h"

// Make argc and argv static since they are used by the singleton app
namespace ProgramArgs {
    static int p_argc = 0;
    static char** p_argv = nullptr;
}

// Singleton application object
// Also set theme properties
class LauncherApp : public ilixi::Application
{
    public:
        static LauncherApp& instance();

        // This method allow an activity to register a widget in the toolbar
        // If there are several widgets, pack them into a ContainerWidget
        void registerToolBarWidget(const std::string& activityName, ilixi::Widget* widget);

        void showActivity(std::string activityName);

        // Return stylist object
        ilixi::StylistBase* stylist() const;

        // Static method that allow widget to load icons
        static ilixi::Icon* loadLauncherIcon(const std::string& name);
        static ilixi::Icon* loadIcon(const std::string& name, const int width);
        static std::string iconPathForName(const std::string& name);

        // Re-implemented
        void exec();
        void searchUSBDevice();

    private:
        LauncherApp(int* argc, char*** argv); // ctor
        std::string _executableDir;

        USBManager *_usbManager = nullptr;
        USBManagerThread *_usbThread = nullptr;
        bool _usbManagerReady = false;

        LauncherToolBar *_toolBar;
        ilixi::ToolButton *_menuButton;
        MenuDialog *_menuDialog;

        std::map<std::string, ilixi::Widget*> _toolBarWidgetsMap;
        std::map<std::string, BaseActivity*> _activitiesMap;
        std::string _launchedActivity = "";

        void registerActivity(BaseActivity *act);

        static LauncherApp _instance;
};

#endif // LAUNCHERAPP_H
