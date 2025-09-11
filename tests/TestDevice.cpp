#include <gtest/gtest.h>
#include "mempulse/mempulse.h"

struct TestDevice : public ::testing::Test {
	void SetUp() {
		MempulseError err;

		err = MempulseInitialize(&context);
		ASSERT_EQ(err, MEMPULSE_SUCCESS);
	}

	void TearDown() {
		MempulseError err;
		err = MempulseShutdown(context);
		EXPECT_EQ(err, MEMPULSE_SUCCESS);
	}

	MempulseContext context;
};

TEST_F(TestDevice, count) {
	MempulseError err;
	int deviceCount;

	err = MempulseGetAvailabeDeviceCount(context, &deviceCount);
	EXPECT_EQ(err, MEMPULSE_SUCCESS);
}

TEST_F(TestDevice, nullptr_count) {
	MempulseError err;
	err = MempulseGetAvailabeDeviceCount(context, nullptr);
	EXPECT_EQ(err, MEMPULSE_INVALID_PARAMETER);
}

TEST_F(TestDevice, get_memory_info_device_0) {
	MempulseError err;

	int deviceCount;

	err = MempulseGetAvailabeDeviceCount(context, &deviceCount);
	ASSERT_EQ(err, MEMPULSE_SUCCESS);
	ASSERT_GT(deviceCount, 0);

	MempulseDeviceMemoryInfo info;
	err = MempulseGetDeviceMemoryInfo(context, 0, &info);
	EXPECT_EQ(err, MEMPULSE_SUCCESS);

	MempulsePrintDeviceMemoryInfo(info);
}

TEST_F(TestDevice, get_memory_info_device_0_bad_ptr) {
	MempulseError err;

	int deviceCount;

	err = MempulseGetAvailabeDeviceCount(context, &deviceCount);
	ASSERT_EQ(err, MEMPULSE_SUCCESS);
	ASSERT_GT(deviceCount, 0);

	err = MempulseGetDeviceMemoryInfo(context, 0, nullptr);
	EXPECT_EQ(err, MEMPULSE_INVALID_PARAMETER);
}

TEST_F(TestDevice, get_memory_usage_device_0) {
	MempulseError err;

	int deviceCount;

	err = MempulseGetAvailabeDeviceCount(context, &deviceCount);
	ASSERT_EQ(err, MEMPULSE_SUCCESS);
	ASSERT_GT(deviceCount, 0);

	MempulseDeviceMemoryUsage usage;
	err = MempulseGetDeviceMemoryUsage(context, 0, &usage);
	EXPECT_EQ(err, MEMPULSE_SUCCESS);

	MempulsePrintDeviceMemoryUsage(usage);
}
