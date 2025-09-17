#include "DrmVersionPtr.h"

namespace mempulse {
DrmVersionPtr::DrmVersionPtr(drmVersionPtr ver)
: m_pVersion(ver) {
}

DrmVersionPtr::~DrmVersionPtr() {
	if (!m_pVersion)
		return;

	drmFreeVersion(m_pVersion);
}

drmVersionPtr& DrmVersionPtr::operator->() noexcept {
	return m_pVersion;
}

DrmVersionPtr::operator bool() const noexcept {
	return m_pVersion != nullptr;
}

} // namespace mempulse
