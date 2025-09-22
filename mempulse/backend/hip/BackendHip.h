#pragma once

#include "mempulse/Backend.h"

namespace mempulse {

class BackendHip : public Backend {
public:
	BackendHip();
	~BackendHip();

    int GetDeviceCount() override;

	std::unique_ptr<Device> createDevice(int deviceIndex) override;
};

}
