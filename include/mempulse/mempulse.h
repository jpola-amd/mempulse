#pragma once

#include "config.h"
#include "memory_reporter.h"
#include "memory_reporter_factory.h"

namespace mempulse {

/**
 * Initialize MemPulse library
 * Call this before using any other MemPulse functions
 * @return true if initialization was successful
 */    
bool Initialize();

/**
 * Shutdown MemPulse library
 * Call this when done using MemPulse to cleanup resources
 */
void Shutdown();

/**
 * Get library version string
 * @return Version string
 */
const char* GetVersion();

} // namespace mempulse