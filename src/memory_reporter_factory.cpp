#include "mempulse/memory_reporter_factory.h"
#include <hip/hip_runtime.h>
#include <memory>


//TODO:REMOVE iostream
#include <iostream>
#include <iomanip>

#ifdef _WIN32
    #include "platform/windows/d3dkmt_memory_reporter.h"
#else
    #include "platform/linux/hip_memory_reporter.h"
#endif

namespace mempulse {

std::unique_ptr<MemoryReporter> MemoryReporterFactory::CreateMemoryReporter(int hipDeviceId) {
    
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
    
    LUID hipLuid;
    memcpy(&hipLuid, deviceProperties.luid, sizeof(LUID));

    std::cout << "Creating memory reporter for device: " << deviceProperties.name 
    << " (LUID: 0x" << std::hex << std::uppercase << std::setfill('0')
              << std::setw(8) << static_cast<uint32_t>(hipLuid.HighPart) << ":"
              << std::setw(8) << hipLuid.LowPart << std::dec << ")" << std::endl;

    std::unique_ptr<MemoryReporter> reporter;
    
#ifdef _WIN32
    reporter = std::make_unique<D3DKMTMemoryReporter>(hipDeviceId, deviceProperties.integrated, hipLuid);
#else
    reporter = std::make_unique<HipMemoryReporter>(hipDeviceId);
#endif
    
    // Initialize the reporter
    if (reporter && reporter->Initialize()) {
        return reporter;
    }
    
    return nullptr;
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
