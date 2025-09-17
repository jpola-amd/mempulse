#pragma once

#include <cassert>
#if defined(MEMPULSE_ASSERT_ENABLED)
#include <iostream>
#define assert_with_message(condition, message) \
    ((condition) ? static_cast<void>(0) : [&]{ std::cerr << "Assertion failed: " << message << std::endl; assert(false); }())
#else
#define assert_with_message(condition, message) ((void)0)
#endif
