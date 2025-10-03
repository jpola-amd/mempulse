#include "Authentificate.h"
#include "ErrorDrm.h"
#include "mempulse/Logging.h"
#include <drm.h>
#include <xf86drm.h>


namespace mempulse {
void Authentificate(const File& file) {
	MEMPULSE_LOG_TRACE();

	drm_magic_t magic;

	/* Obtain magic for our DRM client. */
	if (drmGetMagic(file, &magic) < 0) {
		return;
	}

	/* Try self-authenticate (if we are somehow the master). */
	if (drmAuthMagic(file, magic) == 0) {
		if (drmDropMaster(file)) {
			throw ErrorDrm("failed to call drmDropMaster");
		}
	}
}
}
