#include "Error.h"
#include "mempulse.h"

namespace mempulse {

Error::Error(int err, const std::string& message)
    : std::runtime_error(message)
    , errorCode(err) {
}

ErrorInternal::ErrorInternal(const std::string& message)
    : Error(MEMPULSE_INTERNAL_ERROR, "Internal error: " + message) {
}

ErrorInvalidContext::ErrorInvalidContext() : Error(MEMPULSE_BAD_CONTEXT, "Invalid context") {
}

ErrorInvalidParameter::ErrorInvalidParameter(const std::string& parameterName, const std::string& message)
    : Error(MEMPULSE_INVALID_PARAMETER, "Invalid parameter: \"" + parameterName + "\". " + message) {
}

ErrorInvalidDevice::ErrorInvalidDevice(int deviceId)
    : Error(MEMPULSE_BAD_DEVICE_INDEX, "Bad gpu device index: " + std::to_string(deviceId)) {
}

} // namespace mempulse
