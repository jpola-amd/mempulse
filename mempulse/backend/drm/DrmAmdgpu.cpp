#include "DrmAmdgpu.h"
#include "ErrorDrm.h"

#include "mempulse/Logging.h"
#include "mempulse/Assert.h"

namespace mempulse {

DrmAmdgpu::DrmAmdgpu(const FileDrmAmdgpu &file):
 m_drmFile(file) {
	MEMPULSE_LOG_TRACE();

	if (!file.IsOpen())
		throw ErrorDrm("DrmDevice::DrmDevice: file is not open");

	int ret;
	ret = amdgpu_device_initialize(m_drmFile, &m_drmMajor, &m_drmMinor, &m_amdgpuDev);
	check_drm(ret, "amdgpu_device_initialize");
}

DrmAmdgpu::~DrmAmdgpu() {
	MEMPULSE_LOG_TRACE();

	if (!m_amdgpuDev)
		return;

	(void)amdgpu_device_deinitialize(m_amdgpuDev);
}

drm_amdgpu_info_vram_gtt DrmAmdgpu::QueryVramGtt() {
	MEMPULSE_LOG_TRACE();

	assert_with_message(m_amdgpuDev, "m_amdgpuDev is nullptr");

	drm_amdgpu_info_vram_gtt vramGtt;

	int ret;

	ret = amdgpu_query_info(m_amdgpuDev, AMDGPU_INFO_VRAM_GTT,sizeof(vramGtt), &vramGtt);
	check_drm(ret, "failed to get vram gtt");

	return vramGtt;
}
unsigned long long DrmAmdgpu::QueryGttUsage() {
	MEMPULSE_LOG_TRACE();

	assert_with_message(m_amdgpuDev, "m_amdgpuDev is nullptr");

	uint64_t gttUsage;
	int ret;

	ret = amdgpu_query_info(m_amdgpuDev, AMDGPU_INFO_GTT_USAGE,sizeof(gttUsage), &gttUsage);
	check_drm(ret, "failed to get gtt usage");

	return gttUsage;
}

unsigned long long DrmAmdgpu::QueryVramUsage() {
	MEMPULSE_LOG_TRACE();

	assert_with_message(m_amdgpuDev, "m_amdgpuDev is nullptr");

	uint64_t vramUsage;
	int ret;

	ret = amdgpu_query_info(m_amdgpuDev, AMDGPU_INFO_VRAM_USAGE, sizeof(vramUsage), &vramUsage);
	check_drm(ret, "failed to get vram usage");

	return vramUsage;
}

} // namespace mempulse
