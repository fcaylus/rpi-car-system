#include "usbmanager.h"
#include "easylogging++.h"

//unsigned char buffer[128];

INITIALIZE_EASYLOGGINGPP

int main(int argc, char *argv[])
{
    START_EASYLOGGINGPP(argc, argv);

    // Set logger flags
    el::Loggers::addFlag(el::LoggingFlag::LogDetailedCrashReason);
    el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
    el::Loggers::addFlag(el::LoggingFlag::AutoSpacing);

    el::Configurations defaultLogConf;
    defaultLogConf.setToDefault();
    defaultLogConf.setGlobally(el::ConfigurationType::ToFile, "false");
    el::Loggers::reconfigureLogger("default", defaultLogConf);


    LOG(INFO) << "Program start !";

    int result = 0;

    USBManager usbMgr;
    result = usbMgr.init((char *) "Tout_juste",
                         (char *) "rpi-car-system",
                         (char *) "Allow communication between the phone and the car",
                         (char *) "1.0",
                         (char *) "tjdev.fr",
                         (char *) "1245398564792");
    if(result < 0)
        return result;

    result = usbMgr.searchForDevice();
    if(result < 0)
        return result;
    //result = usbMgr.setupAccessory(0x18D1, 0x4EE2);
    //if(result < 0)
    //    return result;

    result = usbMgr.connectToDevice();
    if(result < 0)
        return result;

    return EXIT_SUCCESS;
}
