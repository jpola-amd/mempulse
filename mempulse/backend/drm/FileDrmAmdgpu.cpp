#include "FileDrmAmdgpu.h"
#include "DrmVersionPtr.h"
#include "Authentificate.h"
#include "mempulse/Logging.h"

#include <cstring>
#include <stdexcept>

namespace mempulse {

FileDrmAmdgpu::FileDrmAmdgpu(const char* path)
	: File(path, Mode::ReadWrite) {

	MEMPULSE_LOG_TRACE();

	DrmVersionPtr ver(drmGetVersion(*this));
	if (!ver) {
		throw std::runtime_error("Failed to query driver version");
	}
	// TODO: print driver version + tag

	Authentificate(*this);

	if (strcmp(ver->name, "amdgpu") != 0) {
		throw std::runtime_error("Unsupported driver " + std::string(ver->name));
	}
}

} // namespace mempulse
