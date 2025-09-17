#include "Utils.h"
#include "Logging.h"

namespace mempulse {

std::ostream& operator<<(std::ostream& stream, const MempulseDeviceMemoryInfo& memInfo)
{
	MEMPULSE_LOG_TRACE();

	stream << "Total Memory Local:\t" << memInfo.dedicatedTotal / (1024*1024) << " MB ("
        << static_cast<float>(memInfo.dedicatedTotal) / (1024*1024*1024) << " GB)" << std::endl;
    stream << "Total Memory Shared:\t" << memInfo.sharedTotal  / (1024*1024) << " MB ("
    	<< static_cast<float>( memInfo.sharedTotal) / (1024*1024*1024) << " GB)" << std::endl;

    stream << "Used Memory Local:\t" << memInfo.dedicatedUsed / (1024*1024) << " MB ("
    	<< static_cast<float>(memInfo.dedicatedUsed) / (1024*1024*1024) << " GB)" << std::endl;

    stream << "Used Memory Shared:\t" << memInfo.sharedUsed / (1024*1024) << " MB ("
    	<< static_cast<float>(memInfo.sharedUsed) / (1024*1024*1024) << " GB)" << std::endl;

    stream << "Free Memory: Local:\t" << (memInfo.dedicatedTotal - memInfo.dedicatedUsed) / (1024*1024) << " MB ("
    	<< static_cast<float>(memInfo.dedicatedTotal - memInfo.dedicatedUsed) / (1024*1024*1024) << " GB)" << std::endl;

    stream << "Free Memory: Shared:\t" << (memInfo.sharedTotal - memInfo.sharedUsed) / (1024*1024) << " MB ("
    	<< static_cast<float>(memInfo.sharedTotal - memInfo.sharedUsed) / (1024*1024*1024) << " GB)" << std::endl;

	return stream;

}

std::ostream& operator<<(std::ostream& stream, const MempulseDeviceMemoryUsage& usage)
{
	MEMPULSE_LOG_TRACE();

	stream << "Free Memory: " << usage.free / (1024*1024) << " MB ("
        << static_cast<float>(usage.total) / (1024*1024*1024) << " GB)" << std::endl;

	return stream;
}

} // namespace mempulse



