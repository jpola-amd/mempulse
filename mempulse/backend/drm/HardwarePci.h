#pragma once

#include "DrmFileAmdgpu.h"
#include "DrmDevice.h"
#include "DrmAmdgpu.h"

#include <optional>

namespace mempulse {
class HardwarePci {
public:
	explicit HardwarePci(const char* path = "", short bus = -1);

	DrmDevice& GetDrmDevice() { return m_drmDevice; }
	DrmAmdgpu& GetDrmAmdgpu() { return m_drmAmdgpu; }

private:
	static DrmFileAmdgpu OpenDrmFile(const char* path, short bus);
	static std::optional<DrmFileAmdgpu> FindDrm(short bus = -1);

	DrmFileAmdgpu m_drmFile;
	DrmDevice m_drmDevice;
	DrmAmdgpu m_drmAmdgpu;


};
}