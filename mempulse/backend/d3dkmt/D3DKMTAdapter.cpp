#include <Windows.h>
#include <d3dkmthk.h>
#include <winternl.h>  // Add this for NT_SUCCESS macro
#pragma comment(lib, "gdi32.lib")

#include "D3DKMTAdapter.h"
#include "BackendD3dkmt.h"
#include "ErrorD3dkmt.h"
#include "mempulse/Error.h"
#include "mempulse/Logging.h"

namespace mempulse {

D3DKMTAdapter::D3DKMTAdapter(const LUID& luid) {
	MEMPULSE_LOG_TRACE();

	D3DKMT_OPENADAPTERFROMLUID open = {};
	open.AdapterLuid = luid;

	NTSTATUS status = D3DKMTOpenAdapterFromLuid(&open);
	check_d3dkmt(status, "D3DKMTAdapter: D3DKMTOpenAdapterFromLuid");

	m_adapter = open.hAdapter;

	m_isOpen = true;
}

D3DKMTAdapter::~D3DKMTAdapter() {
	MEMPULSE_LOG_TRACE();

	try {
		close();
	} catch (const std::exception& e) {
		MEMPULSE_LOG_ERROR(e.what());
	} catch (...) {
		MEMPULSE_LOG_ERROR("D3DKMTAdapter::~D3DKMTAdapter: unknown error");
	}
}

void D3DKMTAdapter::close() {
	MEMPULSE_LOG_TRACE();

	if (!m_isOpen)
		return;

	D3DKMT_CLOSEADAPTER close = {};
	close.hAdapter = m_adapter;

	NTSTATUS status = D3DKMTCloseAdapter(&close);
	check_d3dkmt(status, "D3DKMTAdapter: D3DKMTCloseAdapter");

	m_isOpen = false;
}
	
void D3DKMTAdapter::CheckWddm30Caps() {
	MEMPULSE_LOG_TRACE();

	D3DKMT_WDDM_3_0_CAPS caps = {};

	D3DKMT_QUERYADAPTERINFO query = {};
	query.hAdapter = m_adapter;
	query.Type = KMTQAITYPE_WDDM_3_0_CAPS;
	query.pPrivateDriverData = &caps;
	query.PrivateDriverDataSize = sizeof(caps);

	NTSTATUS status = D3DKMTQueryAdapterInfo(&query);
	check_d3dkmt(status, "D3DKMTAdapter: D3DKMTQueryAdapterInfo - WDDM 3.0 caps query failed");
}

} // namespace mempulse
