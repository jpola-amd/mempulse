#include <gtest/gtest.h>
#include "mempulse/mempulse.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>

namespace {

class ScopedEnvVar {
public:
	ScopedEnvVar(const char* name, const std::string& value)
		: m_name(name), m_hadOriginal(false) {
		loadCurrentValue();

		set(value.c_str());
	}

	~ScopedEnvVar() {
		if (m_hadOriginal)
			set(m_original.c_str());
		else
			unset();
	}

private:
	void loadCurrentValue() {
#ifdef _WIN32
		char* raw = nullptr;
		size_t len = 0;
		errno_t rc = _dupenv_s(&raw, &len, m_name.c_str());
		if (rc == 0 && raw != nullptr) {
			m_hadOriginal = true;
			m_original = raw;
			free(raw);
		}
#else
		const char* current = std::getenv(m_name.c_str());
		if (current != nullptr) {
			m_hadOriginal = true;
			m_original = current;
		}
#endif
	}

	void set(const char* value) {
#ifdef _WIN32
		_putenv_s(m_name.c_str(), value);
#else
		setenv(m_name.c_str(), value, 1);
#endif
	}

	void unset() {
#ifdef _WIN32
		_putenv_s(m_name.c_str(), "");
#else
		unsetenv(m_name.c_str());
#endif
	}

	std::string m_name;
	std::string m_original;
	bool m_hadOriginal;
};

} // namespace

struct TestDevice : public ::testing::TestWithParam<MempulseBackend> {
	void SetUp() {
		MempulseError err;

		const MempulseBackend backend = GetParam();

		err = MempulseInitialize(&context, backend);
		ASSERT_EQ(err, MEMPULSE_SUCCESS);

		err = MempulseSetLoggingLevel(0);
		ASSERT_EQ(err, MEMPULSE_SUCCESS);

	}

	void TearDown() {
		MempulseError err;
		err = MempulseShutdown(context);
		EXPECT_EQ(err, MEMPULSE_SUCCESS);
	}

	MempulseContext context;
};

TEST_P(TestDevice, count) {
	MempulseError err;
	int deviceCount;

	err = MempulseGetAvailabeDeviceCount(context, &deviceCount);
	EXPECT_EQ(err, MEMPULSE_SUCCESS);
}

TEST_P(TestDevice, nullptr_count) {
	MempulseError err;
	err = MempulseGetAvailabeDeviceCount(context, nullptr);
	EXPECT_EQ(err, MEMPULSE_ERROR_INVALID_PARAMETER);
}

TEST_P(TestDevice, get_memory_info_device_0) {
	MempulseError err;

	int deviceCount;

	err = MempulseGetAvailabeDeviceCount(context, &deviceCount);
	ASSERT_EQ(err, MEMPULSE_SUCCESS);
	ASSERT_GT(deviceCount, 0);

	MempulseDeviceMemoryInfo info;
	err = MempulseGetDeviceMemoryInfo(context, 0, &info);
	EXPECT_EQ(err, MEMPULSE_SUCCESS);

	err = MempulsePrintDeviceMemoryInfo(&info);
	EXPECT_EQ(err, MEMPULSE_SUCCESS);
	EXPECT_GT(info.dedicatedTotal, 0);
	EXPECT_GT(info.dedicatedUsed, 0);
}

TEST_P(TestDevice, get_memory_info_device_0_bad_ptr) {
	MempulseError err;

	int deviceCount;

	err = MempulseGetAvailabeDeviceCount(context, &deviceCount);
	ASSERT_EQ(err, MEMPULSE_SUCCESS);
	ASSERT_GT(deviceCount, 0);

	err = MempulseGetDeviceMemoryInfo(context, 0, nullptr);
	EXPECT_EQ(err, MEMPULSE_ERROR_INVALID_PARAMETER);
}

TEST_P(TestDevice, get_memory_usage_device_0) {
	MempulseError err;

	int deviceCount;

	err = MempulseGetAvailabeDeviceCount(context, &deviceCount);
	ASSERT_EQ(err, MEMPULSE_SUCCESS);
	ASSERT_GT(deviceCount, 0);

	MempulseDeviceMemoryUsage usage;
	err = MempulseGetDeviceMemoryUsage(context, 0, &usage);
	EXPECT_EQ(err, MEMPULSE_SUCCESS);

	err = MempulsePrintDeviceMemoryUsage(&usage);
	EXPECT_EQ(err, MEMPULSE_SUCCESS);
	EXPECT_GT(usage.free, 0);
	EXPECT_GT(usage.total, 0);
}

