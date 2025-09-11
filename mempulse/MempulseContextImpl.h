#pragma once

#include "mempulse.h"
#include "Backend.h"
#include <memory>

namespace mempulse {

class MempulseContextImpl {
public:
	using BackendPtr = std::unique_ptr<Backend>;

	BackendPtr createBackend(MempulseBackend backend);

	MempulseContextImpl(MempulseBackend backend);

	Backend* backend() { return m_context.get(); }
	const Backend* backend() const { return m_context.get(); }

private:
	BackendPtr m_context;
};

MempulseContextImpl* get_ctx(void* context);

} // namespace mempulse
