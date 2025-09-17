#include "BackendHip.h"
#include "DeviceHip.h"
#include "mempulse/Logging.h"
#include "check_hip.h"

#include <hip/hip_runtime.h>

namespace mempulse {

BackendHip::BackendHip() {
	MEMPULSE_LOG_TRACE();

    hipError_t err = hipInit(0);
	CHECK_HIP(err, "failed to initalize hip");
}

int BackendHip::GetDeviceCount() {
	MEMPULSE_LOG_TRACE();

	int deviceCount;

    hipError_t err = hipGetDeviceCount(&deviceCount);
	CHECK_HIP(err, "failed to get device count");

	return deviceCount;
}

std::unique_ptr<Device> BackendHip::createDevice(int deviceIndex) {
	MEMPULSE_LOG_TRACE();

	return std::make_unique<DeviceHip>(*this, deviceIndex);
}

} // namespace mempulse
