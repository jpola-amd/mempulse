#include <gtest/gtest.h>
#include "mempulse/mempulse.h"

struct TestLibrary : public ::testing::Test {
};

TEST_F(TestLibrary, initialization) {
	mempulse::Initialize();
	mempulse::Shutdown();
}

TEST_F(TestLibrary, bad_initialization) {
	mempulse::Initialize();
	mempulse::Initialize();

	mempulse::Shutdown();
}

TEST_F(TestLibrary, bad_shutdown) {
	mempulse::Initialize();

	mempulse::Shutdown();
	mempulse::Shutdown();
}

TEST_F(TestLibrary, get_version) {
	const char* version = mempulse::GetVersion();
	EXPECT_GT(strlen(version), 0) << "library version: \"" << version << "\"" << std::endl;
}


