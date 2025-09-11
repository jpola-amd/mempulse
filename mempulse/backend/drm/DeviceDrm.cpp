#include "DeviceDrm.h"
#include "LibraryContextDrm.h"
#include "mempulse/Logging.h"

namespace mempulse {

DeviceDrm::DeviceDrm(const LibraryContextDrm&, int deviceId)
: Device(deviceId)
{
	MEMPULSE_LOG_TRACE();
}

MempulseDeviceMemoryInfo DeviceDrm::GetMemoryInfo()
{
	MEMPULSE_LOG_TRACE();

	return {
		.dedicatedUsed = 0,
		.dedicatedTotal = 0,
		.sharedUsed = 0,
		.sharedTotal = 0,
	};
}

MempulseDeviceMemoryUsage DeviceDrm::GetMemoryUsage()
{
	MEMPULSE_LOG_TRACE();

	MempulseDeviceMemoryInfo memInfo = GetMemoryInfo();

    return {
		.free = memInfo.dedicatedTotal - memInfo.dedicatedUsed,
		.total = memInfo.dedicatedTotal,
	};
}

std::string DeviceDrm::GetHardwareName()
{
	MEMPULSE_LOG_TRACE();

	return "";
}

} // namespace mempulse
