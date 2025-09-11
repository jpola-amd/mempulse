#include "mempulse/mempulse.h" #include <hip/hip_runtime.h> int main(int argc, char** argv) { if (!mempulse::Initialize()) { std::cerr << "Failed to initialize mempulse library." << std::endl; return -1; } int deviceCount = mempulse::MemoryReporterFactory::GetDeviceCount();
    if (deviceCount <= 0) {
        std::cerr << "No HIP devices found." << std::endl;
        return -1;
    }

    for (int i = 0; i < deviceCount; ++i) {

        const int hipDeviceId = i;
        std::cout << "\nQuerying memory for HIP Device ID: " << hipDeviceId << std::endl;
        hipDeviceProp_t deviceProperties;
        hipGetDeviceProperties(&deviceProperties, hipDeviceId);
        std::cout << "Device Name: " << deviceProperties.name << std::endl;

        auto reporter = mempulse::MemoryReporterFactory::CreateMemoryReporter(hipDeviceId, false);
        if (!reporter) {
            std::cerr << "Failed to create memory reporter." << std::endl;
            return -1;
        }

        mempulse::MemoryInfo memInfo;
        if (reporter->GetMemoryInfo(memInfo)) {
            std::cout << "Total Memory Local: " << memInfo.dedicatedTotal / (1024*1024) << " MB ("
            << (float) memInfo.dedicatedTotal / (1024*1024*1024) << " GB)" << std::endl;
            std::cout << "Total Memory Shared: " << memInfo.sharedTotal  / (1024*1024) << " MB ("
            << (float) memInfo.sharedTotal  / (1024*1024*1024) << " GB)" << std::endl;

            std::cout << "Used Memory Local: " << memInfo.dedicatedUsed / (1024*1024) << " MB ("
            << (float) memInfo.dedicatedUsed / (1024*1024*1024) << " GB)" << std::endl;
            std::cout << "Used Memory Shared: " << memInfo.sharedUsed / (1024*1024) << " MB ("
            << (float) memInfo.sharedUsed / (1024*1024*1024) << " GB)" << std::endl;

            std::cout << "Free Memory: Local: " << (memInfo.dedicatedTotal - memInfo.dedicatedUsed) / (1024*1024) << " MB ("
            << (float) (memInfo.dedicatedTotal - memInfo.dedicatedUsed) / (1024*1024*1024) << " GB)" << std::endl;
            std::cout << "Free Memory: Shared: " << (memInfo.sharedTotal - memInfo.sharedUsed) / (1024*1024) << " MB ("
            << (float) (memInfo.sharedTotal - memInfo.sharedUsed) / (1024*1024*1024) << " GB)" << std::endl;
        } else {
            std::cerr << "Failed to get memory information." << std::endl;
        }

        reporter->ShutDown();
    }

    mempulse::Shutdown();

    return 0;
}
