#pragma once

#include "mempulse/Device.h"

namespace mempulse {

class BackendDrm;

class DeviceDrm : public Device {
public:
	explicit DeviceDrm(const BackendDrm&, int deviceId);

	MempulseDeviceMemoryInfo GetMemoryInfo() override;
    MempulseDeviceMemoryUsage GetMemoryUsage() override;

	std::string GetHardwareName() override;
private:
};

} // namespace mempulse
