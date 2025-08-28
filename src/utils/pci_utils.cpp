
#include "pci_utils.h"
#include <regex>
#include <SetupAPI.h>
#include <devguid.h>
#include <iostream>
#include <cassert>
#define assert_with_message(condition, message) \
    ((condition) ? static_cast<void>(0) : [&]{ std::cerr << "Assertion failed: " << message << std::endl; assert(false); }())



#pragma comment(lib, "setupapi.lib")

namespace mempulse {
namespace pci_utils {
    static bool ParseHardwareId(const std::string& hardwareId, int& vendorId, int& deviceId) {
        
        static const std::regex regex1(R"(PCI\\VEN_(\w{4})&DEV_(\w{4})&SUBSYS_\w{8}&REV_(\w{2}))");
        static const std::regex regex2(R"(PCI\\VEN_(\w{4})&DEV_(\w{4})&SUBSYS_\w{8})");
        static const std::regex regex3(R"(PCI\\VEN_(\w{4})&DEV_(\w{4})&REV_(\w{2}))");
        static const std::regex regex4(R"(PCI\\VEN_(\w{4})&DEV_(\w{4}))");
        static const std::regex regex5(R"(PCI\\VEN_(\w{4})&DEV_(\w{4})&CC_\w{6})");
        static const std::regex regex6(R"(PCI\\VEN_(\w{4})&DEV_(\w{4})&CC_\w{4})");
        
        std::smatch match;
        std::string vendorIDStr, deviceIDStr, revisionIDStr;

        if (std::regex_match(hardwareId, match, regex1) ||
            std::regex_match(hardwareId, match, regex2) ||
            std::regex_match(hardwareId, match, regex3) ||
            std::regex_match(hardwareId, match, regex4) ||
            std::regex_match(hardwareId, match, regex5) ||
            std::regex_match(hardwareId, match, regex6)) {
            
            vendorIDStr = match[1];
            deviceIDStr = match[2];
            
            // Convert hex strings to integers
            vendorId = std::stoi(vendorIDStr, nullptr, 16);
            deviceId = std::stoi(deviceIDStr, nullptr, 16);
            return true;
        
        } else {
            return false;
        }
    }


    std::vector<PciDeviceInfo> GetPciDisplayDevices() {
        std::vector<PciDeviceInfo> devices;

        HDEVINFO deviceInfoSet = SetupDiGetClassDevs(  
            &GUID_DEVCLASS_DISPLAY, // Class GUID for display adapters  
            nullptr,  
            nullptr,  
            DIGCF_PRESENT  
        ); 
        
        if (deviceInfoSet == INVALID_HANDLE_VALUE) {
            return devices; // No devices found
        }
        
        SP_DEVINFO_DATA deviceInfoData;
        deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
        
        for (DWORD i = 0; SetupDiEnumDeviceInfo(deviceInfoSet, i, &deviceInfoData); ++i) {
            PciDeviceInfo deviceInfo;
            DWORD size; 
            // Get the device description
            if (!SetupDiGetDeviceRegistryProperty(deviceInfoSet, &deviceInfoData, SPDRP_DEVICEDESC, NULL, (PBYTE)deviceInfo.deviceName, sizeof(deviceInfo.deviceName), &size)) 
            {
                continue;
            }
            // Get the bus number
            if (!SetupDiGetDeviceRegistryProperty(deviceInfoSet, &deviceInfoData, SPDRP_BUSNUMBER, nullptr, (PBYTE)&deviceInfo.busNumber, sizeof(deviceInfo.busNumber), &size) )
            {
                continue;
            }

            // Get the hardware ID
            char deviceHardwareID[MEMPULSE_MAX_DEVICE_NAME_LENGTH];
            if (!SetupDiGetDeviceRegistryProperty(deviceInfoSet, &deviceInfoData, SPDRP_HARDWAREID, NULL, (PBYTE)deviceHardwareID, sizeof(deviceHardwareID), &size))
            {
                continue;
            }

            if (!ParseHardwareId(deviceHardwareID, deviceInfo.vendorId, deviceInfo.deviceId)) {
                continue; // Skip if parsing failed
            }

            if (deviceInfo.isValid()) {
                devices.push_back(deviceInfo);
            }
        }
        
        auto result = SetupDiDestroyDeviceInfoList(deviceInfoSet);
        if (!result) {
            // Handle error if needed
            DWORD error = GetLastError();
            std::string errorMessage = "SetupDiDestroyDeviceInfoList failed with error: " + std::to_string(error);
            assert_with_message(false, errorMessage);
        }

        return devices;
    }

} // namespace pci_utils
} // namespace mempulse


