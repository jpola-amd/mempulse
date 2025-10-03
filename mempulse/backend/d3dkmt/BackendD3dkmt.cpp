#include "BackendD3dkmt.h"
#include "DeviceD3dkmt.h"
#include "mempulse/Logging.h"

namespace mempulse {


BackendD3dkmt::BackendD3dkmt() {
    MEMPULSE_LOG_TRACE();
}

std::unique_ptr<Device> BackendD3dkmt::createDevice(int deviceIndex) {
	MEMPULSE_LOG_TRACE();

	return std::make_unique<DeviceD3dkmt>(*this, deviceIndex);
}

} // namespace mempulse
