#include "usbmanager.h"

#include "easylogging++.h"

#include <cstdio>

// Used to check error on libusb function
#define QUOTE_FUNC(func) #func

// Return and call the handler on error
#define CHECK_ERROR(func)                   \
_error = func;                              \
if(!checkError(_error, QUOTE_FUNC(func)))   \
    return _error;

/*
 * Static call back. The USBManager is passed with the user_data field.
 */
static void LIBUSB_CALL transferCallback(struct libusb_transfer *transfer)
{
    USBManager *manager = static_cast<USBManager *>(transfer->user_data);

    if(transfer->status != LIBUSB_TRANSFER_COMPLETED)
    {
        if(transfer->status == LIBUSB_TRANSFER_CANCELLED)
            libusb_free_transfer(transfer);
        else
        {
            manager->errorToString(transfer->status);
            manager->sendTransferErrorHandlerEvent();
        }
        return;
    }

    //ssize_t totalLength = 0;
    for(int i=0 ; i < transfer->num_iso_packets ; ++i)
    {
        struct libusb_iso_packet_descriptor *pack = &transfer->iso_packet_desc[i];
        //totalLength += pack->actual_length;
        manager->sendTransferNewDataHandlerEvent(libusb_get_iso_packet_buffer_simple(transfer, i), pack->actual_length);
    }

    //manager->sendTransferNewDataHandlerEvent(transfer->buffer, totalLength);

    // Now resubmit the transfer
    int error = libusb_submit_transfer(transfer);
    if(error < 0)
    {
        manager->errorToString(error);
        manager->sendTransferErrorHandlerEvent();
    }
}

USBManager::USBManager()
{

}

USBManager::~USBManager()
{
    uninit();
}

// Public methods

void USBManager::uninit()
{
    if(_unintialized)
        return;

    if(_transferObj != nullptr)
    {
        if(_transferObj->status == LIBUSB_TRANSFER_COMPLETED)
            libusb_free_transfer(_transferObj);
        else
            libusb_cancel_transfer(_transferObj);
    }

    if(_deviceHandle != nullptr)
    {
        if(_transferObj != nullptr)
        {
            while(_transferObj->status != LIBUSB_TRANSFER_CANCELLED)
            {
            }
        }

        libusb_release_interface(_deviceHandle, 0);
        libusb_attach_kernel_driver(_deviceHandle, 0);
        libusb_close(_deviceHandle);
        _deviceHandle = nullptr;
    }

    // Uninitialize libusb
    libusb_exit(nullptr);

    _unintialized = true;
    LOG(INFO) << "USBManager unitialized !";
}

result_code USBManager::init(char *description, char *version,
                             char *url, char *serialNumber)
{
    _description = description;
    _version = version;
    _url = url;
    _serialNumber = serialNumber;

    CHECK_ERROR(libusb_init(nullptr));

    // Set log level
#ifdef DEBUG_BUILD
    libusb_set_debug(nullptr, LIBUSB_LOG_LEVEL_INFO);
#else
    libusb_set_debug(nullptr, LIBUSB_LOG_LEVEL_WARNING);
#endif

    LOG(INFO) << "USBManager initialized !";

    return USBMANAGER_SUCCESS;
}

