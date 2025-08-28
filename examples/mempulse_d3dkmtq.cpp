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
        gpus.push_back({ desc.AdapterLuid, desc.Description, discrete });

        adapter->Release();
    }

    factory->Release();
    return gpus;
}
void QueryGpuMemoryUsagePerSegment(const GpuInfo& gpu) {
    std::wcout << gpu.name << L" Memory Usage by Segment:\n";

    // First get adapter info to know how many segments exist
    D3DKMT_QUERYSTATISTICS adapterStats = {};
    adapterStats.Type = D3DKMT_QUERYSTATISTICS_ADAPTER;
    adapterStats.AdapterLuid = gpu.luid;

    NTSTATUS status = D3DKMTQueryStatistics(&adapterStats);
    if (status != 0) {
        std::wcerr << L"Failed to query adapter info: 0x" << std::hex << status << L"\n";
        return;
    }

    UINT numSegments = adapterStats.QueryResult.AdapterInformation.NbSegments;
    std::wcout << L"  Number of segments: " << numSegments << L"\n";

    // Query each segment
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
            
            std::wcout << L"    Segment " << segmentId << L":\n";
            std::wcout << L"      Commit Limit: " << commitLimitMB << L" MB\n";
            std::wcout << L"      Committed: " << bytesCommittedMB << L" MB\n";
            std::wcout << L"      Resident: " << bytesResidentMB << L" MB\n";
            std::wcout << L"      Aperture: " << segInfo.Aperture << L"\n";
        }
    }
}

void QueryProcessMemoryUsage(const GpuInfo& gpu) {
    std::wcout << L"\nProcess Memory Usage on " << gpu.name << L":\n";

    // Query process adapter information
    D3DKMT_QUERYSTATISTICS processStats = {};
    processStats.Type = D3DKMT_QUERYSTATISTICS_PROCESS_ADAPTER;
    processStats.AdapterLuid = gpu.luid;

    NTSTATUS status = D3DKMTQueryStatistics(&processStats);
    if (status != 0) {
        std::wcerr << L"Failed to query process adapter info: 0x" << std::hex << status << L"\n";
        return;
    }

    auto& procInfo = processStats.QueryResult.ProcessAdapterInformation;
    
    std::wcout << L"  Number of segments: " << procInfo.NbSegments << L"\n";
    std::wcout << L"  Virtual memory usage: " << procInfo.VirtualMemoryUsage << L" bytes\n";
    std::wcout << L"  Commitment data:\n";
    std::wcout << L"    Total bytes evicted: " << procInfo.CommitmentData.TotalBytesEvictedFromProcess / (1024*1024) << L" MB\n";

    // Query process segments
    for (UINT segmentId = 0; segmentId < procInfo.NbSegments; segmentId++) {
        D3DKMT_QUERYSTATISTICS procSegStats = {};
        procSegStats.Type = D3DKMT_QUERYSTATISTICS_PROCESS_SEGMENT;
        procSegStats.AdapterLuid = gpu.luid;
        procSegStats.QueryProcessSegment.SegmentId = segmentId;

        status = D3DKMTQueryStatistics(&procSegStats);
        if (status == 0) {
            auto& procSegInfo = procSegStats.QueryResult.ProcessSegmentInformation;
            
            ULONGLONG bytesCommittedMB = procSegInfo.BytesCommitted / (1024 * 1024);
            ULONGLONG maxWorkingSetMB = procSegInfo.MaximumWorkingSet / (1024 * 1024);
            ULONGLONG minWorkingSetMB = procSegInfo.MinimumWorkingSet / (1024 * 1024);
            
            std::wcout << L"  Process Segment " << segmentId << L":\n";
            std::wcout << L"    Committed: " << bytesCommittedMB << L" MB\n";
            std::wcout << L"    Max Working Set: " << maxWorkingSetMB << L" MB\n";
            std::wcout << L"    Min Working Set: " << minWorkingSetMB << L" MB\n";
        }
    }
}

#if (DXGKDDI_INTERFACE_VERSION >= DXGKDDI_INTERFACE_VERSION_WDDM2_1)
void QuerySegmentGroupUsage(const GpuInfo& gpu) {
    std::wcout << L"\nSegment Group Usage on " << gpu.name << L":\n";

    // Query local memory group (dedicated VRAM)
    D3DKMT_QUERYSTATISTICS localStats = {};
    localStats.Type = D3DKMT_QUERYSTATISTICS_PROCESS_SEGMENT_GROUP;
    localStats.AdapterLuid = gpu.luid;
    localStats.QueryProcessSegmentGroup = D3DKMT_MEMORY_SEGMENT_GROUP_LOCAL;

    NTSTATUS status = D3DKMTQueryStatistics(&localStats);
    if (status == 0) {
        auto& groupInfo = localStats.QueryResult.ProcessSegmentGroupInformation;
        
        std::wcout << L"  Local Memory (VRAM):\n";
        std::wcout << L"    Budget: " << groupInfo.Budget / (1024*1024) << L" MB\n";
        std::wcout << L"    Requested: " << groupInfo.Requested / (1024*1024) << L" MB\n";
        std::wcout << L"    Usage: " << groupInfo.Usage / (1024*1024) << L" MB\n";
    }

    // Query non-local memory group (system RAM)
    D3DKMT_QUERYSTATISTICS nonLocalStats = {};
    nonLocalStats.Type = D3DKMT_QUERYSTATISTICS_PROCESS_SEGMENT_GROUP;
    nonLocalStats.AdapterLuid = gpu.luid;
    nonLocalStats.QueryProcessSegmentGroup = D3DKMT_MEMORY_SEGMENT_GROUP_NON_LOCAL;

    status = D3DKMTQueryStatistics(&nonLocalStats);
    if (status == 0) {
        auto& groupInfo = nonLocalStats.QueryResult.ProcessSegmentGroupInformation;
        
        std::wcout << L"  Non-Local Memory (System RAM):\n";
        std::wcout << L"    Budget: " << groupInfo.Budget / (1024*1024) << L" MB\n";
        std::wcout << L"    Requested: " << groupInfo.Requested / (1024*1024) << L" MB\n";
        std::wcout << L"    Usage: " << groupInfo.Usage / (1024*1024) << L" MB\n";
    }
}
#endif

void QueryGpuMemoryUsage(const GpuInfo& gpu) {
    std::wcout << L"\n" << std::wstring(50, L'=') << L"\n";
    std::wcout << gpu.name << L" (" << (gpu.isDiscrete ? L"Discrete" : L"Integrated") << L")\n";
    std::wcout << std::wstring(50, L'=') << L"\n";

    // Query different types of memory information
    QueryGpuMemoryUsagePerSegment(gpu);
    QueryProcessMemoryUsage(gpu);
    
#if (DXGKDDI_INTERFACE_VERSION >= DXGKDDI_INTERFACE_VERSION_WDDM2_1)
    QuerySegmentGroupUsage(gpu);
#endif
}

int main() {
    try {
        auto gpus = EnumerateGpus();
        
        if (gpus.empty()) {
            std::wcout << L"No GPUs found\n";
            return 1;
        }

        for (const auto& gpu : gpus) {
            QueryGpuMemoryUsage(gpu);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}