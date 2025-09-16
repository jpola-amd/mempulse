#pragma once

#include "mempulse/Device.h"
#include "DrmDevice.h"
#include "DrmAmdgpu.h"

namespace mempulse {

class BackendDrm;

class DeviceDrm : public Device {
public:
	explicit DeviceDrm(const BackendDrm&, int deviceId);

	MempulseDeviceMemoryInfo GetMemoryInfo() override;
    MempulseDeviceMemoryUsage GetMemoryUsage() override;

	std::string GetHardwareName() override;
private:
	static DrmFileAmdgpu OpenDrmFile(short bus = -1);

	DrmFileAmdgpu m_drmFile;

    DrmDevice m_drmDevice;
    DrmAmdgpu m_drmAmdgpu;


};

} // namespace mempulse