result_code USBManager::searchForDevice()
{
    // List all connected devices
    CHECK_ERROR(retrieveDevicesList());

    libusb_device *dev;
    int i = 0;

    // First, search for google ACCESSORY_VID and ACCESSORY_PID
    while ((dev = _devicesList[i++]) != nullptr)
    {
        // Get the descriptor
        struct libusb_device_descriptor desc;
        // Only continue if success
        _error = libusb_get_device_descriptor(dev, &desc);
        if(checkError(_error, "libusb_get_device_descriptor"))
        {
            LOG(INFO) << deviceToStr(dev, desc);

            if(desc.idVendor == ACCESSORY_VID && (desc.idProduct == ACCESSORY_AUDIO_PID
                                                  || desc.idProduct == ACCESSORY_AUDIO_PID_DEBUG
                                                  || desc.idProduct == AUDIO_PID
                                                  || desc.idProduct == AUDIO_PID_DEBUG))
            {
                LOG(INFO) << "Found a device in accessory mode !";
                libusb_free_device_list(_devicesList, 1);
                return USBMANAGER_SUCCESS;
            }
        }
    }

    // Now, try to switch each device into ACCESSORY_MODE
    i = 0;
    while ((dev = _devicesList[i++]) != nullptr)
    {
        // Get the descriptor
        struct libusb_device_descriptor desc;
        // Only continue if success
        _error = libusb_get_device_descriptor(dev, &desc);
        if(checkError(_error, "libusb_get_device_descriptor"))
        {
            if(setupAccessory(desc.idVendor, desc.idProduct) >= 0)
            {
                libusb_free_device_list(_devicesList, 1);
                return USBMANAGER_SUCCESS;
            }

            // Reset handle for the next iteration
            if(_deviceHandle != nullptr)
            {
                libusb_release_interface(_deviceHandle, 0);
                libusb_attach_kernel_driver(_deviceHandle, 0);
                libusb_close(_deviceHandle);
                _deviceHandle = nullptr;
            }
        }
    }

    libusb_free_device_list(_devicesList, 1);

    // Here, no device support the accessory mode, return failure
    LOG(ERROR) << "No devices support the accessory mode. Please connect an Android 4.1+ phone.";
    return USBMANAGER_NO_DEVICES_COMPATIBLE;
}

result_code USBManager::setupAccessory(const uint16_t& vid, const uint16_t pid)
{
    char infoBuffer[9];
    std::sprintf(infoBuffer, "%04x:%04x", vid, pid);
    LOG(INFO) << "Trying to setup the device:" << infoBuffer << "...";

    // Open the device
    if((_deviceHandle = libusb_open_device_with_vid_pid(nullptr, vid, pid)) == nullptr)
    {
        LOG(ERROR) << "Cannot retrieve handle !";
        return USBMANAGER_CANT_RETRIEVE_HANDLE;
    }

    libusb_set_auto_detach_kernel_driver(_deviceHandle, 1);
    libusb_detach_kernel_driver(_deviceHandle, 0);

    // Don't exit for LIBUSB_ERROR_BUSY
    _error = libusb_claim_interface(_deviceHandle, 0);
    if(_error != LIBUSB_ERROR_BUSY && !checkError(_error, "libusb_claim_interface()"))
        return _error;

    // 16 bits buffer for the protocol version
    unsigned char ioBuffer[2];

    // Send GET_PROTOCOL (0x51) request to figure out whether the device supports ADK
    // If protocol is not 0 the device supports it
    CHECK_ERROR(libusb_control_transfer(
                    _deviceHandle,
                    USB_ENDPOINT_IN | USB_TYPE_VENDOR, // bmRequestType
                    AOA_COMMAND_GETPROTOCOL, // bRequest
                    0,        // wValue
                    0,        // wIndex
                    ioBuffer, // data
                    2,        // wLength
                    0         // timeout
                    ));

    // Extract protocol version to check whether Accessory mode is supported
    const uint16_t devVersion = ioBuffer[1] << 8 | ioBuffer[0];
    if(devVersion == 2)
    {
        LOG(INFO) << "Device support AOA protocol version" << devVersion;
    }
    else
    {
        LOG(ERROR) << "The device does not support the AOA protocol v2 !";
        return USBMANAGER_NO_AOAP_SUPPORT;
    }

    // sometimes hangs on the next trasnfer, so wait !
    usleep(10000);

    // Accessory mode is supported: send out IDs
    // Don't send manufacturer and model name if you want no popup on connection
    CHECK_ERROR(sendControlInfo(AOA_STRING_DESCRIPTION, _description));
    usleep(1000);
    CHECK_ERROR(sendControlInfo(AOA_STRING_VERSION, _version));
    usleep(1000);
    CHECK_ERROR(sendControlInfo(AOA_STRING_URL, _url));
    usleep(1000);
    CHECK_ERROR(sendControlInfo(AOA_STRING_SERIAL, _serialNumber));
    usleep(1000);

    LOG(INFO) << "Accessory Identification sent";

    // Set the audio mode
    // See https://source.android.com/accessories/aoa2.html#audio-support
    CHECK_ERROR(libusb_control_transfer(_deviceHandle, USB_ENDPOINT_OUT | USB_TYPE_VENDOR,
                                        AOA_COMMAND_SETAUDIOMODE, 1, 0, nullptr, 0, 0));

    LOG(INFO) << "Audio mode sent";

    // Request device to start up in accessory mode
    CHECK_ERROR(libusb_control_transfer(_deviceHandle, USB_ENDPOINT_OUT | USB_TYPE_VENDOR,
                                        AOA_COMMAND_START, 0, 0, nullptr, 0, 0));

    // Close phone handles as we need to connect to Accessory device in a while
    if(_deviceHandle != nullptr){
        libusb_release_interface(_deviceHandle, 0);
        libusb_attach_kernel_driver(_deviceHandle, 0);
        libusb_close(_deviceHandle);
        _deviceHandle = nullptr;
    }

    // Wait to let the device enable the accessory mode
    usleep(500000); // 500 ms

    LOG(INFO) << "This device fully support the AOA protocol !";
    return USBMANAGER_SUCCESS;
}

