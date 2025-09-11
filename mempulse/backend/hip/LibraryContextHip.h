#pragma once

#include "mempulse/LibraryContext.h"

namespace mempulse {

class LibraryContextHip : public LibraryContext {
public:
	LibraryContextHip();
    int GetDeviceCount() override;

	std::unique_ptr<Device> createDevice(int deviceIndex) override;
};

}
