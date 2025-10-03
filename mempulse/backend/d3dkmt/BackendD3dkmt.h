#pragma once

#include "mempulse/backend/hip/BackendHip.h"

namespace mempulse {

class BackendD3dkmt : public BackendHip {
public:
    BackendD3dkmt();

    std::unique_ptr<Device> createDevice(int deviceIndex) override;
};

} // namespace mempulse
