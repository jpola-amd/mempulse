#include "Authentificate.h"
#include <drm.h>
#include <xf86drm.h>
#include <stdexcept>

namespace mempulse {
void Authentificate(const File& file) {
	drm_magic_t magic;

	/* Obtain magic for our DRM client. */
	if (drmGetMagic(file, &magic) < 0) {
		return;
	}

	/* Try self-authenticate (if we are somehow the master). */
	if (drmAuthMagic(file, magic) == 0) {
		if (drmDropMaster(file)) {
			throw std::runtime_error("Failed to call drmDropMaster");
		}
	}
}
}