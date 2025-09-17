#pragma once

#include <vector>
#include <xf86drm.h>

namespace mempulse {
class DrmDevices {
public:
	using container_type = std::vector<drmDevicePtr>;

	DrmDevices();
	~DrmDevices();

	[[nodiscard]]
	const container_type& Get()	const noexcept { return m_devices; }

	[[nodiscard]]
	size_t Size() const noexcept { return m_devices.size(); }

	[[nodiscard]]
	container_type::value_type& operator[](size_t index) noexcept { return m_devices[index]; }

	[[nodiscard]]
	const container_type::value_type& operator[](size_t index) const noexcept { return m_devices[index]; }

	DrmDevices(const DrmDevices &)            = delete;
	DrmDevices &operator=(const DrmDevices &) = delete;
private:
	container_type m_devices;

};
}