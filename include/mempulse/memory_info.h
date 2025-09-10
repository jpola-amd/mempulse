#pragma once

#include <cstdint>
#include <ostream>

namespace mempulse {

struct MemoryInfo {
    // on linux only dedicated is reported
    // for integrated gpus it account the memory in dedicated section
    uint64_t dedicatedUsed;
    uint64_t dedicatedTotal;
    uint64_t sharedUsed;
    uint64_t sharedTotal;
};

std::ostream& operator<<(std::ostream& stream, const MemoryInfo& memInfo);

} // namespace mempulse
