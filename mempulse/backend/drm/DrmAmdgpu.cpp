#include "DrmAmdgpu.h"

#include <stdexcept>
#include <cassert>

#define check(ret, msg) if (ret != 0) throw std::runtime_error(msg);

namespace mempulse {
	DrmAmdgpu::DrmAmdgpu(const DrmFileAmdgpu &file): m_drmFile(file) {
		if (!file.IsOpen())
			throw std::runtime_error("DrmDevice::DrmDevice: file is not open");

		int ret;
		ret = amdgpu_device_initialize(m_drmFile, &m_drmMajor, &m_drmMinor, &m_amdgpuDev);
		check(ret, "amdgpu_device_initialize");
	}

	DrmAmdgpu::~DrmAmdgpu() {
		if (!m_amdgpuDev)
			return;

		(void)amdgpu_device_deinitialize(m_amdgpuDev);
	}

	drm_amdgpu_info_vram_gtt DrmAmdgpu::QueryVramGtt() {
		assert(m_amdgpuDev);

		drm_amdgpu_info_vram_gtt vramGtt;

		int ret;

		ret = amdgpu_query_info(m_amdgpuDev, AMDGPU_INFO_VRAM_GTT,sizeof(vramGtt), &vramGtt);
		check(ret, "failed to get vram gtt")

		return vramGtt;
	}
	unsigned long long DrmAmdgpu::QueryGttUsage() {
		uint64_t gttUsage;
		int ret;

		ret = amdgpu_query_info(m_amdgpuDev, AMDGPU_INFO_GTT_USAGE,sizeof(gttUsage), &gttUsage);
		check(ret, "failed to get gtt usage")

		return gttUsage;
	}

	unsigned long long DrmAmdgpu::QueryVramUsage() {
		uint64_t vramUsage;
		int ret;

		ret = amdgpu_query_info(m_amdgpuDev, AMDGPU_INFO_VRAM_USAGE, sizeof(vramUsage), &vramUsage);
		check(ret, "failed to get vram usage")

		return vramUsage;
	}
}