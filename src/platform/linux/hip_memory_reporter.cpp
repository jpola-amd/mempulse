#include "hip_memory_reporter.h"
#include <hip/hip_runtime.h>
#include "../../assert.h"

namespace mempulse {

HipMemoryReporter::HipMemoryReporter(int hipDeviceId)
    : m_hipDeviceId(hipDeviceId) {}

bool HipMemoryReporter::Initialize() {
    if (m_initialized) {
        return true;
    }
    m_initialized = true;
    return true;
}

void HipMemoryReporter::ShutDown() {
    m_initialized = false;
}

bool HipMemoryReporter::GetMemoryInfo(MemoryInfo& memInfo) const {
    if (!m_initialized) {
        return false;
    }

    hipDeviceProp_t prop;
    hipGetDeviceProperties(&prop, m_hipDeviceId);
    hipError_t result = hipSetDevice(m_hipDeviceId);
    if (result != hipSuccess) {
        assert_with_message(false, "Failed to execute hipSetDevice.");
        return false;
    }
    size_t free = 0, total = 0;
    result = hipMemGetInfo(&free, &total);
    if (result != hipSuccess) {
        assert_with_message(false, "Failed to execute hipMemGetInfo.");
        return false;
    }
    memInfo.dedicatedTotal = total;
    memInfo.dedicatedUsed = total - free;

    return true;
}

bool HipMemoryReporter::GetMemoryUsage(size_t& free, size_t& total) const {
    if (!m_initialized) {
        return false;
    }

    MemoryInfo memInfo;
    if (!GetMemoryInfo(memInfo)) {
        return false;
    }

    total = memInfo.dedicatedTotal;
    free = total - memInfo.dedicatedUsed;
    return true;
}

} // namespace mempulse
