#pragma once

#include <xf86drm.h>

namespace mempulse {

class DrmVersionPtr {
public:
	explicit DrmVersionPtr(drmVersionPtr ver = nullptr);
	~DrmVersionPtr();

	drmVersionPtr& operator->() noexcept;
	operator bool() const noexcept;

	DrmVersionPtr(const DrmVersionPtr&)            = delete;
	DrmVersionPtr& operator=(const DrmVersionPtr&) = delete;
private:
	drmVersionPtr m_pVersion;
};

}
