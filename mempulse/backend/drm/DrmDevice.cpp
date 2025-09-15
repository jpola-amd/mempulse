#include "DrmDevice.h"
#include "check_drm.h"
#include <xf86drm.h>
#include <cassert>

namespace mempulse {

DrmDevice::DrmDevice(const File& file) {
	if (!file.IsOpen())
		throw std::runtime_error("DrmDevice::DrmDevice: file is not open");

	int err;
	err = drmGetDevice2(file, 0, &m_pDrmDevicePtr);
	check_drm(err, "Failed to get device info ");

	if (!m_pDrmDevicePtr) {
		throw std::runtime_error("drmGetDevice2 returned nullptr");
	}

	if (m_pDrmDevicePtr->bustype != DRM_BUS_PCI) {
		throw std::runtime_error("Unsupported bus type");
	}

}

DrmDevice::~DrmDevice() {
	if (!m_pDrmDevicePtr)
		return;

	drmFreeDevice(&m_pDrmDevicePtr);
}

short DrmDevice::GetBus() const {
	assert(m_pDrmDevicePtr);
	return m_pDrmDevicePtr->businfo.pci->bus;
}

unsigned int DrmDevice::GetDeviceId() const {
	assert(m_pDrmDevicePtr);
	return  m_pDrmDevicePtr->deviceinfo.pci->device_id;
}

}