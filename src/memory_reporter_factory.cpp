#include "mempulse/memory_reporter_factory.h"
#include <hip/hip_runtime.h>

#ifdef _WIN32
	#include <memory>
    #include "platform/windows/d3dkmt_memory_reporter.h"
    #include "platform/linux/hip_memory_reporter.h"
#else
    #include "platform/linux/hip_memory_reporter.h"
#endif

namespace mempulse {

std::unique_ptr<MemoryReporter> MemoryReporterFactory::CreateMemoryReporter(int hipDeviceId, bool overrideD3DKMTwithHIP) {

    int deviceCount = 0;
    hipError_t err = hipGetDeviceCount(&deviceCount);
    if (err != hipSuccess || hipDeviceId < 0 || hipDeviceId >= deviceCount) {
        return nullptr;
    }

    hipDeviceProp_t deviceProperties;
    err = hipGetDeviceProperties(&deviceProperties, hipDeviceId);
    if (err != hipSuccess) {
        return nullptr;
    }
    std::unique_ptr<MemoryReporter> reporter;

#ifdef _WIN32
    if (!overrideD3DKMTwithHIP) {
        LUID hipLuid;
        memcpy(&hipLuid, deviceProperties.luid, sizeof(LUID));
        reporter = std::make_unique<D3DKMTMemoryReporter>(hipDeviceId, deviceProperties.integrated, hipLuid);
    }
    else {
        reporter = std::make_unique<HipMemoryReporter>(hipDeviceId);
    }
#else
    reporter = std::make_unique<HipMemoryReporter>(hipDeviceId);
#endif

    return reporter;
}

int MemoryReporterFactory::GetDeviceCount() {
    int deviceCount = 0;
    hipError_t err = hipGetDeviceCount(&deviceCount);
    if (err != hipSuccess) {
        return 0;
    }
    return deviceCount;
}

bool MemoryReporterFactory::IsHipAvailable() {
    hipError_t err = hipInit(0);
    return err == hipSuccess;
}

} // namespace mempulse
