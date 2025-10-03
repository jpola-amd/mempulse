#pragma once

#include "mempulse.h"
#include <string>

namespace mempulse {

class Device {
public:
	explicit Device(int hardwareId) : m_hardwareId(hardwareId) {}

	virtual ~Device() = default;

    virtual MempulseDeviceMemoryInfo GetMemoryInfo() = 0;

    virtual MempulseDeviceMemoryUsage GetMemoryUsage() = 0;

	virtual std::string GetHardwareName() = 0;

    int GetHardwareId() const { return m_hardwareId; }
private:
	int m_hardwareId;
};

} // namespace mempulse
