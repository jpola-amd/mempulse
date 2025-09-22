#pragma once

#include "mempulse/Error.h"

#include "hip/hip_runtime.h"

namespace mempulse {

class ErrorHip : public Error {
public:
	ErrorHip(hipError_t hipError, const std::string& message);
private:
};

inline void check_hip(hipError_t hipError, const std::string& message) {
	if (hipError == hipSuccess)
		return;

	throw ErrorHip(hipError, message);
}

} // namespace mempulse


