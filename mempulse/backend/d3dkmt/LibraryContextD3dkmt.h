#pragma once

#include "mempulse/backend/hip/LibraryContextHip.h"

namespace mempulse {

class LibraryContextD3dkmt : public LibraryContextHip {
public:
    LibraryContextD3dkmt();

    std::unique_ptr<Device> createDevice(int deviceIndex) override;
};

} // namespace mempulse
