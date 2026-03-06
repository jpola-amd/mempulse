#include "mempulse.h"
#include "Config.h"
#include "Device.h"
#include "Utils.h"
#include "Backend.h"
#include "LibraryContext.h"
#include "SafeCall.h"
#include "Logging.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <limits>
#include <optional>
#include <string>

using namespace mempulse;

namespace {

std::optional<unsigned long long> parse_memory_bytes(std::string value) {
	value.erase(std::remove_if(value.begin(), value.end(), [](unsigned char ch) {
		return std::isspace(ch) != 0;
	}), value.end());

	if (value.empty())
		return std::nullopt;

	std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
		return static_cast<char>(std::tolower(ch));
	});

	std::size_t split = 0;
	while (split < value.size() && std::isdigit(static_cast<unsigned char>(value[split])) != 0)
		++split;

	if (split == 0)
		return std::nullopt;

	unsigned long long base = 0;
	try {
		base = static_cast<unsigned long long>(std::stoull(value.substr(0, split)));
	} catch (...) {
		return std::nullopt;
	}

	const std::string unit = value.substr(split);
	unsigned long long multiplier = 1ULL;
	if (unit.empty() || unit == "b") {
		multiplier = 1ULL;
	} else if (unit == "kb") {
		multiplier = 1000ULL;
	} else if (unit == "mb") {
		multiplier = 1000ULL * 1000ULL;
	} else if (unit == "gb") {
		multiplier = 1000ULL * 1000ULL * 1000ULL;
	} else if (unit == "kib") {
		multiplier = 1024ULL;
	} else if (unit == "mib") {
		multiplier = 1024ULL * 1024ULL;
	} else if (unit == "gib") {
		multiplier = 1024ULL * 1024ULL * 1024ULL;
	} else if (unit == "tb") {
		multiplier = 1000ULL * 1000ULL * 1000ULL * 1000ULL;
	} else if (unit == "tib") {
		multiplier = 1024ULL * 1024ULL * 1024ULL * 1024ULL;
	} else {
		return std::nullopt;
	}

	if (base > std::numeric_limits<unsigned long long>::max() / multiplier)
		return std::nullopt;

	return base * multiplier;
}

std::optional<unsigned long long> get_trim_visible_memory_limit() {
	std::string value;
#ifdef _WIN32
	char* raw = nullptr;
	size_t len = 0;
	errno_t rc = _dupenv_s(&raw, &len, "MEMPULSE_TRIM_VISIBLE_MEMORY");
	if (rc != 0 || raw == nullptr)
		return std::nullopt;

	value.assign(raw);
	free(raw);
#else
	const char* raw = std::getenv("MEMPULSE_TRIM_VISIBLE_MEMORY");
	if (!raw)
		return std::nullopt;
	value.assign(raw);
#endif

	if (value.empty())
		return std::nullopt;

	return parse_memory_bytes(value);
}

void apply_trim_visible_memory(MempulseDeviceMemoryInfo& info) {
	auto trimVisibleMemory = get_trim_visible_memory_limit();
	if (!trimVisibleMemory.has_value())
		return;

	const unsigned long long limit = trimVisibleMemory.value();
	if (info.dedicatedTotal <= limit)
		return;

	info.dedicatedTotal = limit;
	if (info.dedicatedUsed > info.dedicatedTotal)
		info.dedicatedUsed = info.dedicatedTotal;
}

void apply_trim_visible_memory(MempulseDeviceMemoryUsage& usage) {
	auto trimVisibleMemory = get_trim_visible_memory_limit();
	if (!trimVisibleMemory.has_value())
		return;

	const unsigned long long limit = trimVisibleMemory.value();
	if (usage.total <= limit)
		return;

	const unsigned long long used = usage.total > usage.free ? usage.total - usage.free : 0ULL;
	usage.total = limit;
	usage.free = used >= usage.total ? 0ULL : usage.total - used;
}

} // namespace

MempulseError MempulseInitialize(MempulseContext* context, MempulseBackend backend) {
	MEMPULSE_LOG_TRACE();

	*context = nullptr; // in case of exception in initialization, context will be nullptr (required for lazy library shutdown)

	auto result = safeCall([&] {
		LibraryContext* ctx = new LibraryContext(backend);
		*context = ctx;
	});
	check(result);

	return MEMPULSE_SUCCESS;
}

MempulseError MempulseShutdown(MempulseContext context) {
	MEMPULSE_LOG_TRACE();

	auto result = safeCall([&] {
		LibraryContext* ctx = static_cast<LibraryContext*>(context);
		if (ctx != nullptr)
			delete ctx;
	});
	check(result);

	return MEMPULSE_SUCCESS;
}

MEMPULSE_API MempulseError MempulseSetLoggingLevel(int level) {
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
		LibraryContext* ctx = get_ctx(context);
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
		LibraryContext* ctx = get_ctx(context);
		if (!deviceName)
			throw ErrorInvalidParameter("deviceName", "must be not null");

		auto device = ctx->backend()->createDevice(deviceIndex);
		if (!device) {
			throw ErrorInvalidDevice(deviceIndex);
		}

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
		LibraryContext* ctx = get_ctx(context);
		if (!deviceMemoryInfo)
			throw ErrorInvalidParameter("deviceMemoryInfo", "must be not nullptr");

		auto device = ctx->backend()->createDevice(deviceIndex);
		if (!device)
			throw ErrorInvalidDevice(deviceIndex);

		auto info = device->GetMemoryInfo();
		apply_trim_visible_memory(info);
		*deviceMemoryInfo = info;
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
		LibraryContext* ctx = get_ctx(context);

		if (!deviceMemoryUsage)
			throw ErrorInvalidParameter("deviceMemoryUsage", "must be not nullptr");

		auto device = ctx->backend()->createDevice(deviceIndex);
		if (!device)
			throw ErrorInvalidDevice(deviceIndex);

		auto usage = device->GetMemoryUsage();
		apply_trim_visible_memory(usage);
		*deviceMemoryUsage = usage;
	});
	check(result);

	return MEMPULSE_SUCCESS;
}

MempulseError MempulsePrintDeviceMemoryInfo(
		const MempulseDeviceMemoryInfo* deviceMemoryInfo) {
	MEMPULSE_LOG_TRACE();

	auto result = safeCall([&] {
		if (!deviceMemoryInfo)
			throw ErrorInvalidParameter("deviceMemoryInfo", "must be not nullptr");

		std::cout << *deviceMemoryInfo << std::endl;
	});
	check(result);

	return MEMPULSE_SUCCESS;
}

MempulseError MempulsePrintDeviceMemoryUsage(
		const MempulseDeviceMemoryUsage* deviceMemoryUsage)
{
	MEMPULSE_LOG_TRACE();

	auto result = safeCall([&] {
		if (!deviceMemoryUsage)
			throw ErrorInvalidParameter("deviceMemoryUsage", "must be not nullptr");

    	std::cout << *deviceMemoryUsage << std::endl;
	});
	check(result);

	return MEMPULSE_SUCCESS;
}

