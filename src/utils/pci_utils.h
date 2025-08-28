#pragma once

#include "mempulse/config.h"
#include <Windows.h>
#include <string>
#include <vector>



namespace mempulse {
namespace pci_utils {

struct PciDeviceInfo {
    char deviceName[MEMPULSE_MAX_DEVICE_NAME_LENGTH] = {0};
    int deviceId {-1};
    int vendorId {-1};
    DWORD busNumber {0};

    bool isValid() const {
        return deviceId >= 0 && vendorId >= 0 && busNumber != 0;
    }
};

    // Function to get PCI display devices
    // This function retrieves all display devices and their PCI information
    // The most important information is the bus number, vendor ID, and device ID which we then match against the DXGI adapter
std::vector<PciDeviceInfo> GetPciDisplayDevices();

} // namespace pci_utils
} // namespace mempulse



