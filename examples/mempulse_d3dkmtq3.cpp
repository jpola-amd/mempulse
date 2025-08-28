#include <Windows.h>
#include <dxgi1_6.h>
#include <iostream>
#include <vector>
#include <d3dkmthk.h>
#include <ntstatus.h>
#pragma comment(lib, "gdi32.lib")

struct GpuInfo {
    LUID luid;
    std::wstring name;
    bool isDiscrete;
    UINT physicalAdapterIndex;
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
        gpus.push_back({ desc.AdapterLuid, desc.Description, discrete, i });

        adapter->Release();
    }

    factory->Release();
    return gpus;
}

void QuerySegmentGroupSizeInfo(const GpuInfo& gpu, D3DKMT_HANDLE hAdapter) {
    std::wcout << L"\n=== Segment Group Size Information ===\n";

    D3DKMT_QUERYADAPTERINFO queryInfo = {};
    queryInfo.hAdapter = hAdapter;
    queryInfo.Type = KMTQAITYPE_GETSEGMENTGROUPSIZE;

    D3DKMT_SEGMENTGROUPSIZEINFO segmentGroupInfo = {};
    segmentGroupInfo.PhysicalAdapterIndex = gpu.physicalAdapterIndex;

    queryInfo.pPrivateDriverData = &segmentGroupInfo;
    queryInfo.PrivateDriverDataSize = sizeof(segmentGroupInfo);

    NTSTATUS status = D3DKMTQueryAdapterInfo(&queryInfo);
    if (status == STATUS_SUCCESS) {
        std::wcout << L"Physical Adapter Index: " << segmentGroupInfo.PhysicalAdapterIndex << L"\n\n";

        // Legacy segment information
        std::wcout << L"Legacy Segment Information:\n";
        std::wcout << L"  Dedicated Video Memory: " 
                  << segmentGroupInfo.LegacyInfo.DedicatedVideoMemorySize / (1024*1024) << L" MB\n";
        std::wcout << L"  Dedicated System Memory: " 
                  << segmentGroupInfo.LegacyInfo.DedicatedSystemMemorySize / (1024*1024) << L" MB\n";
        std::wcout << L"  Shared System Memory: " 
                  << segmentGroupInfo.LegacyInfo.SharedSystemMemorySize / (1024*1024) << L" MB\n";

        // Modern segment group information
        std::wcout << L"\nModern Segment Group Information:\n";
        std::wcout << L"  Local Memory (VRAM): " 
                  << segmentGroupInfo.LocalMemory / (1024*1024) << L" MB\n";
        std::wcout << L"  Non-Local Memory (System RAM): " 
                  << segmentGroupInfo.NonLocalMemory / (1024*1024) << L" MB\n";
        std::wcout << L"  Non-Budget Memory: " 
                  << segmentGroupInfo.NonBudgetMemory / (1024*1024) << L" MB\n";

        // Calculate totals
        ULONGLONG totalMemoryMB = (segmentGroupInfo.LocalMemory + 
                                  segmentGroupInfo.NonLocalMemory) / (1024*1024);
        std::wcout << L"\nTotal Available Memory: " << totalMemoryMB << L" MB\n";

        // Memory type breakdown
        if (segmentGroupInfo.LocalMemory > 0) {
            std::wcout << L"GPU Type: Discrete (has dedicated VRAM)\n";
        } else {
            std::wcout << L"GPU Type: Integrated (uses system memory)\n";
        }
    } else {
        std::wcerr << L"Failed to query segment group size info: 0x" << std::hex << status << L"\n";
    }
}

void QueryLegacySegmentSize(D3DKMT_HANDLE hAdapter) {
    std::wcout << L"\n=== Legacy Segment Size Information ===\n";

    D3DKMT_QUERYADAPTERINFO queryInfo = {};
    queryInfo.hAdapter = hAdapter;
    queryInfo.Type = KMTQAITYPE_GETSEGMENTSIZE;

    D3DKMT_SEGMENTSIZEINFO segmentInfo = {};
    queryInfo.pPrivateDriverData = &segmentInfo;
    queryInfo.PrivateDriverDataSize = sizeof(segmentInfo);

    NTSTATUS status = D3DKMTQueryAdapterInfo(&queryInfo);
    if (status == 0) {
        std::wcout << L"Dedicated Video Memory: " 
                  << segmentInfo.DedicatedVideoMemorySize / (1024*1024) << L" MB\n";
        std::wcout << L"Dedicated System Memory: " 
                  << segmentInfo.DedicatedSystemMemorySize / (1024*1024) << L" MB\n";
        std::wcout << L"Shared System Memory: " 
                  << segmentInfo.SharedSystemMemorySize / (1024*1024) << L" MB\n";
    } else {
        std::wcerr << L"Failed to query legacy segment size info: 0x" << std::hex << status << L"\n";
    }
}

void QueryGpuMemoryInfo(const GpuInfo& gpu) {
    std::wcout << L"\n" << std::wstring(60, L'=') << L"\n";
    std::wcout << gpu.name << L" (" << (gpu.isDiscrete ? L"Discrete" : L"Integrated") << L")\n";
    std::wcout << L"Physical Adapter Index: " << gpu.physicalAdapterIndex << L"\n";
    std::wcout << std::wstring(60, L'=') << L"\n";

    // Open adapter
    D3DKMT_OPENADAPTERFROMLUID openAdapter = {};
    openAdapter.AdapterLuid = gpu.luid;

    NTSTATUS status = D3DKMTOpenAdapterFromLuid(&openAdapter);
    if (status != 0) {
        std::wcerr << L"Failed to open adapter: 0x" << std::hex << status << L"\n";
        return;
    }

    // Query both legacy and modern segment information
    QueryLegacySegmentSize(openAdapter.hAdapter);
    QuerySegmentGroupSizeInfo(gpu, openAdapter.hAdapter);

    // Close adapter
    D3DKMT_CLOSEADAPTER closeAdapter = {};
    closeAdapter.hAdapter = openAdapter.hAdapter;
    D3DKMTCloseAdapter(&closeAdapter);
}

int main() {
    try {
        std::wcout << L"GPU Memory Information using D3DKMT_SEGMENTGROUPSIZEINFO\n";
        std::wcout << L"=======================================================\n";

        auto gpus = EnumerateGpus();
        
        if (gpus.empty()) {
            std::wcout << L"No GPUs found\n";
            return 1;
        }

        std::wcout << L"Found " << gpus.size() << L" GPU(s)\n";

        for (const auto& gpu : gpus) {
            QueryGpuMemoryInfo(gpu);
        }

        // Keep window open for viewing results
        std::wcout << L"\nPress any key to exit...\n";
        std::cin.get();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}