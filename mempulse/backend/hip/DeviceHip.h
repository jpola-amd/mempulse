#pragma once

#include "mempulse/Device.h"
#include <hip/hip_runtime.h>

namespace mempulse {

class LibraryContextHip;

class DeviceHip : public Device {
public:
	explicit DeviceHip(const LibraryContextHip&, int deviceId);

	MempulseDeviceMemoryInfo GetMemoryInfo() override;
    MempulseDeviceMemoryUsage GetMemoryUsage() override;

	std::string GetHardwareName() override;
private:
	hipDeviceProp_t m_deviceProperties;
};

} // namespace mempulse
