#pragma once

#include "mempulse/memory_reporter.h"

namespace mempulse {

class HipMemoryReporter : public MemoryReporter {
public:
    explicit HipMemoryReporter(int hipDeviceId);

    MemoryInfo GetMemoryInfo() override;
    MemoryUsage GetMemoryUsage() override;
    int GetDeviceId() const override { return m_hipDeviceId; }
	std::string GetDeviceName();
private:
    int m_hipDeviceId;
};

} // namespace mempulse
