#pragma once

#include "mempulse/Device.h"
#include "HipRuntime.h"
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

	const Luid& luid() const;
    bool IsIntegrated() const;

 	int PciBusID() const;
   	int PciDeviceID() const;
    int PciDomainID() const;

private:
	hipDeviceProp_t m_deviceProperties;
	Luid m_luid;
};

} // namespace mempulse
