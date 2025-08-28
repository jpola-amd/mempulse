#pragma once

#include "mempulse/memory_reporter.h"

namespace mempulse {

class HipMemoryReporter : public MemoryReporter {
public:
    explicit HipMemoryReporter(int hipDeviceId);
    virtual ~HipMemoryReporter() override = default;

    bool Initialize() override;
    void ShutDown() override;
    bool GetMemoryInfo(MemoryInfo& memInfo) const override;
    bool GetMemoryUsage(size_t& free, size_t& total) const override;
    int GetHipDeviceId() const override { return m_hipDeviceId; }
    bool IsInitialized() const override { return m_initialized; }

private:
    bool m_initialized {false};
    int m_hipDeviceId {-1};
};

} // namespace mempulse
