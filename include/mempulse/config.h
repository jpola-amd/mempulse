#pragma once

/**
 * MemPulse Configuration Header
 * 
 * This file contains build-time configuration and platform detection
 */

// Version information
#define MEMPULSE_VERSION_MAJOR 1
#define MEMPULSE_VERSION_MINOR 0
#define MEMPULSE_VERSION_PATCH 0
#define MEMPULSE_VERSION_STRING "1.0.0"

// Platform detection
#ifdef _WIN32
    #define MEMPULSE_PLATFORM_WINDOWS 1
    #define MEMPULSE_PLATFORM_LINUX 0
#else
    #define MEMPULSE_PLATFORM_WINDOWS 0
    #define MEMPULSE_PLATFORM_LINUX 1
#endif

// API export/import macros for Windows
#ifdef _WIN32
    #ifdef MEMPULSE_EXPORTS
        #define MEMPULSE_API __declspec(dllexport)
    #else
        #define MEMPULSE_API __declspec(dllimport)
    #endif
#else
    #define MEMPULSE_API __attribute__((visibility("default")))
#endif

// Compiler-specific attributes
#ifdef __GNUC__
    #define MEMPULSE_DEPRECATED __attribute__((deprecated))
    #define MEMPULSE_UNUSED __attribute__((unused))
#elif defined(_MSC_VER)
    #define MEMPULSE_DEPRECATED __declspec(deprecated)
    #define MEMPULSE_UNUSED __pragma(warning(suppress:4100))
#else
    #define MEMPULSE_DEPRECATED
    #define MEMPULSE_UNUSED
#endif

// Debug build detection
#ifdef NDEBUG
    #define MEMPULSE_DEBUG 0
#else
    #define MEMPULSE_DEBUG 1
#endif

// Feature availability
#if MEMPULSE_PLATFORM_WINDOWS
    #define MEMPULSE_HAS_DXGI 1
    #define MEMPULSE_HAS_HIP_MEMORY_BUDGET 0
#else
    #define MEMPULSE_HAS_DXGI 0
    #define MEMPULSE_HAS_HIP_MEMORY_BUDGET 1
#endif

// Logging and debugging
#if MEMPULSE_DEBUG
    #include <iostream>
    #define MEMPULSE_LOG_DEBUG(msg) std::cout << "[DEBUG] " << msg << std::endl
    #define MEMPULSE_LOG_ERROR(msg) std::cerr << "[ERROR] " << msg << std::endl
#else
    #define MEMPULSE_LOG_DEBUG(msg) ((void)0)
    #define MEMPULSE_LOG_ERROR(msg) ((void)0)
#endif

// Memory alignment
#define MEMPULSE_MEMORY_ALIGNMENT 64

// Maximum number of devices to support
#define MEMPULSE_MAX_DEVICES 16

// Buffer sizes
#define MEMPULSE_MAX_DEVICE_NAME_LENGTH 256
#define MEMPULSE_MAX_PCI_BUS_ID_LENGTH 32
