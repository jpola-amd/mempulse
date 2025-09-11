#pragma once

#include "mempulse.h"
#include "LibraryContext.h"
#include <memory>

namespace mempulse {

class MempulseContextImpl {
public:
	using LibraryContextPtr = std::unique_ptr<LibraryContext>;

	LibraryContextPtr createBackend(MempulseBackend backend);

	MempulseContextImpl(MempulseBackend backend);

	LibraryContext* backend() { return m_context.get(); }
	const LibraryContext* backend() const { return m_context.get(); }

private:
	LibraryContextPtr m_context;
};

MempulseContextImpl* get_ctx(void* context);

} // namespace mempulse
