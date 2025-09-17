#include "DrmDevice.h"
#include "check_drm.h"
#include "mempulse/Logging.h"
#include <xf86drm.h>
#include <cassert>

namespace mempulse {

DrmDevice::DrmDevice(const File& file) {
	MEMPULSE_LOG_TRACE();

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
	MEMPULSE_LOG_TRACE();

	if (!m_pDrmDevicePtr)
		return;

	drmFreeDevice(&m_pDrmDevicePtr);
}

short DrmDevice::GetBus() const {
	MEMPULSE_LOG_TRACE();

	assert(m_pDrmDevicePtr);
	assert(m_pDrmDevicePtr->businfo.pci);

	return m_pDrmDevicePtr->businfo.pci->bus;
}

unsigned int DrmDevice::GetDeviceId() const {
	MEMPULSE_LOG_TRACE();

	assert(m_pDrmDevicePtr);
	assert(m_pDrmDevicePtr->businfo.pci);

	return  m_pDrmDevicePtr->deviceinfo.pci->device_id;
}

}
