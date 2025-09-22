#pragma once

#include <memory>

namespace mempulse {

class Device;

class Backend
{
public:
	virtual ~Backend() = default;

	virtual int GetDeviceCount() = 0;
	virtual std::unique_ptr<Device> createDevice(int deviceIndex) = 0;
};

} // namespace mempulse
