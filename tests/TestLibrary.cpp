#include <gtest/gtest.h>
#include "mempulse/mempulse.h"

struct TestLibrary : public ::testing::TestWithParam<MempulseBackend> {
};

TEST_P(TestLibrary, initialization) {
	MempulseError err;
	MempulseContext context;

	err = MempulseInitialize(&context, GetParam());
	ASSERT_EQ(err, MEMPULSE_SUCCESS);

	err = MempulseShutdown(context);
	EXPECT_EQ(err, MEMPULSE_SUCCESS);
}

TEST_P(TestLibrary, get_version) {
	const char* version = MempulseGetVersion();
	EXPECT_GT(strlen(version), 0) << "library version: \"" << version << "\"" << std::endl;
}

#ifdef _WIN32
INSTANTIATE_TEST_SUITE_P(
		GenericInit,
		TestLibrary,
        ::testing::Values(
			MempulseBackend::MEMPULSE_BACKEND_HIP,
            MempulseBackend::MEMPULSE_BACKEND_D3DKMT,
            MempulseBackend::MEMPULSE_BACKEND_ANY));
#else
INSTANTIATE_TEST_SUITE_P(
		GenericInit,
		TestLibrary,
		::testing::Values(
			MempulseBackend::MEMPULSE_BACKEND_HIP,
			MempulseBackend::MEMPULSE_BACKEND_DRM,
			MempulseBackend::MEMPULSE_BACKEND_ANY));

#endif

