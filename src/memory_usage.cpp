#include "mempulse/memory_usage.h"

namespace mempulse {

std::ostream& operator<<(std::ostream& stream, const MemoryUsage& usage)
{
	stream << "Free Memory: " << usage.free / (1024*1024) << " MB ("
        << (float) usage.total / (1024*1024*1024) << " GB)" << std::endl;

	return stream;
}

}
