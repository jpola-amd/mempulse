#include <gtest/gtest.h>
#include "mempulse/mempulse.h"

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

	MempulsePrintDeviceMemoryInfo(info);
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

	MempulsePrintDeviceMemoryUsage(usage);
	EXPECT_GT(usage.free, 0);
	EXPECT_GT(usage.total, 0);
}

// ----------------------------------------------------------------------

#ifdef _WIN32
INSTANTIATE_TEST_SUITE_P(
		TestBackend,
		TestDevice,
        ::testing::Values(MempulseBackend::MEMPULSE_BACKEND_HIP,
                          MempulseBackend::MEMPULSE_BACKEND_D3DKMT,
                          MempulseBackend::MEMPULSE_BACKEND_ANY));
#else
INSTANTIATE_TEST_SUITE_P(
		TestBackend,
		TestDevice,
		::testing::Values(MempulseBackend::MEMPULSE_BACKEND_HIP,
                          MempulseBackend::MEMPULSE_BACKEND_DRM,
                          MempulseBackend::MEMPULSE_BACKEND_ANY));
#endif
