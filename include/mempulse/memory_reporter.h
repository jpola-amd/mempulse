#pragma once

#include <cstdint>
#include <string>

namespace mempulse {

/**
 * Structure containing GPU memory information
 */

struct MemoryInfo {
    // on linux only dedicated is reported 
    // for integrated gpus it account the memory in dedicated section
    uint64_t dedicatedUsed {0};
    uint64_t dedicatedTotal {0};
    uint64_t sharedUsed {0};
    uint64_t sharedTotal {0};
   
};

/**
 * Abstract base class for memory reporting across platforms
 */
class MemoryReporter {
public:
    virtual ~MemoryReporter() = default;
    
    /**
     * Initialize the memory reporter
     * @return true if initialization was successful
     */
    virtual bool Initialize() = 0;
    
    /**
     * Shutdown the memory reporter and cleanup resources
     */
    virtual void ShutDown() = 0;
    
    /**
     * Get current memory information
     * @param memInfo Output structure to fill with memory data
     * @return true if successful
     */
    virtual bool GetMemoryInfo(MemoryInfo& memInfo) const = 0;

    /**
     * Get current memory usage
     * @param free Output parameter to fill with free memory (bytes)
     * @param total Output parameter to fill with total memory (bytes)
     * @return true if successful
     */
    virtual bool GetMemoryUsage(size_t& free, size_t& total) const = 0;
    
    /**
     * Get the HIP device ID this reporter is associated with
     * @return HIP device ID
     */
    virtual int GetHipDeviceId() const = 0;
    
    /**
     * Check if the reporter is properly initialized
     * @return true if initialized
     */
    virtual bool IsInitialized() const = 0;
};

} // namespace mempulse
