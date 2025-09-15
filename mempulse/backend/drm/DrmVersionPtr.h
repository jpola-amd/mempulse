#pragma once

#include <xf86drm.h>

class DrmVersionPtr {
public:
	explicit DrmVersionPtr(drmVersionPtr ver = nullptr) : m_pVersion(ver) {}
	~DrmVersionPtr() {
		if (m_pVersion) {
			drmFreeVersion(m_pVersion);
		}
	}

	drmVersionPtr& operator->() noexcept { return m_pVersion; }
	operator bool() const noexcept { return m_pVersion != nullptr; }

	DrmVersionPtr(const DrmVersionPtr&)            = delete;
	DrmVersionPtr& operator=(const DrmVersionPtr&) = delete;
private:
	drmVersionPtr m_pVersion;
};
