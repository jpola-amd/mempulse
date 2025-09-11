#include <gtest/gtest.h>
#include "mempulse/mempulse.h"

struct TestLibrary : public ::testing::Test {
};

TEST_F(TestLibrary, initialization) {
	MempulseError err;
	MempulseContext context;

	err = MempulseInitialize(&context);
	ASSERT_EQ(err, MEMPULSE_SUCCESS);

	err = MempulseShutdown(context);
	EXPECT_EQ(err, MEMPULSE_SUCCESS);
}


TEST_F(TestLibrary, get_version) {
	const char* version = MempulseGetVersion();
	EXPECT_GT(strlen(version), 0) << "library version: \"" << version << "\"" << std::endl;
}


