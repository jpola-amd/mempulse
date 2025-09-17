#pragma once

#include "File.h"
#include <xf86drm.h>

namespace mempulse {
class DrmDevice {
public:
	explicit DrmDevice(const File&);
	~DrmDevice();

	[[nodiscard]]
	short GetBus() const;

	[[nodiscard]]
	unsigned int GetDeviceId() const;

	DrmDevice(const DrmDevice &) = delete;
	DrmDevice &operator=(const DrmDevice &) = delete;
private:
	drmDevicePtr m_pDrmDevicePtr;

};

}
