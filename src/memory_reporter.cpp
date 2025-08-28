#include "mempulse/mempulse.h"
#include <hip/hip_runtime.h>

namespace mempulse {

static bool g_libraryInitialized = false;

bool Initialize() {
    if (g_libraryInitialized) {
        return true;
    }
    
    hipError_t err = hipInit(0);
    if (err != hipSuccess) {
        return false;
    }
    
    g_libraryInitialized = true;
    return true;
}

void Shutdown() {
    g_libraryInitialized = false;
}

const char* GetVersion() {
    return MEMPULSE_VERSION_STRING;
}

} // namespace mempulse
