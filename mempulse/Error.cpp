#include "Error.h"
#include "mempulse.h"
#include <sstream>

namespace {
	std::string constructInvalidParameterMessage(const std::string& parameterName, const std::string& message)
	{
		std::stringstream stream;
		stream << "invalid paramter: \"" << parameterName << "\". " << message;
		return stream.str();
	}
} // namespace anonymous

namespace mempulse {

Error::Error(int err, const std::string& message)
    : std::runtime_error(message)
    , errorCode(err) {
}

ErrorInternal::ErrorInternal(const std::string& message)
    : Error(MEMPULSE_ERROR_INVALID_PARAMETER, "internal error: " + message) {
}


ErrorInvalidParameter::ErrorInvalidParameter(const std::string& parameterName, const std::string& message)
    : Error(MEMPULSE_ERROR_INVALID_PARAMETER, constructInvalidParameterMessage(parameterName, message)) {
}

ErrorInvalidContext::ErrorInvalidContext() :
	Error(MEMPULSE_ERROR_INVALID_CONTEXT, "invalid context") {
}

ErrorInvalidMemory::ErrorInvalidMemory() :
	Error(MEMPULSE_ERROR_INVALID_CONTEXT, "invalid memory") {
}

ErrorInvalidDevice::ErrorInvalidDevice(int deviceId) :
	Error(MEMPULSE_ERROR_INVALID_DEVICE, "invalid device " + std::to_string(deviceId)) {
}

} // namespace mempulse
