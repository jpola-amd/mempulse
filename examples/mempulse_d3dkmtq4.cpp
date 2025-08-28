#include <Windows.h>
#include <dxgi1_6.h>
#include <iostream>
#include <vector>
#include <d3dkmthk.h>
#include <ntstatus.h>
#include <winternl.h>  // Add this for NT_SUCCESS macro
#include <iomanip>

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "dxgi.lib")


std::string wstring_to_string(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();

    // Get the length of the resulting UTF-8 string
    int size_needed = WideCharToMultiByte(
        CP_UTF8,            // convert to UTF-8
        0,                  // flags
        wstr.data(),        // input wide string
        (int)wstr.size(),   // length of input
        nullptr,            // no output buffer yet
        0,                  // calculate size only
        nullptr, nullptr);

    if (size_needed == 0) {
        // Error handling if needed
        return std::string();
    }

    // Allocate string of needed size
    std::string strTo(size_needed, 0);

    // Do the actual conversion
    int converted_chars = WideCharToMultiByte(
        CP_UTF8,
        0,
        wstr.data(),
        (int)wstr.size(),
        &strTo[0],
        size_needed,
        nullptr,
        nullptr);

    if (converted_chars == 0) {
        // Error handling if needed
        return std::string();
    }

    return strTo;
}

struct GpuInfo {
    LUID luid;
    std::string name;
    bool isIntegrated;
    UINT physicalAdapterIndex;

};

struct MemInfo {
    size_t local_free;
    size_t local_total;
    size_t shared_free;
    size_t shared_total;
   
    size_t total_free() const {
        return local_free + shared_free;
    }

    size_t total_used() const {
        return local_total - local_free + shared_total - shared_free;
    }

    size_t total() const {
        return local_total + shared_total;
    }

};

bool is_integrated_gpu_d3dkmt(LUID adapter_luid) {
    D3DKMT_OPENADAPTERFROMLUID openAdapter = {};
    openAdapter.AdapterLuid = adapter_luid;
    
    if (D3DKMTOpenAdapterFromLuid(&openAdapter) != STATUS_SUCCESS) {
        return false;
    }
    
    D3DKMT_QUERYADAPTERINFO queryInfo = {};
    queryInfo.hAdapter = openAdapter.hAdapter;
    queryInfo.Type = KMTQAITYPE_ADAPTERTYPE;
    
    D3DKMT_ADAPTERTYPE adapterType = {};
    queryInfo.pPrivateDriverData = &adapterType;
    queryInfo.PrivateDriverDataSize = sizeof(adapterType);
    
    bool integrated = false;
    if (D3DKMTQueryAdapterInfo(&queryInfo) == STATUS_SUCCESS) {
        integrated = (adapterType.HybridIntegrated != 0);
    }
    return integrated;
}

std::vector<GpuInfo> get_available_gpus() {
    int physicalAdapterIndex = 0;
    std::vector<GpuInfo> gpus;
    std::vector<std::pair<UINT, UINT>> seen_devices; // Track VendorId+DeviceId pairs

    IDXGIFactory1* factory = nullptr;
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

        std::wcout 
        << L"Found GPU: " << desc.Description
        << L" (LUID: 0x" << std::hex << std::uppercase << std::setfill(L'0')
        << std::setw(8) << static_cast<uint32_t>(desc.AdapterLuid.HighPart) << L":"
        << std::setw(8) << desc.AdapterLuid.LowPart << std::dec << L")"
        << std::endl;

        auto device_id = std::make_pair(desc.VendorId, desc.DeviceId);
        
        // Check if we've already seen this vendor/device combination
        bool already_seen = false;
        for (const auto& seen_device : seen_devices) {
            if (seen_device == device_id) {
                already_seen = true;
                break;
            }
        }

        if (!already_seen) {
            seen_devices.push_back(device_id);
            gpus.push_back({ desc.AdapterLuid, wstring_to_string(desc.Description), is_integrated_gpu_d3dkmt(desc.AdapterLuid), i});
        }

        adapter->Release();
    }

    factory->Release();
    return gpus;
}

