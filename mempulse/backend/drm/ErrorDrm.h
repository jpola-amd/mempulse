#pragma once

#include "mempulse/Error.h"

namespace mempulse {

class ErrorDrm : public Error {
public:
	ErrorDrm(const std::string& message);
};

inline void check_drm(int status, const std::string& message) {
	if (status == 0)
		return;

	throw ErrorDrm(message);

}

} // namepsace mempulse
