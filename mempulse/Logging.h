#pragma once

// Logging and debugging
#if MEMPULSE_DEBUG
    #include <iostream>
    #define MEMPULSE_LOG_TRACE() std::cout << "[TRACE] " << __FILE__ << ":" << __LINE__ << " " << __func__ << "\n"
    #define MEMPULSE_LOG_DEBUG(msg) std::cout << "[DEBUG] " << msg << "\n"
    #define MEMPULSE_LOG_ERROR(msg) std::cerr << "[ERROR] " << msg << "\n"
#else
    #define MEMPULSE_LOG_TRACE() ((void)0)
    #define MEMPULSE_LOG_DEBUG(msg) ((void)0)
    #define MEMPULSE_LOG_ERROR(msg) ((void)0)
#endif
