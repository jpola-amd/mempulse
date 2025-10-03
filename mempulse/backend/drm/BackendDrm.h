#pragma once

#include "mempulse/backend/hip/BackendHip.h"

namespace mempulse {

class BackendDrm : public BackendHip {
public:
	BackendDrm();
	//int GetDeviceCount() override;

	std::unique_ptr<Device> createDevice(int deviceIndex) override;
private:
};

}
