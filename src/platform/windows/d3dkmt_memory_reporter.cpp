#include "d3dkmt_memory_reporter.h"
#include "../../utils/pci_utils.h"
#include <d3dkmthk.h>
#include <winternl.h> 
#pragma comment(lib, "gdi32.lib")

#include <cassert>
#define assert_with_message(condition, message) \
    ((condition) ? static_cast<void>(0) : [&]{ std::cerr << "Assertion failed: " << message << std::endl; assert(false); }())



namespace mempulse {



// std::string PciBusInfo::ToString() const {
//     return "Domain: " + std::to_string(domain) +
//            ", Bus: " + std::to_string(bus) +
//            ", Device: " + std::to_string(device) +
//            ", Function: " + std::to_string(function);
// }

// static PciBusInfo ParseHipPciBusId(int pciBusId) {
//     PciBusInfo busInfo;

//     // Extract PCI components from integer representation
//     // This format may vary by HIP implementation, but commonly:
//     // bits 0-7: function and device
//     // bits 8-15: bus
//     // bits 16-31: domain (if present)

//     busInfo.function = pciBusId & 0x7;
//     busInfo.device = (pciBusId >> 3) & 0x1F;
//     busInfo.bus = (pciBusId >> 8) & 0xFF;
//     busInfo.domain = (pciBusId >> 16) & 0xFFFF;

//     return busInfo;
// }

 
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


bool D3DKMTMemoryReporter::Initialize() {
    if (m_initialized) {
        return true; // Already initialized
    }
    m_initialized = true;
    return true;
}

void D3DKMTMemoryReporter::ShutDown() {
    m_initialized = false;
}


bool D3DKMTMemoryReporter::GetMemoryInfo(MemoryInfo& memInfo) const {
    if (!m_initialized) {
        return false;
    }

    D3DKMT_QUERYSTATISTICS_MEMORY_USAGE localUsage = {};
    memset(&localUsage, 0, sizeof(localUsage));
    if (!QuerySegmentGroupUsage(m_luid, D3DKMT_MEMORY_SEGMENT_GROUP_LOCAL, localUsage)) {
        return false;
    }

    D3DKMT_QUERYSTATISTICS_MEMORY_USAGE nonLocalUsage = {};
    memset(&nonLocalUsage, 0, sizeof(nonLocalUsage));
    if (m_isIntegrated) {
        if (!QuerySegmentGroupUsage(m_luid, D3DKMT_MEMORY_SEGMENT_GROUP_NON_LOCAL, nonLocalUsage)) {
            return false;
        }
    }

    memInfo.dedicatedTotal = GetTotalMemory(localUsage);
    memInfo.dedicatedUsed = localUsage.AllocatedBytes + localUsage.ModifiedBytes;

    memInfo.sharedTotal = GetTotalMemory(nonLocalUsage);
    memInfo.sharedUsed = nonLocalUsage.AllocatedBytes + nonLocalUsage.ModifiedBytes;
    
    return true;
}


bool D3DKMTMemoryReporter::GetMemoryUsage(size_t& free, size_t& total) const {

    if (!m_initialized) {
        return false;
    }
    MemoryInfo memInfo;
    if (!GetMemoryInfo(memInfo)) {
        return false;
    }

    total = memInfo.dedicatedTotal + memInfo.sharedTotal;
    free = total - (memInfo.dedicatedUsed + memInfo.sharedUsed);
    return true;
}

} // namespace mempulse
