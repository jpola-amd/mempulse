#pragma once

#include <stdexcept>
#include <sstream>

namespace mempulse {

#define CHECK_HIP(status, message) if (status != hipSuccess) { \
	std::stringstream str; \
	str << "[ERROR] " << __FILE__ << ":" << __LINE__  <<" " << message << ": " << hipGetErrorName(status) << " - " << hipGetErrorString(status); \
	throw std::runtime_error(str.str()); }

} // namespace mempulse