TEST_P(TestDevice, trim_visible_memory_caps_reported_totals) {
	MempulseError err;

	int deviceCount;
	err = MempulseGetAvailabeDeviceCount(context, &deviceCount);
	ASSERT_EQ(err, MEMPULSE_SUCCESS);
	ASSERT_GT(deviceCount, 0);

	MempulseDeviceMemoryInfo baseInfo;
	err = MempulseGetDeviceMemoryInfo(context, 0, &baseInfo);
	ASSERT_EQ(err, MEMPULSE_SUCCESS);
	ASSERT_GT(baseInfo.dedicatedTotal, 0ULL);

	MempulseDeviceMemoryUsage baseUsage;
	err = MempulseGetDeviceMemoryUsage(context, 0, &baseUsage);
	ASSERT_EQ(err, MEMPULSE_SUCCESS);
	ASSERT_GT(baseUsage.total, 0ULL);

	std::cout << "[trim-test] base info: dedicatedUsed=" << baseInfo.dedicatedUsed
			  << " dedicatedTotal=" << baseInfo.dedicatedTotal
			  << " sharedUsed=" << baseInfo.sharedUsed
			  << " sharedTotal=" << baseInfo.sharedTotal << std::endl;
	std::cout << "[trim-test] base usage: free=" << baseUsage.free
			  << " total=" << baseUsage.total << std::endl;

	const unsigned long long cap = (baseInfo.dedicatedTotal > (64ULL * 1024ULL * 1024ULL))
		? (baseInfo.dedicatedTotal - (64ULL * 1024ULL * 1024ULL))
		: baseInfo.dedicatedTotal;
	ASSERT_GT(cap, 0ULL);
	std::cout << "[trim-test] cap=" << cap << std::endl;

	ScopedEnvVar capEnv("MEMPULSE_TRIM_VISIBLE_MEMORY", std::to_string(cap));

	MempulseDeviceMemoryInfo cappedInfo;
	err = MempulseGetDeviceMemoryInfo(context, 0, &cappedInfo);
	ASSERT_EQ(err, MEMPULSE_SUCCESS);

	MempulseDeviceMemoryUsage cappedUsage;
	err = MempulseGetDeviceMemoryUsage(context, 0, &cappedUsage);
	ASSERT_EQ(err, MEMPULSE_SUCCESS);

	std::cout << "[trim-test] capped info: dedicatedUsed=" << cappedInfo.dedicatedUsed
			  << " dedicatedTotal=" << cappedInfo.dedicatedTotal
			  << " sharedUsed=" << cappedInfo.sharedUsed
			  << " sharedTotal=" << cappedInfo.sharedTotal << std::endl;
	std::cout << "[trim-test] capped usage: free=" << cappedUsage.free
			  << " total=" << cappedUsage.total << std::endl;

	EXPECT_EQ(cappedInfo.dedicatedTotal, std::min(baseInfo.dedicatedTotal, cap));
	EXPECT_EQ(cappedUsage.total, std::min(baseUsage.total, cap));
	EXPECT_LE(cappedUsage.free, cappedUsage.total);
	EXPECT_LE(cappedInfo.dedicatedUsed, cappedInfo.dedicatedTotal);
}

TEST_P(TestDevice, trim_visible_memory_to_24gb) {
	MempulseError err;

	int deviceCount;
	err = MempulseGetAvailabeDeviceCount(context, &deviceCount);
	ASSERT_EQ(err, MEMPULSE_SUCCESS);
	ASSERT_GT(deviceCount, 0);

	MempulseDeviceMemoryInfo baseInfo;
	err = MempulseGetDeviceMemoryInfo(context, 0, &baseInfo);
	ASSERT_EQ(err, MEMPULSE_SUCCESS);

	MempulseDeviceMemoryUsage baseUsage;
	err = MempulseGetDeviceMemoryUsage(context, 0, &baseUsage);
	ASSERT_EQ(err, MEMPULSE_SUCCESS);

	std::cout << "[trim-test-24gb] base info: dedicatedUsed=" << baseInfo.dedicatedUsed
			  << " dedicatedTotal=" << baseInfo.dedicatedTotal
			  << " sharedUsed=" << baseInfo.sharedUsed
			  << " sharedTotal=" << baseInfo.sharedTotal << std::endl;
	std::cout << "[trim-test-24gb] base usage: free=" << baseUsage.free
			  << " total=" << baseUsage.total << std::endl;

	const unsigned long long cap = 24ULL * 1000ULL * 1000ULL * 1000ULL;
	std::cout << "[trim-test-24gb] cap=" << cap << std::endl;
	ScopedEnvVar capEnv("MEMPULSE_TRIM_VISIBLE_MEMORY", std::to_string(cap));

	MempulseDeviceMemoryInfo cappedInfo;
	err = MempulseGetDeviceMemoryInfo(context, 0, &cappedInfo);
	ASSERT_EQ(err, MEMPULSE_SUCCESS);

	MempulseDeviceMemoryUsage cappedUsage;
	err = MempulseGetDeviceMemoryUsage(context, 0, &cappedUsage);
	ASSERT_EQ(err, MEMPULSE_SUCCESS);

	std::cout << "[trim-test-24gb] capped info: dedicatedUsed=" << cappedInfo.dedicatedUsed
			  << " dedicatedTotal=" << cappedInfo.dedicatedTotal
			  << " sharedUsed=" << cappedInfo.sharedUsed
			  << " sharedTotal=" << cappedInfo.sharedTotal << std::endl;
	std::cout << "[trim-test-24gb] capped usage: free=" << cappedUsage.free
			  << " total=" << cappedUsage.total << std::endl;

	EXPECT_EQ(cappedInfo.dedicatedTotal, std::min(baseInfo.dedicatedTotal, cap));
	EXPECT_EQ(cappedUsage.total, std::min(baseUsage.total, cap));
	EXPECT_LE(cappedInfo.dedicatedUsed, cappedInfo.dedicatedTotal);
	EXPECT_LE(cappedUsage.free, cappedUsage.total);
}

// ----------------------------------------------------------------------

#ifdef _WIN32
INSTANTIATE_TEST_SUITE_P(
		Generic,
		TestDevice,
        ::testing::Values(MempulseBackend::MEMPULSE_BACKEND_HIP,
                          MempulseBackend::MEMPULSE_BACKEND_D3DKMT,
                          MempulseBackend::MEMPULSE_BACKEND_ANY));
#else
INSTANTIATE_TEST_SUITE_P(
		Generic,
		TestDevice,
		::testing::Values(MempulseBackend::MEMPULSE_BACKEND_HIP,
                          MempulseBackend::MEMPULSE_BACKEND_DRM,
                          MempulseBackend::MEMPULSE_BACKEND_ANY));
#endif
