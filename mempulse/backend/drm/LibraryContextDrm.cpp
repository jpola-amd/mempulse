#include "LibraryContextDrm.h"
#include "DeviceDrm.h"
#include "mempulse/Logging.h"

namespace mempulse {

LibraryContextDrm::LibraryContextDrm() {
	MEMPULSE_LOG_TRACE();
}

int LibraryContextDrm::GetDeviceCount() {
	MEMPULSE_LOG_TRACE();

	int deviceCount = 0;

	return deviceCount;
}

std::unique_ptr<Device> LibraryContextDrm::createDevice(int deviceIndex) {
	MEMPULSE_LOG_TRACE();

	return std::make_unique<DeviceDrm>(*this, deviceIndex);
}

} // namespace mempulse
