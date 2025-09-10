#include "mempulse/memory_info.h"

namespace mempulse {

std::ostream& operator<<(std::ostream& stream, const MemoryInfo& memInfo)
{
	stream << "Total Memory Local:\t" << memInfo.dedicatedTotal / (1024*1024) << " MB ("
        << (float) memInfo.dedicatedTotal / (1024*1024*1024) << " GB)" << std::endl;
    stream << "Total Memory Shared:\t" << memInfo.sharedTotal  / (1024*1024) << " MB ("
    	<< (float) memInfo.sharedTotal  / (1024*1024*1024) << " GB)" << std::endl;

    stream << "Used Memory Local:\t" << memInfo.dedicatedUsed / (1024*1024) << " MB ("
    	<< (float) memInfo.dedicatedUsed / (1024*1024*1024) << " GB)" << std::endl;

    stream << "Used Memory Shared:\t" << memInfo.sharedUsed / (1024*1024) << " MB ("
    	<< (float) memInfo.sharedUsed / (1024*1024*1024) << " GB)" << std::endl;

    stream << "Free Memory: Local:\t" << (memInfo.dedicatedTotal - memInfo.dedicatedUsed) / (1024*1024) << " MB ("
    	<< (float) (memInfo.dedicatedTotal - memInfo.dedicatedUsed) / (1024*1024*1024) << " GB)" << std::endl;

    stream << "Free Memory: Shared:\t" << (memInfo.sharedTotal - memInfo.sharedUsed) / (1024*1024) << " MB ("
    	<< (float) (memInfo.sharedTotal - memInfo.sharedUsed) / (1024*1024*1024) << " GB)" << std::endl;

	return stream;
}

}
