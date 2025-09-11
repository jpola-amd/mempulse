#include "BackendDrm.h"
#include "DeviceDrm.h"
#include "mempulse/Logging.h"

namespace mempulse {

BackendDrm::BackendDrm() {
	MEMPULSE_LOG_TRACE();
}

int BackendDrm::GetDeviceCount() {
	MEMPULSE_LOG_TRACE();

	int deviceCount = 0;

	return deviceCount;
}

std::unique_ptr<Device> BackendDrm::createDevice(int deviceIndex) {
	MEMPULSE_LOG_TRACE();

	return std::make_unique<DeviceDrm>(*this, deviceIndex);
}

} // namespace mempulse
