#include "DeviceHip.h"
#include "BackendHip.h"
#include "check_hip.h"
#include "mempulse/Logging.h"

namespace mempulse {

DeviceHip::DeviceHip(const BackendHip&, int deviceId)
: Device(deviceId)
{
	MEMPULSE_LOG_TRACE();
	hipError_t err;

    err = hipGetDeviceProperties(&m_deviceProperties, deviceId);
	CHECK_HIP(err, "can't get device properties");

    assert(m_luid.size() == sizeof(m_deviceProperties.luid));
	memcpy(m_luid.data(), m_deviceProperties.luid, m_luid.size());
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

const DeviceHip::Luid& DeviceHip::luid() const
{
    return m_luid;
}

bool DeviceHip::IsIntegrated() const {
    return m_deviceProperties.integrated;
}

int DeviceHip::PciBusID() const {
	return m_deviceProperties.pciBusID;
}

int DeviceHip::PciDeviceID() const {
	return m_deviceProperties.pciDeviceID;
}

int DeviceHip::PciDomainID() const {
	return m_deviceProperties.pciDomainID;
}

} // namespace mempulse
