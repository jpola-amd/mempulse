#include "DeviceDrm.h"
#include "Defines.h"
#include "BackendDrm.h"
#include "DrmDevices.h"
#include "DrmDevice.h"
#include "DrmAmdgpu.h"
#include "ErrorDrm.h"

#include "mempulse/Logging.h"

#include <sstream>

namespace mempulse {

namespace {
static bool IsValid(drmDevicePtr devicePtr) {
	if (!devicePtr) {
		MEMPULSE_LOG_ERROR("device is nullptr. Unexpected drm api behaviour");
		return false;
	}

	if (devicePtr->bustype != DRM_BUS_PCI) {
		MEMPULSE_LOG_DEBUG("device is not on PCI bus. Ignore");
		return false;
	}

	if (!devicePtr->deviceinfo.pci) {
		MEMPULSE_LOG_ERROR("device->deviceinfo.pci is nullptr. Unexpected drm api behaviour");
		return false;
	}

	return true;
}

} // namespace anonumous

DeviceDrm::DeviceDrm(const BackendDrm& backend, int deviceId)
: DeviceHip(backend, deviceId),
  m_drmFile(OpenDrmFile(PciBusID(), PciDeviceID()))
{
	MEMPULSE_LOG_TRACE();
	MEMPULSE_LOG_DEBUG("successfully init device drm device " + std::to_string(deviceId));
}

MempulseDeviceMemoryInfo DeviceDrm::GetMemoryInfo()
{
	MEMPULSE_LOG_TRACE();

	DrmAmdgpu drmAmdgpu(m_drmFile);
	const auto gtt = drmAmdgpu.QueryVramGtt();

	return {
		.dedicatedUsed = drmAmdgpu.QueryVramUsage(),
		.dedicatedTotal = gtt.vram_size,
		.sharedUsed = drmAmdgpu.QueryGttUsage(),
		.sharedTotal = gtt.gtt_size,
	};
}

MempulseDeviceMemoryUsage DeviceDrm::GetMemoryUsage()
{
	MEMPULSE_LOG_TRACE();

	MempulseDeviceMemoryInfo memInfo = GetMemoryInfo();

	return {
		.free = memInfo.dedicatedTotal - memInfo.dedicatedUsed,
		.total = memInfo.dedicatedTotal,
	};
}

FileDrmAmdgpu DeviceDrm::OpenDrmFile(int bus, int deviceId)
{
	MEMPULSE_LOG_TRACE();

	DrmDevices devs;

	for (size_t i = 0; i < devs.Size(); i++) {
		const drmDevicePtr& device = devs[i];
		if (!IsValid(device)) {
			MEMPULSE_LOG_DEBUG("ignore device");
			continue;
		}

		if (device->deviceinfo.pci->vendor_id != VENDOR_AMD) {
			MEMPULSE_LOG_DEBUG("device vendor is not amdgpu");
			continue;
		}

		if (bus >= 0 && bus != device->businfo.pci->bus) {
			MEMPULSE_LOG_DEBUG("device filtered by busId");
			continue;
		}

		// try render node first, as it does not require to drop master
		for (int j = DRM_NODE_MAX - 1; j >= 0; j--) {
			if (!(1 << j & device->available_nodes))
				continue;

			if (deviceId >= 0 && deviceId != device->businfo.pci->dev) {
				MEMPULSE_LOG_DEBUG("device filtered by device id");
				continue;
			}

			try {
				return FileDrmAmdgpu(device->nodes[j]);
			} catch (std::exception &e) {
				MEMPULSE_LOG_DEBUG("probing for device failed");

				std::stringstream msg;
				msg << "can't create file drm :\t" << e.what();
				MEMPULSE_LOG_DEBUG(e.what());
			}
		}
	} // foreach devs

	throw ErrorDrm("can't open drm device " + std::to_string(deviceId));
}

} // namespace mempulse
