#include <ilixiGUI.h>

#include "easylogging++.h"

#include "launcherapp.h"

INITIALIZE_EASYLOGGINGPP

using namespace ilixi;

int main(int argc, char* argv[])
{
    START_EASYLOGGINGPP(argc, argv);

    // Set logger flags
    el::Loggers::addFlag(el::LoggingFlag::LogDetailedCrashReason);
    el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
    el::Loggers::addFlag(el::LoggingFlag::AutoSpacing);

    /*el::Configurations defaultLogConf;
    defaultLogConf.setToDefault();
    defaultLogConf.setGlobally(el::ConfigurationType::ToFile, "false");
    el::Loggers::reconfigureLogger("default", defaultLogConf);*/

    LOG(INFO) << "Program start !";

    // Set param args
    ProgramArgs::p_argc = argc;
    ProgramArgs::p_argv = argv;

    LauncherApp &app = LauncherApp::instance();

    app.exec();
    return 0;
}
