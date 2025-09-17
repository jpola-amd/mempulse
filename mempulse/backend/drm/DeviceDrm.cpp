#include "DeviceDrm.h"
#include "BackendDrm.h"
#include "mempulse/Logging.h"
#include "DrmDevices.h"
#include "Defines.h"

#include "DrmDevice.h"
#include "DrmAmdgpu.h"

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
	DrmDevices devs;

	for (size_t i = 0; i < devs.Size(); i++) {
		const drmDevicePtr& device = devs[i];
		if (!IsValid(device)) {
			MEMPULSE_LOG_DEBUG("ignore device");
			continue;
		}


		// dev is 0

		std::stringstream stream;
		stream << "----------------------------------\n";
		stream << "bus\t\t" << bus << "\n";
		stream << "deviceId\t" << deviceId << "\n";
		stream << "deviceinfo.pci->deviceId " << device->deviceinfo.pci->device_id << "\n";
		stream << "businfo.pci->dev " << static_cast<int>(device->businfo.pci->dev) << "\n";
		stream << "----------------------------------\n";

		std::cout << stream.str();

		if (device->deviceinfo.pci->vendor_id != VENDOR_AMD ||
			(bus >= 0 && bus != device->businfo.pci->bus)) {
			continue;
		}

		// TODO: use deviceId
		// try render node first, as it does not require to drop master
		for (int j = DRM_NODE_MAX - 1; j >= 0; j--) {
			if (!(1 << j & device->available_nodes))
				continue;

			if (deviceId >= 0 && deviceId != device->businfo.pci->dev) {
				continue;
			}

			try {
				return FileDrmAmdgpu(devs[i]->nodes[j]);
			} catch (std::exception &e) {
				MEMPULSE_LOG_DEBUG("probing for device failed");
				std::cerr << e.what() << std::endl;
			}
		}
	} // foreach devs

	throw std::runtime_error("can't create device");
}

} // namespace mempulse
