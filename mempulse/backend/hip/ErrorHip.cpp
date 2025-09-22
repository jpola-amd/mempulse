#include "ErrorHip.h"
#include "mempulse/mempulse.h"
#include <sstream>

namespace {

std::string constructErrorMessage(hipError_t hipError, const std::string& message) {
	std::stringstream stream;

	stream << message << "\n";
	stream << hipGetErrorName(hipError) << "\n";
	stream << hipGetErrorString(hipError) << "\n";

	return stream.str();
}

} // namespace anonymous

namespace mempulse {

ErrorHip::ErrorHip(hipError_t hipError, const std::string& message)
: Error(MEMPULSE_ERROR_HIP, constructErrorMessage(hipError, message))
{
}

} // namespace mempulse
