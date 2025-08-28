#include "mempulse/mempulse.h"


#include <hip/hip_runtime.h>

int main(int argc, char** argv) {
    // Initialize the mempulse library
    if (!mempulse::Initialize()) {
        std::cerr << "Failed to initialize mempulse library." << std::endl;
        return -1;
    }

    // Get the number of HIP devices
    int deviceCount = mempulse::MemoryReporterFactory::GetDeviceCount();
    if (deviceCount <= 0) {
        std::cerr << "No HIP devices found." << std::endl;
        return -1;
    }

    // Create a memory reporter for the first HIP device

    for (int i = 0; i < deviceCount; ++i) {
        
        const int hipDeviceId = i;
        
        auto reporter = mempulse::MemoryReporterFactory::CreateMemoryReporter(hipDeviceId);
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
        
        // Shutdown the reporter
        reporter->ShutDown();
    }

    // Shutdown the mempulse library
    mempulse::Shutdown();

    return 0;
}