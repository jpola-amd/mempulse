#pragma once

#include "mempulse/backend/hip/DeviceHip.h"

namespace mempulse {

class BackendD3dkmt;

class DeviceD3dkmt : public DeviceHip {
public:
    explicit DeviceD3dkmt(const BackendD3dkmt& context, int deviceId);

    MempulseDeviceMemoryInfo GetMemoryInfo() override;
    MempulseDeviceMemoryUsage GetMemoryUsage() override;
};

}  // namespace mempulse
