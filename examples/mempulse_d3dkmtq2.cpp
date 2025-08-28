#include <Windows.h>
#include <dxgi1_6.h>
#include <iostream>
#include <vector>
#include <d3dkmthk.h>
#pragma comment(lib, "gdi32.lib")

struct GpuInfo {
    LUID luid;
    std::wstring name;
    bool isDiscrete;
    UINT physicalAdapterIndex;
    bool supportsWDDM31;
};

std::vector<GpuInfo> EnumerateGpus() {
    std::vector<GpuInfo> gpus;
    IDXGIFactory6* factory = nullptr;
    if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&factory)))) {
        throw std::runtime_error("Failed to create DXGI factory");
    }

    IDXGIAdapter1* adapter = nullptr;
    for (UINT i = 0; factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; i++) {
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);

        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
            adapter->Release();
            continue;
        }

        bool discrete = (desc.DedicatedVideoMemory > 0);
        gpus.push_back({ desc.AdapterLuid, desc.Description, discrete, i, false });

        adapter->Release();
    }

    factory->Release();
    return gpus;
}

bool TestWDDM31Support(const GpuInfo& gpu) {
    // Test WDDM 3.1 ADAPTER2 query
    D3DKMT_QUERYSTATISTICS testStats = {};
    testStats.Type =   D3DKMT_QUERYSTATISTICS_TYPE::D3DKMT_QUERYSTATISTICS_ADAPTER2;
    testStats.AdapterLuid = gpu.luid;
    testStats.QueryAdapter2.PhysicalAdapterIndex = gpu.physicalAdapterIndex;

    NTSTATUS status1 = D3DKMTQueryStatistics(&testStats);
    

    
    // Try SEGMENT_USAGE query as additional test
    memset(&testStats, 0, sizeof(testStats));
    testStats.Type = D3DKMT_QUERYSTATISTICS_TYPE::D3DKMT_QUERYSTATISTICS_SEGMENT_USAGE;
    testStats.AdapterLuid = gpu.luid;
    
    NTSTATUS status2 = D3DKMTQueryStatistics(&testStats);
    return (status1 == 0 && status2 == 0);
}

void QuerySegmentUsageWDDM31(const GpuInfo& gpu) {
    std::wcout << L"\n=== WDDM 3.1 Segment Usage ===\n";

    // First get adapter info using ADAPTER2
    D3DKMT_QUERYSTATISTICS adapterStats = {};
    adapterStats.Type = D3DKMT_QUERYSTATISTICS_TYPE::D3DKMT_QUERYSTATISTICS_ADAPTER2;
    adapterStats.AdapterLuid = gpu.luid;
    adapterStats.QueryAdapter2.PhysicalAdapterIndex = gpu.physicalAdapterIndex;

    NTSTATUS status = D3DKMTQueryStatistics(&adapterStats);
    if (status != 0) {
        std::wcerr << L"Failed to query adapter2 info: 0x" << std::hex << status << L"\n";
        return;
    }

    UINT numSegments = adapterStats.QueryResult.AdapterInformation.NbSegments;
    std::wcout << L"Number of segments: " << numSegments << L"\n";

    // Query each segment's usage using SEGMENT_USAGE
    for (UINT segmentId = 0; segmentId < numSegments && segmentId < 8; segmentId++) {
        D3DKMT_QUERYSTATISTICS segmentUsageStats = {};
        segmentUsageStats.Type = D3DKMT_QUERYSTATISTICS_TYPE::D3DKMT_QUERYSTATISTICS_SEGMENT_USAGE;
        segmentUsageStats.AdapterLuid = gpu.luid;
        segmentUsageStats.QuerySegmentUsage.PhysicalAdapterIndex = gpu.physicalAdapterIndex;
        segmentUsageStats.QuerySegmentUsage.SegmentId = segmentId;

        status = D3DKMTQueryStatistics(&segmentUsageStats);
        if (status == 0) {
            // The result should be in SegmentUsageInformation
            // Try to access the memory usage data
            UINT64* resultData = reinterpret_cast<UINT64*>(&segmentUsageStats.QueryResult);
            
            UINT64 allocatedBytes = resultData[0];
            UINT64 freeBytes = resultData[1];
            
            std::wcout << L"\nSegment " << segmentId << L" Usage:\n";
            std::wcout << L"  Allocated: " << allocatedBytes / (1024*1024) << L" MB\n";
            std::wcout << L"  Free: " << freeBytes / (1024*1024) << L" MB\n";
            
            UINT64 totalMB = (allocatedBytes + freeBytes) / (1024*1024);
            UINT64 usedMB = allocatedBytes / (1024*1024);
            if (totalMB > 0) {
                double usagePercent = (double)usedMB / totalMB * 100.0;
                std::wcout << L"  Usage: " << usedMB << L"/" << totalMB << L" MB (" << usagePercent << L"%)\n";
            }
        } else {
            std::wcout << L"Segment " << segmentId << L": Query failed (0x" << std::hex << status << L")\n";
        }
    }
}

