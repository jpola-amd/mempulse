#pragma once

#include <string>
#include <mempulse/memory_info.h>
#include <mempulse/memory_usage.h>

namespace mempulse {


/**
 * Abstract base class for memory reporting across platforms
 */
class MemoryReporter {
public:

    virtual ~MemoryReporter() = default;

    /**
     * Get current memory information
     * @param memInfo Output structure to fill with memory data
     * @return true if successful
     */
    virtual MemoryInfo GetMemoryInfo() = 0;

    /**
     * Get current memory usage
     * @param free Output parameter to fill with free memory (bytes)
     * @param total Output parameter to fill with total memory (bytes)
     * @return MemoryUsage in bytes
     */
    virtual MemoryUsage GetMemoryUsage() = 0;

	virtual std::string GetDeviceName() = 0;

    /**
     * Get the HIP device ID this reporter is associated with
     * @return HIP device ID
     */
    virtual int GetDeviceId() const = 0;
};

std::ostream& operator<<(std::ostream& stream, const MemoryInfo& memoryInfo);

} // namespace mempulse
