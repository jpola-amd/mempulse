#pragma once

static int MEMPULSE_LOGGING_LEVEL = 1;

#include <iostream>
#define MEMPULSE_LOG_TRACE()    if (MEMPULSE_LOGGING_LEVEL >= 3) std::cout << "[TRACE] " << __FILE__ << ":" << __LINE__ << " " << __func__ << "\n"
#define MEMPULSE_LOG_DEBUG(msg) if (MEMPULSE_LOGGING_LEVEL >= 2) std::cout << "[DEBUG] " << msg << "\n"
#define MEMPULSE_LOG_ERROR(msg) if (MEMPULSE_LOGGING_LEVEL >= 1) std::cerr << "[ERROR] " << msg << "\n"