void QuerySegmentGroupUsageWDDM31(const GpuInfo& gpu) {
    std::wcout << L"\n=== WDDM 3.1 Segment Group Usage ===\n";

    // Query Local Memory Group using SEGMENT_GROUP_USAGE
    D3DKMT_QUERYSTATISTICS localStats = {};
    localStats.Type = D3DKMT_QUERYSTATISTICS_TYPE::D3DKMT_QUERYSTATISTICS_SEGMENT_GROUP_USAGE;
    localStats.AdapterLuid = gpu.luid;
    localStats.QuerySegmentGroupUsage.PhysicalAdapterIndex = gpu.physicalAdapterIndex;
    
    
    NTSTATUS status = D3DKMTQueryStatistics(&localStats);
    if (status == 0) {
        auto& resultData = localStats.QueryResult.SegmentGroupUsageInformation;
        UINT64 allocatedBytes = resultData.AllocatedBytes;
        UINT64 freeBytes = resultData.FreeBytes;

        std::wcout << L"Local Memory (Dedicated VRAM):\n";
        std::wcout << L"  Allocated: " << allocatedBytes / (1024*1024) << L" MB\n";
        std::wcout << L"  Free: " << freeBytes / (1024*1024) << L" MB\n";
        
        UINT64 totalMB = (allocatedBytes + freeBytes) / (1024*1024);
        UINT64 usedMB = allocatedBytes / (1024*1024);
        if (totalMB > 0) {
            double usagePercent = (double)usedMB / totalMB * 100.0;
            std::wcout << L"  Usage: " << usedMB << L"/" << totalMB << L" MB (" << usagePercent << L"%)\n";
        }
    } else {
        std::wcout << L"Local memory query failed (0x" << std::hex << status << L")\n";
    }

    // Query Non-Local Memory Group
    D3DKMT_QUERYSTATISTICS nonLocalStats = {};
    nonLocalStats.Type = D3DKMT_QUERYSTATISTICS_TYPE::D3DKMT_QUERYSTATISTICS_SEGMENT_GROUP_USAGE;
    nonLocalStats.AdapterLuid = gpu.luid;
    nonLocalStats.QuerySegmentGroupUsage.PhysicalAdapterIndex = gpu.physicalAdapterIndex;

    status = D3DKMTQueryStatistics(&nonLocalStats);
    if (status == 0) {
        auto resultData = nonLocalStats.QueryResult.SegmentGroupUsageInformation;

        UINT64 allocatedBytes = resultData.AllocatedBytes;
        UINT64 freeBytes = resultData.FreeBytes;

        std::wcout << L"\nNon-Local Memory (System RAM):\n";
        std::wcout << L"  Allocated: " << allocatedBytes / (1024*1024) << L" MB\n";
        std::wcout << L"  Free: " << freeBytes / (1024*1024) << L" MB\n";
        
        UINT64 totalMB = (allocatedBytes + freeBytes) / (1024*1024);
        UINT64 usedMB = allocatedBytes / (1024*1024);
        if (totalMB > 0) {
            double usagePercent = (double)usedMB / totalMB * 100.0;
            std::wcout << L"  Usage: " << usedMB << L"/" << totalMB << L" MB (" << usagePercent << L"%)\n";
        }
    } else {
        std::wcout << L"Non-local memory query failed (0x" << std::hex << status << L")\n";
    }
}

