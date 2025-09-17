#include "HardwarePci.h"
#include "File.h"
#include "DrmVersionPtr.h"
#include "DrmAmdgpu.h"
#include "DrmFileAmdgpu.h"
#include "DrmDevices.h"
#include "Defines.h"
#include <iostream>

namespace mempulse {

HardwarePci::HardwarePci(const char *path, short bus)
: m_drmFile(OpenDrmFile(path, bus)),
  m_drmDevice(m_drmFile),
  m_drmAmdgpu(m_drmFile)
{
}

DrmFileAmdgpu HardwarePci::OpenDrmFile(const char* path, short bus) {
	if (path) {
		return DrmFileAmdgpu(path);
	}

	std::optional<DrmFileAmdgpu> drmFile = FindDrm(bus);
	if (!drmFile.has_value()) {
		throw std::runtime_error("Failed to find DRM device");
	}
	return *drmFile;;
}

std::optional<DrmFileAmdgpu> HardwarePci::FindDrm(short bus) {
	DrmDevices devs;

	for (size_t i = 0; i < devs.Size(); i++) {
		if (devs[i]->bustype != DRM_BUS_PCI ||
			devs[i]->deviceinfo.pci->vendor_id != VENDOR_AMD ||
			(bus >= 0 && bus != devs[i]->businfo.pci->bus))
			continue;

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
	return std::nullopt;
}

}
