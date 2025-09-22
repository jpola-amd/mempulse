#pragma once

#include "mempulse/Error.h"

#include <Windows.h>
#include <winternl.h>  

namespace mempulse {

class ErrorD3dkmt : public Error {
public:
	explicit ErrorD3dkmt(const std::string& message);
private:
};

inline void check_d3dkmt(NTSTATUS status, const std::string& message) {
    if (!NT_SUCCESS(status))
		throw ErrorD3dkmt(message);
}

} // namespace mempulse