void QueryGpuMemoryUsageWDDM21(const GpuInfo& gpu) {
    std::wcout << L"\n=== WDDM 2.1+ Memory Query ===\n";

    // Query adapter info to get segment count
    D3DKMT_QUERYSTATISTICS adapterStats = {};
    adapterStats.Type = D3DKMT_QUERYSTATISTICS_ADAPTER;
    adapterStats.AdapterLuid = gpu.luid;

    NTSTATUS status = D3DKMTQueryStatistics(&adapterStats);
    if (status != 0) {
        std::wcerr << L"Failed to query adapter info: 0x" << std::hex << status << L"\n";
        return;
    }

    UINT numSegments = adapterStats.QueryResult.AdapterInformation.NbSegments;
    std::wcout << L"Number of segments: " << numSegments << L"\n";

    // Query individual segments
    for (UINT segmentId = 0; segmentId < numSegments; segmentId++) {
        D3DKMT_QUERYSTATISTICS segmentStats = {};
        segmentStats.Type = D3DKMT_QUERYSTATISTICS_SEGMENT;
        segmentStats.AdapterLuid = gpu.luid;
        segmentStats.QuerySegment.SegmentId = segmentId;

        status = D3DKMTQueryStatistics(&segmentStats);
        if (status == 0) {
            auto& segInfo = segmentStats.QueryResult.SegmentInformation;
            
            ULONGLONG commitLimitMB = segInfo.CommitLimit / (1024 * 1024);
            ULONGLONG bytesCommittedMB = segInfo.BytesCommitted / (1024 * 1024);
            ULONGLONG bytesResidentMB = segInfo.BytesResident / (1024 * 1024);
            
            std::wcout << L"\nSegment " << segmentId << L":\n";
            std::wcout << L"  Commit Limit: " << commitLimitMB << L" MB\n";
            std::wcout << L"  Committed: " << bytesCommittedMB << L" MB\n";
            std::wcout << L"  Resident: " << bytesResidentMB << L" MB\n";
            std::wcout << L"  Aperture: " << (segInfo.Aperture ? L"Yes" : L"No") << L"\n";
            
            if (commitLimitMB > 0) {
                double usagePercent = (double)bytesCommittedMB / commitLimitMB * 100.0;
                std::wcout << L"  Usage: " << usagePercent << L"%\n";
            }
        }
    }

    // Try segment group queries (WDDM 2.1+)
    std::wcout << L"\n=== Segment Group Memory ===\n";

    // Local memory group
    D3DKMT_QUERYSTATISTICS localStats = {};
    localStats.Type = D3DKMT_QUERYSTATISTICS_PROCESS_SEGMENT_GROUP;
    localStats.AdapterLuid = gpu.luid;
    localStats.QueryProcessSegmentGroup = D3DKMT_MEMORY_SEGMENT_GROUP_LOCAL;

    status = D3DKMTQueryStatistics(&localStats);
    if (status == 0) {
        auto& groupInfo = localStats.QueryResult.ProcessSegmentGroupInformation;
        
        std::wcout << L"Local Memory (VRAM):\n";
        std::wcout << L"  Budget: " << groupInfo.Budget / (1024*1024) << L" MB\n";
        std::wcout << L"  Usage: " << groupInfo.Usage / (1024*1024) << L" MB\n";
        if (groupInfo.Budget > 0) {
            double usagePercent = (double)groupInfo.Usage / groupInfo.Budget * 100.0;
            std::wcout << L"  Usage: " << usagePercent << L"%\n";
        }
    } else {
        std::wcout << L"Local memory group query failed (0x" << std::hex << status << L")\n";
    }

    // Non-local memory group
    D3DKMT_QUERYSTATISTICS nonLocalStats = {};
    nonLocalStats.Type = D3DKMT_QUERYSTATISTICS_PROCESS_SEGMENT_GROUP;
    nonLocalStats.AdapterLuid = gpu.luid;
    nonLocalStats.QueryProcessSegmentGroup = D3DKMT_MEMORY_SEGMENT_GROUP_NON_LOCAL;

    status = D3DKMTQueryStatistics(&nonLocalStats);
    if (status == 0) {
        auto& groupInfo = nonLocalStats.QueryResult.ProcessSegmentGroupInformation;
        
        std::wcout << L"\nNon-Local Memory (System RAM):\n";
        std::wcout << L"  Budget: " << groupInfo.Budget / (1024*1024) << L" MB\n";
        std::wcout << L"  Usage: " << groupInfo.Usage / (1024*1024) << L" MB\n";
        if (groupInfo.Budget > 0) {
            double usagePercent = (double)groupInfo.Usage / groupInfo.Budget * 100.0;
            std::wcout << L"  Usage: " << usagePercent << L"%\n";
        }
    } else {
        std::wcout << L"Non-local memory group query failed (0x" << std::hex << status << L")\n";
    }
}

void QueryGpuMemoryUsage(GpuInfo& gpu) {
    std::wcout << L"\n" << std::wstring(60, L'=') << L"\n";
    std::wcout << gpu.name << L" (" << (gpu.isDiscrete ? L"Discrete" : L"Integrated") << L")\n";
    std::wcout << L"Physical Adapter Index: " << gpu.physicalAdapterIndex << L"\n";
    
    // Test WDDM 3.1 support for this specific GPU
    gpu.supportsWDDM31 = TestWDDM31Support(gpu);
    std::wcout << L"WDDM 3.1 Support: " << (gpu.supportsWDDM31 ? L"Yes" : L"No") << L"\n";
    std::wcout << std::wstring(60, L'=') << L"\n";

    // Use appropriate method for this GPU
    if (gpu.supportsWDDM31) {
        std::wcout << L"Using WDDM 3.1 Enhanced Memory Monitoring\n";
        QuerySegmentUsageWDDM31(gpu);
        QuerySegmentGroupUsageWDDM31(gpu);
    } else {
        std::wcout << L"Using WDDM 2.1+ Memory Monitoring\n";
        QueryGpuMemoryUsageWDDM21(gpu);
    }
}

int main() {
    try {
        std::wcout << L"GPU Memory Monitor - Per-GPU WDDM Version Detection\n";
        std::wcout << L"===================================================\n";

        auto gpus = EnumerateGpus();
        
        if (gpus.empty()) {
            std::wcout << L"No GPUs found\n";
            return 1;
        }

        std::wcout << L"Found " << gpus.size() << L" GPU(s)\n";

        // Query each GPU individually with appropriate method
        for (auto& gpu : gpus) {
            QueryGpuMemoryUsage(gpu);
        }

        // Summary
        std::wcout << L"\n" << std::wstring(60, L'=') << L"\n";
        std::wcout << L"Summary:\n";
        for (const auto& gpu : gpus) {
            std::wcout << L"  " << gpu.name 
                      << L" - WDDM 3.1: " << (gpu.supportsWDDM31 ? L"✓" : L"✗") << L"\n";
        }

        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}