void query_segment_group_usage(const GpuInfo& gpu, D3DKMT_MEMORY_SEGMENT_GROUP group) {
    std::cout << "\nSegment Group " << (group == D3DKMT_MEMORY_SEGMENT_GROUP_LOCAL ? "Local (VRAM)" : "Non-Local (System)")
              << " Usage on " << gpu.name 
              << " (LUID: 0x" << std::hex << std::uppercase << std::setfill('0')
        << std::setw(8) << static_cast<uint32_t>(gpu.luid.HighPart) << ":"
        << std::setw(8) << gpu.luid.LowPart << std::dec << "): \n";

    D3DKMT_QUERYSTATISTICS stats = {};
    stats.Type = D3DKMT_QUERYSTATISTICS_SEGMENT_GROUP_USAGE;
    stats.AdapterLuid = gpu.luid;
    //stats.QuerySegmentGroupUsage.PhysicalAdapterIndex = gpu.physicalAdapterIndex;
    stats.QuerySegmentGroupUsage.SegmentGroup = group;

    NTSTATUS status = D3DKMTQueryStatistics(&stats);
    if (NT_SUCCESS(status)) {
        auto& usage = stats.QueryResult.SegmentGroupUsageInformation;

        std::cout << "  Dedicated (Allocated + Modified): " << ((usage.AllocatedBytes + usage.ModifiedBytes) / (1024*1024)) << " MB\n";
        std::cout << "  Allocated: " << (usage.AllocatedBytes / (1024*1024)) << " MB\n";
        std::cout << "  Free: " << (usage.FreeBytes / (1024*1024)) << " MB\n";
        std::cout << "  Zero: " << (usage.ZeroBytes / (1024*1024)) << " MB\n";
        std::cout << "  Modified: " << (usage.ModifiedBytes / (1024*1024)) << " MB\n";
        std::cout << "  Standby: " << (usage.StandbyBytes / (1024*1024)) << " MB\n";
        
        UINT64 totalMB = (usage.AllocatedBytes + usage.FreeBytes + usage.ZeroBytes + 
                         usage.ModifiedBytes + usage.StandbyBytes) / (1024*1024);
        std::cout << "  Total: " << totalMB << " MB\n";
        
        // Calculate actual usage percentage
        if (totalMB > 0) {
            float usedPercentage = (float)(usage.AllocatedBytes + usage.ModifiedBytes) / 
                                 (float)(usage.AllocatedBytes + usage.FreeBytes + usage.ZeroBytes + 
                                        usage.ModifiedBytes + usage.StandbyBytes) * 100.0f;
            std::cout << "  Usage: " << std::fixed << std::setprecision(1) << usedPercentage << "%\n";
        }
    } else {
        std::cout << "  Failed to query segment group usage: 0x" << std::hex << status << std::dec << "\n";
    }
}

void sandbox(GpuInfo gpu) {
    std::cout << "\nMemory Usage Analysis for " << gpu.name << ":\n";
    std::cout << std::string(50, '=') << "\n";
    
    // Query Local Memory (VRAM) usage
    query_segment_group_usage(gpu, D3DKMT_MEMORY_SEGMENT_GROUP_LOCAL);
    
    // Query Non-Local Memory (System RAM) usage
    query_segment_group_usage(gpu, D3DKMT_MEMORY_SEGMENT_GROUP_NON_LOCAL);
}

int main() {
    auto gpus = get_available_gpus();
    for (const auto& gpu : gpus) {
        std::cout << "GPU: " << gpu.name << " (LUID: " << gpu.luid.LowPart << ", " << gpu.luid.HighPart << ")\n";
        sandbox(gpu);
        std::cout << "\n";
    }
    return 0;
}
