#include "DrmDevice.h"
#include "ErrorDrm.h"
#include "mempulse/Logging.h"
#include "mempulse/Assert.h"
#include <xf86drm.h>

namespace mempulse {

DrmDevice::DrmDevice(const File& file) {
	MEMPULSE_LOG_TRACE();

	if (!file.IsOpen())
		throw ErrorDrm("DrmDevice::DrmDevice: file is not open");

	int err;
	err = drmGetDevice2(file, 0, &m_pDrmDevicePtr);
	check_drm(err, "failed to get device info ");

	if (!m_pDrmDevicePtr) {
		throw ErrorDrm("drmGetDevice2 returned nullptr");
	}

	if (m_pDrmDevicePtr->bustype != DRM_BUS_PCI) {
		throw ErrorDrm("unsupported bus type");
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

	assert_with_message(m_pDrmDevicePtr, "m_pDrmDevicePtr is nullptr");
	assert_with_message(m_pDrmDevicePtr->businfo.pci, "m_pDrmDevicePtr->businfo.pci is nullptr");

	return m_pDrmDevicePtr->businfo.pci->bus;
}

unsigned int DrmDevice::GetDeviceId() const {
	MEMPULSE_LOG_TRACE();

	assert_with_message(m_pDrmDevicePtr, "m_pDrmDevicePtr is nullptr");
	assert_with_message(m_pDrmDevicePtr->businfo.pci, "m_pDrmDevicePtr->businfo.pci is nullptr");

	return  m_pDrmDevicePtr->deviceinfo.pci->device_id;
}

} // namespace mempulse
