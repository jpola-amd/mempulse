#include "DrmFileAmdgpu.h"

#include <cstring>
#include <stdexcept>

#include "DrmVersionPtr.h"
#include "Authentificate.h"

namespace mempulse {

DrmFileAmdgpu::DrmFileAmdgpu(const char* path)
	: File(path, Mode::ReadWrite) {

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

}
