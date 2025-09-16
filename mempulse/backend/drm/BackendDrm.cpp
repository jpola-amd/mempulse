#include "BackendDrm.h"
#include "DeviceDrm.h"
#include "mempulse/Logging.h"

#include "DrmDevices.h"

namespace mempulse {

BackendDrm::BackendDrm() {
	MEMPULSE_LOG_TRACE();

}

int BackendDrm::GetDeviceCount() {
	MEMPULSE_LOG_TRACE();

	seteuid(0);
	DrmDevices devs;
	size_t count = devs.Size();
	setuid(getuid());

	return count;
}

std::unique_ptr<Device> BackendDrm::createDevice(int deviceIndex) {
	MEMPULSE_LOG_TRACE();

	return std::make_unique<DeviceDrm>(*this, deviceIndex);
}

} // namespace mempulse
