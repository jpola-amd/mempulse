#pragma once

#include "File.h"

namespace mempulse {
class DrmFileAmdgpu : public File {
public:
	explicit DrmFileAmdgpu(const char* path);
};
}