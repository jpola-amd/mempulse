#include "mempulse/mempulse.h"
#include <hip/hip_runtime.h>
#include "utils.h"

namespace mempulse {

void Initialize() {
    hipError_t err = hipInit(0);
	CHECK_HIP(err, "failed to initalize hip");
}

void Shutdown() {
}

const char* GetVersion() {
    return MEMPULSE_VERSION_STRING;
}

} // namespace mempulse
