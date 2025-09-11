#pragma once

#include "mempulse/LibraryContext.h"

namespace mempulse {

class LibraryContextDrm : public LibraryContext {
public:
	LibraryContextDrm();
	int GetDeviceCount() override;

	std::unique_ptr<Device> createDevice(int deviceIndex) override;
};

}
