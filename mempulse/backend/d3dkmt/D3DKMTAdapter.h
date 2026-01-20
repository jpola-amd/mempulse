#pragma once

#include <d3dkmthk.h>

namespace mempulse {

class D3DKMTAdapter {
public:
	D3DKMTAdapter(const LUID& luid);
	~D3DKMTAdapter();

	void close();
                
	void CheckWddm30Caps();

	D3DKMTAdapter(const D3DKMTAdapter&) = delete;
	D3DKMTAdapter& operator=(const D3DKMTAdapter&) = delete;
private:
	D3DKMT_HANDLE m_adapter;
	bool m_isOpen;
};

} // namespace mempulse
