#include "DeviceDrm.h"
#include "BackendDrm.h"
#include "mempulse/Logging.h"
#include "DrmDevices.h"
#include "Defines.h"

namespace mempulse {

DeviceDrm::DeviceDrm(const BackendDrm&, int deviceId)
: Device(deviceId),
  m_drmFile(OpenDrmFile()),
  m_drmDevice(m_drmFile),
  m_drmAmdgpu(m_drmFile)
{
	MEMPULSE_LOG_TRACE();
}

MempulseDeviceMemoryInfo DeviceDrm::GetMemoryInfo()
{
	MEMPULSE_LOG_TRACE();
	const auto gtt = m_drmAmdgpu.QueryVramGtt();

	return {
		.dedicatedUsed = m_drmAmdgpu.QueryVramUsage(),
		.dedicatedTotal = gtt.vram_size,
		.sharedUsed = m_drmAmdgpu.QueryGttUsage(),
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

std::string DeviceDrm::GetHardwareName()
{
	MEMPULSE_LOG_TRACE();

	return "amdgpu";
}

DrmFileAmdgpu DeviceDrm::OpenDrmFile(short bus)
{
	DrmDevices devs;

    for (size_t i = 0; i < devs.Size(); i++) {
        if (devs[i]->bustype != DRM_BUS_PCI ||
                devs[i]->deviceinfo.pci->vendor_id != VENDOR_AMD ||
                (bus >= 0 && bus != devs[i]->businfo.pci->bus))
                continue;

		// TODO: use deviceId
        // try render node first, as it does not require to drop master
        for (int j = DRM_NODE_MAX - 1; j >= 0; j--) {
            if (!(1 << j & devs[i]->available_nodes))
                    continue;
            try {
                    return DrmFileAmdgpu(devs[i]->nodes[j]);
            } catch (std::exception &e) {
                    //TODO: log probing here
                    std::cout << e.what() << std::endl;
            }
        }
    }

	throw std::runtime_error("can't create device");

}

} // namespace mempulse
