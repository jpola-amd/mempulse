#pragma once

#include "mempulse.h"

#include <cstdint>
#include <ostream>

namespace mempulse {

std::ostream& operator<<(std::ostream& stream, const MempulseDeviceMemoryInfo& memInfo);
std::ostream& operator<<(std::ostream& stream, const MempulseDeviceMemoryUsage& usage);

} // namespace mempulse

