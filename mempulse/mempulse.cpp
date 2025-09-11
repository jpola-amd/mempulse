#include "mempulse.h"
#include "Config.h"
#include "Device.h"
#include "Utils.h"
#include "Backend.h"
#include "MempulseContextImpl.h"
#include "SafeCall.h"
#include "Logging.h"

#include <string.h>

using namespace mempulse;

MempulseError MempulseInitialize(MempulseContext* context, MempulseBackend backend) {
	MEMPULSE_LOG_TRACE();

	auto result = safeCall([&] {
		MempulseContextImpl* ctx = new MempulseContextImpl(backend);
		*context = ctx;
	});
	check(result);

	return MEMPULSE_SUCCESS;
}

MempulseError MempulseShutdown(MempulseContext context) {
	MEMPULSE_LOG_TRACE();

	auto result = safeCall([&] {
		MempulseContextImpl* ctx = get_ctx(context);
		delete ctx;
	});
	check(result);

	return MEMPULSE_SUCCESS;
}

MEMPULSE_API MempulseError MempulseSetLoggingLevel(int level)
{
	MEMPULSE_LOG_TRACE();

	MEMPULSE_LOGGING_LEVEL = level;
	return MEMPULSE_SUCCESS;
}

const char* MempulseGetVersion() {
	MEMPULSE_LOG_TRACE();

    return MEMPULSE_VERSION_STRING;
}

MempulseError MempulseGetAvailabeDeviceCount(MempulseContext context, int* count) {
	MEMPULSE_LOG_TRACE();

	auto result = safeCall([&] {
		MempulseContextImpl* ctx = get_ctx(context);
		if (!count)
			throw ErrorInvalidParameter("count", "must be not null");
		*count = ctx->backend()->GetDeviceCount();
	});
	check(result);

	return MEMPULSE_SUCCESS;
}

MempulseError MempulseGetDeviceName(
		MempulseContext context,
		int deviceIndex,
		char* deviceName,
		unsigned long long int deviceNameSize) {
	MEMPULSE_LOG_TRACE();

	auto result = safeCall([&] {
		MempulseContextImpl* ctx = get_ctx(context);
		if (!deviceName)
			throw ErrorInvalidParameter("deviceName", "must be not null");

		auto device = ctx->backend()->createDevice(deviceIndex);

		const std::string gpuDeviceName = device->GetHardwareName();

		for (unsigned long long int i = 0; i < deviceNameSize; ++i) {
			if (i < gpuDeviceName.size()) {
				deviceName[i] = gpuDeviceName[i];
			} else {
				deviceName[i] = '\0';
			}
		}
	});
	check(result);

	return MEMPULSE_SUCCESS;
}

MempulseError MempulseGetDeviceMemoryInfo(
		MempulseContext context,
		int deviceIndex,
		MempulseDeviceMemoryInfo* deviceMemoryInfo)
{
	MEMPULSE_LOG_TRACE();

	auto result = safeCall([&] {
		MempulseContextImpl* ctx = get_ctx(context);
		if (!deviceMemoryInfo)
			throw ErrorInvalidParameter("deviceMemoryInfo", "must be not nullptr");

		auto device = ctx->backend()->createDevice(deviceIndex);
		if (!device)
			throw ErrorInvalidDevice(deviceIndex);

		*deviceMemoryInfo = device->GetMemoryInfo();
	});
	check(result);

	return MEMPULSE_SUCCESS;
}

MEMPULSE_API MempulseError MempulseGetDeviceMemoryUsage(
		MempulseContext context,
		int deviceIndex,
		MempulseDeviceMemoryUsage* deviceMemoryUsage)
{
	MEMPULSE_LOG_TRACE();

	auto result = safeCall([&] {
		MempulseContextImpl* ctx = get_ctx(context);

		if (!deviceMemoryUsage)
			throw ErrorInvalidParameter("deviceMemoryUsage", "must be not nullptr");

		auto device = ctx->backend()->createDevice(deviceIndex);
		if (!device)
			throw ErrorInvalidDevice(deviceIndex);

		*deviceMemoryUsage = device->GetMemoryUsage();
	});
	check(result);

	return MEMPULSE_SUCCESS;
}

void MempulsePrintDeviceMemoryInfo(
		const MempulseDeviceMemoryInfo& deviceMemoryInfo) {
	std::cout << deviceMemoryInfo << std::endl;
}

MEMPULSE_API void MempulsePrintDeviceMemoryUsage(
		const MempulseDeviceMemoryUsage& deviceMemoryUsage)
{
    std::cout << deviceMemoryUsage << std::endl;
}

