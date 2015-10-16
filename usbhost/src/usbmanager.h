#ifndef USBMANAGER_H
#define USBMANAGER_H

#include <usb.h>
#include <libusb.h>

#include <string>

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
        result_code init(char* manufacturer,
                         char* modelName,
                         char* description,
                         char* version,
                         char* url,
                         char* serialNumber);

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

        // Constants for the AOA protocol
        static const int AOA_STRING_MANUFACTURER = 0;
        static const int AOA_STRING_MODEL = 1;
        static const int AOA_STRING_DESCRIPTION = 2;
        static const int AOA_STRING_VERSION = 3;
        static const int AOA_STRING_URL = 4;
        static const int AOA_STRING_SERIAL = 5;

        static const int AOA_COMMAND_GETPROTOCOL = 51;
        static const int AOA_COMMAND_SENDSTRING = 52;
        static const int AOA_COMMAND_START = 53;

    private:

        bool _errorOccured = false;

        libusb_device **_devicesList;

        struct libusb_device_handle* _deviceHandle = nullptr;
        uint8_t _endpointOut = 0;
        uint8_t _endpointIn = 0;

        char* _manufacturer;
        char* _modelName;
        char* _description;
        char* _version;
        char* _url;
        char* _serialNumber;

        // Used to get errors
        int _error = 0;

        //
        // Private methods

        void errorToString(const int& code);
        bool checkError(const int& error, const std::string& funcName = "");

        std::string deviceToStr(libusb_device *device, struct libusb_device_descriptor desc) const;

        // Tell the manager the refresh the list of devices
        result_code retrieveDevicesList();
        // Simple wrapper for libusb function that send accessory information
        result_code sendControlInfo(const int index, char* info);
        // Search available endpoints for the specified device
        result_code retrieveEndpoints(libusb_device *device);
};

#endif // USBMANAGER_H
