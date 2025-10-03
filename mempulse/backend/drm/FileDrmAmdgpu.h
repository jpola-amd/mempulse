#pragma once

#include "File.h"

namespace mempulse {
class FileDrmAmdgpu : public File {
public:
	explicit FileDrmAmdgpu(const char* path);
};
}
