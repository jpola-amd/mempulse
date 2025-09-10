#include "hip_memory_reporter.h"
#include <hip/hip_runtime.h>
#include "../../assert.h"
#include "../../utils.h"

namespace mempulse {

HipMemoryReporter::HipMemoryReporter(int hipDeviceId)
    : m_hipDeviceId(hipDeviceId) {}

MemoryInfo HipMemoryReporter::GetMemoryInfo() {
	hipError_t result;

    hipDeviceProp_t prop;
    result = hipGetDeviceProperties(&prop, m_hipDeviceId);
	CHECK_HIP(result, "can't get hip device properties");

    result = hipSetDevice(m_hipDeviceId);
	CHECK_HIP(result, "failed to execute hipSetDevice");

    size_t free = 0, total = 0;
    result = hipMemGetInfo(&free, &total);
	CHECK_HIP(result, "can't execute hipMemGetInfo");

	return MemoryInfo {
		.dedicatedUsed = total - free,
		.dedicatedTotal = total,
		.sharedUsed = 0,
		.sharedTotal = 0,
	};
}

MemoryUsage HipMemoryReporter::GetMemoryUsage() {
	MemoryInfo memInfo = GetMemoryInfo();

    return MemoryUsage {
		.free = memInfo.dedicatedTotal - memInfo.dedicatedUsed,
		.total = memInfo.dedicatedTotal,
	};
}


std::string HipMemoryReporter::GetDeviceName() {
	hipError_t result;

	hipDeviceProp_t prop;

    result = hipGetDeviceProperties(&prop, m_hipDeviceId);
	CHECK_HIP(result, "can't get hip device properties");

	return prop.name;
}

} // namespace mempulse
