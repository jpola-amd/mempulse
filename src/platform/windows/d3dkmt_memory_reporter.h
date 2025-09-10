#pragma once

#include "mempulse/memory_reporter.h"
#include "dxgi.h" // to have LUID


namespace mempulse {

class D3DKMTMemoryReporter : public MemoryReporter {

public:
    explicit D3DKMTMemoryReporter(int hipDeviceId, bool isIntegrated, LUID luid);

    MemoryInfo GetMemoryInfo() override;
    MemoryUsage GetMemoryUsage() override;
    int GetDeviceId() const override { return m_hipDeviceId; }

private:
    int m_hipDeviceId {-1};
    bool m_isIntegrated{ false};
    LUID m_luid {0, 0};

};
} // namespace mempulse
