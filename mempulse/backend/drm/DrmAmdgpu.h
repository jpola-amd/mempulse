#pragma once

#include <libdrm/amdgpu.h>
#include <libdrm/amdgpu_drm.h>
#include "DrmFileAmdgpu.h"

namespace mempulse {
class DrmAmdgpu {
public:
	explicit DrmAmdgpu(const DrmFileAmdgpu& file);
	~DrmAmdgpu();

	drm_amdgpu_info_vram_gtt QueryVramGtt();
	unsigned long long QueryGttUsage();
	unsigned long long QueryVramUsage();

	DrmAmdgpu(const DrmAmdgpu&) = delete;
	DrmAmdgpu& operator=(const DrmAmdgpu&) = delete;
private:
	DrmFileAmdgpu m_drmFile;
	amdgpu_device_handle m_amdgpuDev;
	uint32_t m_drmMajor;
	uint32_t m_drmMinor;
};
}
