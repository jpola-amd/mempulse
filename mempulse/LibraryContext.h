#pragma once

#include <memory>

namespace mempulse {

class Device;

class LibraryContext
{
public:
	virtual ~LibraryContext() = default;

	virtual int GetDeviceCount() = 0;
	virtual std::unique_ptr<Device> createDevice(int deviceIndex) = 0;
};

} // namespace mempulse
