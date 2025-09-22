#include "FileDrmAmdgpu.h"
#include "ErrorDrm.h"
#include "DrmVersionPtr.h"
#include "Authentificate.h"

#include "mempulse/Logging.h"

#include <cstring>


namespace mempulse {

FileDrmAmdgpu::FileDrmAmdgpu(const char* path)
	: File(path, Mode::ReadWrite) {

	MEMPULSE_LOG_TRACE();

	DrmVersionPtr ver(drmGetVersion(*this));
	if (!ver) {
		throw ErrorDrm("failed to query driver version");
	}
	// TODO: print driver version + tag

	Authentificate(*this);

	if (strcmp(ver->name, "amdgpu") != 0) {
		throw ErrorDrm("unsupported driver " + std::string(ver->name));
	}
}

} // namespace mempulse
