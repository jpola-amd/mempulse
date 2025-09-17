#include "DrmDevices.h"
#include "mempulse/Logging.h"

#include <stdexcept>
#include <cassert>

namespace mempulse {

DrmDevices::DrmDevices() {
	MEMPULSE_LOG_TRACE();

	int count = drmGetDevices2(0, nullptr, 0);
	if (count < 0)
		throw std::runtime_error("drmGetDevices2 failed");

	m_devices.resize(count);

	count = drmGetDevices2(0, m_devices.data(), m_devices.size());
	if (count < 0) {
		throw std::runtime_error("drmGetDevices2(data) failed");
	}

	assert(count == static_cast<int>(m_devices.size()));
}

DrmDevices::~DrmDevices() {
	MEMPULSE_LOG_TRACE();

	drmFreeDevices(m_devices.data(), m_devices.size());
}


}
