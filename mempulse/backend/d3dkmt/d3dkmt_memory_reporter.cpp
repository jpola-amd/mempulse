#include "d3dkmt_memory_reporter.h"
#include <d3dkmthk.h>
#include <winternl.h>
#pragma comment(lib, "gdi32.lib")

#include "../../assert.h"

namespace mempulse {

static bool QuerySegmentGroupUsage(LUID luid, D3DKMT_MEMORY_SEGMENT_GROUP group, D3DKMT_QUERYSTATISTICS_MEMORY_USAGE& usageInfo) {
    D3DKMT_QUERYSTATISTICS stats = {};
    stats.Type = D3DKMT_QUERYSTATISTICS_SEGMENT_GROUP_USAGE;
    stats.AdapterLuid = luid;
    stats.QuerySegmentGroupUsage.SegmentGroup = group;

    NTSTATUS status = D3DKMTQueryStatistics(&stats);
    if (NT_SUCCESS(status)) {
        usageInfo = stats.QueryResult.SegmentGroupUsageInformation;
        return true;
    } else {
        assert_with_message(false, "Failed to query segment group usage.");
        return false;
    }
}

static inline UINT64 GetTotalMemory(const D3DKMT_QUERYSTATISTICS_MEMORY_USAGE& usage) {
    return usage.AllocatedBytes + usage.FreeBytes + usage.ZeroBytes +
           usage.ModifiedBytes + usage.StandbyBytes;
}

D3DKMTMemoryReporter::D3DKMTMemoryReporter(int hipDeviceId, bool isIntegrated, LUID luid)
    : m_hipDeviceId(hipDeviceId), m_isIntegrated(isIntegrated), m_luid(luid)
    {}


MemoryInfo D3DKMTMemoryReporter::GetMemoryInfo() const {
	MemoryInfo memInfo;

    D3DKMT_QUERYSTATISTICS_MEMORY_USAGE localUsage = {};
    memset(&localUsage, 0, sizeof(localUsage));
    if (!QuerySegmentGroupUsage(m_luid, D3DKMT_MEMORY_SEGMENT_GROUP_LOCAL, localUsage)) {
        throw std::runtime_error("QuerySegmentGroupUsage");
    }

    D3DKMT_QUERYSTATISTICS_MEMORY_USAGE nonLocalUsage = {};
    memset(&nonLocalUsage, 0, sizeof(nonLocalUsage));
    if (m_isIntegrated) {
        if (!QuerySegmentGroupUsage(m_luid, D3DKMT_MEMORY_SEGMENT_GROUP_NON_LOCAL, nonLocalUsage)) {
        	throw std::runtime_error("QuerySegmentGroupUsage");
        }
    }

    memInfo.dedicatedTotal = GetTotalMemory(localUsage);
    memInfo.dedicatedUsed = localUsage.AllocatedBytes + localUsage.ModifiedBytes;

    memInfo.sharedTotal = GetTotalMemory(nonLocalUsage);
    memInfo.sharedUsed = nonLocalUsage.AllocatedBytes + nonLocalUsage.ModifiedBytes;

    return memInfo;
}


MemoryUsage D3DKMTMemoryReporter::GetMemoryUsage(size_t& free, size_t& total) const {

    MemoryInfo memInfo = GetMemoryInfo(memInfo);

    total = memInfo.dedicatedTotal + memInfo.sharedTotal;
    free = total - (memInfo.dedicatedUsed + memInfo.sharedUsed);
    return {
		.free = free,
		.total = total,
	};
}

} // namespace mempulse
