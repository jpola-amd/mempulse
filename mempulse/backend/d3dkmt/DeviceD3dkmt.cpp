#include <Windows.h>
#include <d3dkmthk.h>
#include <dxgi1_6.h>
#include <winternl.h>  // Add this for NT_SUCCESS macro
#include <iomanip>
#include <iostream>
#include <vector>

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "dxgi.lib") 

#include "DeviceD3dkmt.h"
#include "LibraryContextD3dkmt.h"
#include "mempulse/Error.h"
#include "mempulse/Logging.h"

namespace {
static void QuerySegmentGroupUsage(LUID luid, D3DKMT_MEMORY_SEGMENT_GROUP group,
                                   D3DKMT_QUERYSTATISTICS_MEMORY_USAGE& usageInfo) {
    D3DKMT_QUERYSTATISTICS stats = {};
    stats.Type = D3DKMT_QUERYSTATISTICS_SEGMENT_GROUP_USAGE;
    stats.AdapterLuid = luid;
    stats.QuerySegmentGroupUsage.SegmentGroup = group;

    NTSTATUS status = D3DKMTQueryStatistics(&stats);
    if (NT_SUCCESS(status)) {
        usageInfo = stats.QueryResult.SegmentGroupUsageInformation;
    } else {
        throw mempulse::ErrorInternal("D3DKMTQueryStatistics");
    }
}

static inline UINT64 GetTotalMemory(const D3DKMT_QUERYSTATISTICS_MEMORY_USAGE& usage) {
    return usage.AllocatedBytes + usage.FreeBytes + usage.ZeroBytes + usage.ModifiedBytes +
           usage.StandbyBytes;
}
}  // namespace

namespace mempulse {

DeviceD3dkmt::DeviceD3dkmt(const LibraryContextD3dkmt& context, int deviceId)
    : DeviceHip(context, deviceId) {
    MEMPULSE_LOG_TRACE();
}

MempulseDeviceMemoryInfo DeviceD3dkmt::GetMemoryInfo() {
    MEMPULSE_LOG_TRACE();

    LUID dx_luid;
    DeviceHip::Luid hipLuid = luid();
    static_assert(sizeof(dx_luid) == hipLuid.size());
    memcpy(&dx_luid, hipLuid.data(), hipLuid.size());

    MempulseDeviceMemoryInfo memInfo;
    memset(&memInfo, 0, sizeof(memInfo));

    D3DKMT_QUERYSTATISTICS_MEMORY_USAGE localUsage = {};
    memset(&localUsage, 0, sizeof(localUsage));
    QuerySegmentGroupUsage(dx_luid, D3DKMT_MEMORY_SEGMENT_GROUP_LOCAL, localUsage);

    D3DKMT_QUERYSTATISTICS_MEMORY_USAGE nonLocalUsage = {};
    memset(&nonLocalUsage, 0, sizeof(nonLocalUsage));
    if (m_isIntegrated) {
        QuerySegmentGroupUsage(dx_luid, D3DKMT_MEMORY_SEGMENT_GROUP_NON_LOCAL, nonLocalUsage);
    }

    memInfo.dedicatedTotal = GetTotalMemory(localUsage);
    memInfo.dedicatedUsed = localUsage.AllocatedBytes + localUsage.ModifiedBytes;

    memInfo.sharedTotal = GetTotalMemory(nonLocalUsage);
    memInfo.sharedUsed = nonLocalUsage.AllocatedBytes + nonLocalUsage.ModifiedBytes;

    return memInfo;
}

MempulseDeviceMemoryUsage DeviceD3dkmt::GetMemoryUsage() {
    MEMPULSE_LOG_TRACE();

    MempulseDeviceMemoryInfo memInfo = GetMemoryInfo();

    return {
        .free = memInfo.dedicatedTotal - memInfo.dedicatedUsed,
        .total = memInfo.dedicatedTotal,
    };
}

}  // namespace mempulse
