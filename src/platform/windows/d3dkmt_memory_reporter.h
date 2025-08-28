#pragma once

#include "mempulse/memory_reporter.h"
#include "dxgi.h" // to have LUID


namespace mempulse {

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
