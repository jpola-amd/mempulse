#pragma once

#include "mempulse/backend/hip/DeviceHip.h"
#include "FileDrmAmdgpu.h"

namespace mempulse {

class BackendDrm;

class DeviceDrm : public DeviceHip {
public:
	explicit DeviceDrm(const BackendDrm&, int deviceId);

	MempulseDeviceMemoryInfo GetMemoryInfo() override;
    MempulseDeviceMemoryUsage GetMemoryUsage() override;
private:
	static FileDrmAmdgpu OpenDrmFile(int bus = -1, int deviceId = -1);

	FileDrmAmdgpu m_drmFile;
};

} // namespace mempulse
