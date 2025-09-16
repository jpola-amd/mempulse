#pragma once

#include "mempulse/Backend.h"

namespace mempulse {

class BackendDrm : public Backend {
public:
	BackendDrm();
	int GetDeviceCount() override;

	std::unique_ptr<Device> createDevice(int deviceIndex) override;
private:
};

}
