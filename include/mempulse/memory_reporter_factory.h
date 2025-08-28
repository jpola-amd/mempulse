#pragma once

#include "memory_reporter.h"
#include <memory>

namespace mempulse {

/**
 * Factory class for creating platform-specific memory reporters
 */
class MemoryReporterFactory {
public:
    /**
     * Create a memory reporter for the specified HIP device
     * @param hipDeviceId The HIP device ID to create a reporter for
     * @return Pointer to MemoryReporter instance, nullptr on failure
     */
    static std::unique_ptr<MemoryReporter> CreateMemoryReporter(int hipDeviceId);
    
    /**
     * Get the number of available HIP devices
     * @return Number of HIP devices
     */
    static int GetDeviceCount();
    
    /**
     * Check if HIP is available on the system
     * @return true if HIP is available
     */
    static bool IsHipAvailable();

private:
    MemoryReporterFactory() = default;
};

} // namespace mempulse
