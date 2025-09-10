#pragma once

#include <cstdint>
#include <ostream>

namespace mempulse {

struct MemoryUsage {
	uint64_t free;
	uint64_t total;
};

std::ostream& operator<<(std::ostream& stream, const MemoryUsage& usage);

} // namespace mempulse

