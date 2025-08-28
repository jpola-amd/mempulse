#pragma once

#include "mempulse/memory_reporter.h"
#include <dxgi1_4.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace mempulse {

// struct PciBusInfo {
//     uint32_t domain;    // PCI domain
//     uint32_t bus;       // PCI bus number
//     uint32_t device;    // PCI device number
//     uint32_t function;  // PCI function number
    
//     PciBusInfo() : domain(0), bus(0), device(0), function(0) {}
//     PciBusInfo(uint32_t d, uint32_t b, uint32_t dev, uint32_t f) 
//         : domain(d), bus(b), device(dev), function(f) {}
    
//     bool operator==(const PciBusInfo& other) const {
//         return domain == other.domain && bus == other.bus && 
//                device == other.device && function == other.function;
//     }
    
//     std::string ToString() const;
// };

class D3DKMTMemoryReporter : public MemoryReporter {

public:
    explicit D3DKMTMemoryReporter(int hipDeviceId, bool isIntegrated, LUID luid);

    virtual ~D3DKMTMemoryReporter() override = default;

    bool Initialize() override;
    void ShutDown() override;
    bool GetMemoryInfo(MemoryInfo& memInfo) const override;
    bool GetMemoryUsage(size_t& free, size_t& total) const override;
    int GetHipDeviceId() const override { return m_hipDeviceId; }
    bool IsInitialized() const override { return m_initialized; }



private:
    bool m_initialized {false};
    int m_hipDeviceId {-1};
    bool m_isIntegrated{ false};
    LUID m_luid {0, 0};

};
} // namespace mempulse
