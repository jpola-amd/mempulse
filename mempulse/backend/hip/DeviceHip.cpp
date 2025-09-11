#include "DeviceHip.h"
#include "LibraryContextHip.h"
#include "check_hip.h"
#include "mempulse/Logging.h"

namespace mempulse {

DeviceHip::DeviceHip(const LibraryContextHip&, int deviceId)
: Device(deviceId)
{
	MEMPULSE_LOG_TRACE();
	hipError_t err;

    err = hipGetDeviceProperties(&m_deviceProperties, deviceId);
	CHECK_HIP(err, "can't get device properties");
}

MempulseDeviceMemoryInfo DeviceHip::GetMemoryInfo()
{
	MEMPULSE_LOG_TRACE();

	hipError_t err;

	err = hipSetDevice(GetHardwareId());
	CHECK_HIP(err, "failed to execute hipSetDevice");

    size_t free = 0, total = 0;
    err = hipMemGetInfo(&free, &total);
	CHECK_HIP(err, "can't execute hipMemGetInfo");

	return {
		.dedicatedUsed = total - free,
		.dedicatedTotal = total,
		.sharedUsed = 0,
		.sharedTotal = 0,
	};
}

MempulseDeviceMemoryUsage DeviceHip::GetMemoryUsage()
{
	MEMPULSE_LOG_TRACE();

	MempulseDeviceMemoryInfo memInfo = GetMemoryInfo();

    return {
		.free = memInfo.dedicatedTotal - memInfo.dedicatedUsed,
		.total = memInfo.dedicatedTotal,
	};
}

std::string DeviceHip::GetHardwareName()
{
	MEMPULSE_LOG_TRACE();

	return m_deviceProperties.name;
}

DeviceHip::Luid DeviceHip::luid() const
{
    Luid luid;
    static_assert(luid.size() == sizeof(m_deviceProperties.luid));

	memcpy(luid.data(), m_deviceProperties.luid, luid.size());
    return luid;
}

bool DeviceHip::IsIntegrated() const
{
    return m_deviceProperties.integrated;
}

} // namespace mempulse
