#pragma once

#include "mempulse/Device.h"
#include <hip/hip_runtime.h>
#include <array>

namespace mempulse {

class BackendHip;

class DeviceHip : public Device {
public:
	using Luid = std::array<char, 8>;

	explicit DeviceHip(const BackendHip&, int deviceId);

	MempulseDeviceMemoryInfo GetMemoryInfo() override;
    MempulseDeviceMemoryUsage GetMemoryUsage() override;
	std::string GetHardwareName() override;

	Luid luid() const;
    bool IsIntegrated() const;

private:
	hipDeviceProp_t m_deviceProperties;
};

} // namespace mempulse