result_code USBManager::connectToDevice()
{
    bool connected = false;
    for(int i=0; i<3; ++i)
    {
        if((_deviceHandle = libusb_open_device_with_vid_pid(nullptr, ACCESSORY_VID, AUDIO_PID)) != nullptr)
        {
            connected = true;
            break;
        }

        // Wait
        usleep(500000); // 500 ms
    }

    // If connection does not work, try with the debug PID
    if(!connected)
    {
        for(int i=0; i<3; ++i)
        {
            if((_deviceHandle = libusb_open_device_with_vid_pid(nullptr, ACCESSORY_VID, AUDIO_PID_DEBUG)) != nullptr)
            {
                connected = true;
                break;
            }

            // Wait
            usleep(500000); // 500 ms
        }
    }

    if(!connected)
    {
        LOG(ERROR) << "Can't connect to the device.";
        return USBMANAGER_CONNECTION_FAILED;
    }

    libusb_set_auto_detach_kernel_driver(_deviceHandle, 1);
    libusb_detach_kernel_driver(_deviceHandle, 0);

    // Wait
    usleep(500000); // 500 ms

    // Get endpoints
    CHECK_ERROR(retrieveEndpoints(libusb_get_device(_deviceHandle)));

    // Set configuration to 1 as specified in AOA protocol
    //CHECK_ERROR(libusb_set_configuration(_deviceHandle, 1));

    // TODO: Useful ?
    CHECK_ERROR(libusb_claim_interface(_deviceHandle, 0));
    CHECK_ERROR(libusb_claim_interface(_deviceHandle, _interfaceNumber));
    CHECK_ERROR(libusb_set_interface_alt_setting(_deviceHandle, _interfaceNumber, _interfaceAltSettingNumber));

    LOG(INFO) << "Interface claimed, ready to transfer data";

    return USBMANAGER_SUCCESS;
}

result_code USBManager::startReading(std::function<void()> onErrorHandler, std::function<void (unsigned char *, ssize_t)> onNewDataHandler)
{
    _onTransferErrorHandler = onErrorHandler;
    _onTransferNewDataHandler = onNewDataHandler;

    _transferObj = libusb_alloc_transfer(ISO_NUM_PACKETS);
    if(!_transferObj)
    {
        errorToString(LIBUSB_ERROR_NO_MEM);
        return LIBUSB_ERROR_NO_MEM;
    }

    int maxPackSize = libusb_get_max_packet_size(libusb_get_device(_deviceHandle), _endpointIn);
    if(maxPackSize <= 0)
    {
        LOG(ERROR) << "Cannot get max packet size";
        checkError(maxPackSize, "libusb_get_max_packet_size()");
        return maxPackSize == 0 ? -200 : maxPackSize;
    }

    LOG(INFO) << "Max packet size:" << maxPackSize;

    libusb_fill_iso_transfer(_transferObj, _deviceHandle, _endpointIn,
                             (unsigned char*)malloc(ISO_NUM_PACKETS*maxPackSize), ISO_NUM_PACKETS*maxPackSize,
                             ISO_NUM_PACKETS, transferCallback, this, 0);

    libusb_set_iso_packet_lengths(_transferObj, maxPackSize);

    CHECK_ERROR(libusb_submit_transfer(_transferObj));
    LOG(INFO) << "Start reading form the phone !";
    return LIBUSB_SUCCESS;
}

result_code USBManager::handleEvents()
{
    CHECK_ERROR(libusb_handle_events(nullptr));
    return LIBUSB_SUCCESS;
}

