#include <gtest/gtest.h>
#include "mempulse/mempulse.h"

using namespace mempulse;

struct TestMeminfo : public ::testing::Test {
	void SetUp() {
		mempulse::Initialize();
	}

	void TearDown() {
		mempulse::Shutdown();
	}
};

TEST_F(TestMeminfo, get_device_count) {
	EXPECT_GT(MemoryReporterFactory::GetDeviceCount(), 0);
}

TEST_F(TestMeminfo, device_mem_info) {
	int deviceCount = MemoryReporterFactory::GetDeviceCount();
	ASSERT_GT(deviceCount, 0) << "no available GPU devices";

    for (int i = 0; i < deviceCount; ++i) {
		const int hipDeviceId = i;
        auto reporter = mempulse::MemoryReporterFactory::CreateMemoryReporter(hipDeviceId, false);
		ASSERT_TRUE(reporter) << "can't create reporter for device " << hipDeviceId;

		EXPECT_GE(reporter->GetDeviceId(), 0) << "bad device id";
		EXPECT_FALSE(reporter->GetDeviceName().empty()) << "null device name";

    	const mempulse::MemoryInfo memInfo = reporter->GetMemoryInfo();

		//EXPECT_GT(memInf

		std::cout << memInfo << std::endl;

	}

}


