#include "MempulseContextImpl.h"
#include "Error.h"
#include "Logging.h"

#ifdef MEMPULSE_BUILD_BACKEND_D3DKMT
#include "backend/d3dkmt/LibraryContextD3dkmt.h"
#endif

#ifdef MEMPULSE_BUILD_BACKEND_HIP
#include "backend/hip/LibraryContextHip.h"
#endif

#ifdef MEMPULSE_BUILD_BACKEND_DRM
#include "backend/drm/LibraryContextDrm.h"
#endif


namespace mempulse {

MempulseContextImpl::MempulseContextImpl(MempulseBackend backend)
{
	MEMPULSE_LOG_TRACE();

	if (backend == MempulseBackend::MEMPULSE_BACKEND_ANY) {
#ifdef MEMPULSE_PLATFORM_WINDOWS
		try {
			m_context = createBackend(MEMPULSE_BACKEND_D3DKMT);
		} catch(std::exception& e) {
			MEMPULSE_LOG_DEBUG(e.what());
		}
#endif
#ifdef MEMPULSE_PLATFORM_LINUX
		try {
			m_context = createBackend(MEMPULSE_BACKEND_DRM);
		} catch(std::exception& e) {
			MEMPULSE_LOG_DEBUG(e.what());
		}

#endif
		if (!m_context)
			m_context = createBackend(MEMPULSE_BACKEND_HIP);
	} else {
		m_context = createBackend(backend);
	}

	if (!m_context) {
		throw ErrorInvalidContext();
	}
}


MempulseContextImpl::LibraryContextPtr MempulseContextImpl::createBackend(MempulseBackend backend)
{
	MEMPULSE_LOG_TRACE();

	switch (backend) {
		case MempulseBackend::MEMPULSE_BACKEND_HIP:
#ifdef MEMPULSE_BUILD_BACKEND_HIP
			return std::make_unique<LibraryContextHip>();
#endif
		break;
		case MempulseBackend::MEMPULSE_BACKEND_DRM:
#ifdef MEMPULSE_BUILD_BACKEND_DRM
			//return std::make_unique<LibraryContextDrm>();
#endif
		break;
		case MempulseBackend::MEMPULSE_BACKEND_D3DKMT:
#ifdef MEMPULSE_BUILD_BACKEND_D3DKMT
			return std::make_unique<LibraryContextD3dkmt>();
#endif
		break;
		default:
			return nullptr;
		break;
	}

	return nullptr;
}

MempulseContextImpl* get_ctx(void* context) {
	MempulseContextImpl* ctx =  static_cast<MempulseContextImpl*>(context);
	if (!ctx)
		throw ErrorInvalidContext();

	return ctx;
}


}