void USBManager::errorToString(const int& code)
{
    LOG(ERROR) << "Error:" << libusb_error_name(code) << "/" << libusb_strerror(static_cast<libusb_error>(code));
}

void USBManager::sendTransferErrorHandlerEvent()
{
    _onTransferErrorHandler();
}

void USBManager::sendTransferNewDataHandlerEvent(unsigned char *data, ssize_t length)
{
    _onTransferNewDataHandler(data, length);
}

//
// Private methods
//

std::string USBManager::deviceToStr(libusb_device *device, struct libusb_device_descriptor desc) const
{
    char buffer[50];
    std::sprintf(buffer, "%04x:%04x (bus %03d, device %03d)",
        desc.idVendor, desc.idProduct,
        libusb_get_bus_number(device), libusb_get_device_address(device));
    return std::string(buffer);
}

result_code USBManager::sendControlInfo(const int index, char* info)
{
    return libusb_control_transfer(_deviceHandle,
                                   USB_ENDPOINT_OUT | USB_TYPE_VENDOR,
                                   AOA_COMMAND_SENDSTRING,
                                   0,
                                   index,
                                   (unsigned char*)info,
                                   // TODO: +1 needed in data length ?
                                   strlen(info),
                                   0);
}

result_code USBManager::retrieveDevicesList()
{
    const ssize_t devicesCount = libusb_get_device_list(nullptr, &_devicesList);
    if(!checkError((int) devicesCount, "libusb_get_device_list()"))
        return (int) devicesCount;

    // Check for no devices
    if(devicesCount == 0)
    {
        LOG(ERROR) << "No devices connected !";
        return USBMANAGER_NO_DEVICES_CONNECTED;
    }
    return USBMANAGER_SUCCESS;
}

result_code USBManager::retrieveEndpoints(libusb_device *device)
{
    // Reset endpoints
    _endpointIn = 0;
    _endpointOut = 0;
    _interfaceNumber = -1;
    _interfaceAltSettingNumber = -1;

    libusb_config_descriptor* configDesc;
    CHECK_ERROR(libusb_get_config_descriptor(device, 0, &configDesc));

    bool found = false;

    for(int i=0; i < (int)configDesc->bNumInterfaces; ++i)
    {
        const libusb_interface* interface = &configDesc->interface[i];
        for(int j=0; j < interface->num_altsetting; ++j)
        {
            const libusb_interface_descriptor* interfaceDesc = &interface->altsetting[j];
            if(interfaceDesc->bInterfaceClass == LIBUSB_CLASS_AUDIO
               && interfaceDesc->bInterfaceSubClass == 2 // Streaming sub-class
               && interfaceDesc->bNumEndpoints > 0
               && !found)
            {
                for(int k=0; k < (int)interfaceDesc->bNumEndpoints; ++k)
                {
                    const libusb_endpoint_descriptor* endpointDesc = &interfaceDesc->endpoint[k];
                    if(endpointDesc->bDescriptorType == LIBUSB_DT_ENDPOINT
                       && endpointDesc->bEndpointAddress & LIBUSB_ENDPOINT_IN
                       && endpointDesc->bmAttributes & LIBUSB_TRANSFER_TYPE_ISOCHRONOUS
                       && !found)
                    {
                        // Here, the correct endpoint / interface has been found
                        _interfaceNumber = interfaceDesc->bInterfaceNumber;
                        _interfaceAltSettingNumber = interfaceDesc->bAlternateSetting;
                        _endpointIn = endpointDesc->bEndpointAddress;
                        found = true;
                    }
                }
            }
        }
    }

    if(!found)
    {
        LOG(INFO) << "No endpoints available for this device !";
        return USBMANAGER_NO_ENDPOINTS;
    }

    LOG(INFO) << "Available endpoints: IN:" << _endpointIn << "INTERFACE:" << _interfaceNumber << "ALTSETTING:" << _interfaceAltSettingNumber;
    return USBMANAGER_SUCCESS;
}

bool USBManager::checkError(const int& error, const std::string& funcName)
{
    if(error < 0)
    {
        if(funcName != "")
            LOG(ERROR) << "In function" << funcName << ":";

        errorToString(error);
        return false;
    }
    return true;
}

