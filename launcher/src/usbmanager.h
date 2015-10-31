#ifndef USBMANAGER_H
#define USBMANAGER_H

#include <usb.h>
#include <libusb.h>

#include <string>
#include <functional>

#include <lib/Thread.h>

typedef int result_code;

// Manage all usb stuff
// For the communication part, follow the Android Open Accessory Protocol specified at:
// http://source.android.com/accessories/aoa.html
class USBManager
{
    public:
        USBManager();
        ~USBManager();

        // Init the manager, must be called before all others methods
        // Call this method ONLY ONE time
        result_code init(char* description,
                         char* version,
                         char* url,
                         char* serialNumber);

        void uninit();

        // Scan all devices and try to find a compatible device for the ACCESSORY mode
        result_code searchForDevice();

        /*
         * Attempts to put USB device in Accessory mode.
         * On success, PID/VID switches to accessory PID/VID
         * and proper application is launched. Application must match
         * Accessory manufacturer, model and version which are defined in
         * "android.hardware.us.action.USB_ACCESSORY_ATTACHED" meta-data
         */
        result_code setupAccessory(const uint16_t& vid, const uint16_t pid);

        // This function will try to connect to a device in accessory mode.
        // You should call this function only if searchForDevice() succeed.
        result_code connectToDevice();

        result_code startReading(std::function<void()> onErrorHandler, std::function<void(unsigned char* /*newBuffer*/, ssize_t /*length*/)> onNewDataHandler);
        result_code handleEvents();

        void errorToString(const int& code);

        void sendTransferErrorHandlerEvent();
        void sendTransferNewDataHandlerEvent(unsigned char* data, ssize_t length);

        enum ManagerError {
            USBMANAGER_SUCCESS = 0,
            USBMANAGER_ALREADY_INIT = -100,
            USBMANAGER_NO_DEVICES_CONNECTED = -101,
            USBMANAGER_NO_DEVICES_COMPATIBLE = -102,
            USBMANAGER_CANT_RETRIEVE_HANDLE = -103,
            USBMANAGER_NO_AOAP_SUPPORT = -104,
            USBMANAGER_CONNECTION_FAILED = -105,
            USBMANAGER_NO_ENDPOINTS = -106
        };

        // Google Accessory IDs
        static const int ACCESSORY_VID = 0x18D1;
        static const int ACCESSORY_PID = 0x2D00;
        static const int ACCESSORY_PID_DEBUG = 0x2D01; // Used if adb is enabled by the phone
        static const int AUDIO_PID = 0x2D02;
        static const int AUDIO_PID_DEBUG = 0x2D03;
        static const int ACCESSORY_AUDIO_PID = 0x2D04;
        static const int ACCESSORY_AUDIO_PID_DEBUG = 0x2D05;

        // Constants for the AOA protocol
        static const int AOA_STRING_MANUFACTURER = 0;
        static const int AOA_STRING_MODEL = 1;
        static const int AOA_STRING_DESCRIPTION = 2;
        static const int AOA_STRING_VERSION = 3;
        static const int AOA_STRING_URL = 4;
        static const int AOA_STRING_SERIAL = 5;

        static const int AOA_COMMAND_GETPROTOCOL = 51;
        static const int AOA_COMMAND_SENDSTRING = 52;
        static const int AOA_COMMAND_SETAUDIOMODE = 58;
        static const int AOA_COMMAND_START = 53;

        static const int AOA_HID_REGISTER = 54;
        static const int AOA_HID_UNREGISTER = 55;
        static const int AOA_HID_SETREPORTDESC = 56;
        static const int AOA_HID_SENDEVENT = 57;

        //static const int ISO_BUFFER_LENGTH = 256;
        static const int ISO_NUM_PACKETS = 400;


    private:

        bool _unintialized = false;

        libusb_device **_devicesList;

        struct libusb_device_handle* _deviceHandle = nullptr;

        uint8_t _endpointOut = 0;
        uint8_t _endpointIn = 0;
        int _interfaceNumber = 0;
        int _interfaceAltSettingNumber = 0;

        char* _description;
        char* _version;
        char* _url;
        char* _serialNumber;

        struct libusb_transfer *_transferObj = nullptr;

        std::function<void()> _onTransferErrorHandler = [](){};
        std::function<void(unsigned char*, ssize_t)> _onTransferNewDataHandler = [](unsigned char*, ssize_t){};

        // Used to get errors
        int _error = 0;

        //
        // Private methods

        bool checkError(const int& error, const std::string& funcName = "");

        std::string deviceToStr(libusb_device *device, struct libusb_device_descriptor desc) const;

        // Tell the manager the refresh the list of devices
        result_code retrieveDevicesList();
        // Simple wrapper for libusb function that send accessory information
        result_code sendControlInfo(const int index, char* info);
        // Search available endpoints for the specified device
        result_code retrieveEndpoints(libusb_device *device);
};

class USBManagerThread : public ilixi::Thread
{
    private:
        bool _success = false;
        bool _finished = false;
        USBManager *_mgr;
        std::function<void(void)> _onErrorHandler;
        std::function<void(unsigned char*, ssize_t)> _onNewDataHandler;

    public:
        USBManagerThread(USBManager *manager, std::function<void(void)> onError, std::function<void(unsigned char*, ssize_t)> onNewData) : ilixi::Thread()
        {
            _mgr = manager;
            _onErrorHandler = onError;
            _onNewDataHandler = onNewData;
        }

        virtual int run() {
            if(_mgr->searchForDevice() >= 0)
            {
                if(_mgr->connectToDevice() >= 0)
                {
                    if(_mgr->startReading(_onErrorHandler, _onNewDataHandler) >= 0)
                        _success = true;
                }
            }
            _finished = true;
        }

        bool succeed() const
        {
            return _success;
        }

        bool finished() const
        {
            return _finished;
        }
};

#endif // USBMANAGER_H
