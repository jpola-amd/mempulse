#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <pdh.h>

#include <pdhmsg.h>

// Implement an object that will monitor the utilization of 
// GPU's shared and dedicated memory
// run it in thread and periodically report the usage statistics
// Display the usage statistics in MB


struct MemPchReporter {

     // Performance counter members
    PDH_HQUERY m_perfQuery = nullptr;
    PDH_HCOUNTER m_dedicatedMemoryCounter = nullptr;
    PDH_HCOUNTER m_sharedMemoryCounter = nullptr;
    bool m_perfCountersInitialized = false;

    std::thread m_monitoringThread;
    std::atomic<bool> m_stopMonitoring{false};

    MemPchReporter();
    ~MemPchReporter();
    void InitializePerfCounters();
    void StartMonitoring();
    void StopMonitoring();
    void UpdateUsage();
    void ShutdownPerfCounters();
    void ReportUsage();
    void EnumerateGpuCounters();
private:
    void MonitoringThreadFunction();
};


MemPchReporter::MemPchReporter() {
    EnumerateGpuCounters(); 
    InitializePerfCounters();
}

MemPchReporter::~MemPchReporter() {
    StopMonitoring();
    ShutdownPerfCounters();
}
void MemPchReporter::EnumerateGpuCounters() {
    // List of GPU performance objects that might contain memory counters
    std::vector<std::string> gpuObjects = {
        "GPU Adapter Memory",
        "GPU Process Memory", 
        "GPU Local Adapter Memory",
        "GPU Non Local Adapter Memory",
        "GPU Memory",
        "GPU Engine"  // Keep this for reference
    };
    
    for (const auto& objectName : gpuObjects) {
        std::cout << "\n=== Checking object: " << objectName << " ===\n";
        
        DWORD bufferSize = 0;
        PDH_STATUS status = PdhEnumObjectItemsA(
            nullptr, nullptr, objectName.c_str(),
            nullptr, &bufferSize,
            nullptr, &bufferSize,
            PERF_DETAIL_WIZARD, 0
        );
        
        if (status == PDH_MORE_DATA) {
            std::vector<char> counterBuffer(bufferSize);
            std::vector<char> instanceBuffer(bufferSize);
            
            DWORD counterBufferSize = bufferSize;
            DWORD instanceBufferSize = bufferSize;
            
            status = PdhEnumObjectItemsA(
                nullptr, nullptr, objectName.c_str(),
                counterBuffer.data(), &counterBufferSize,
                instanceBuffer.data(), &instanceBufferSize,
                PERF_DETAIL_WIZARD, 0
            );
            
            if (status == ERROR_SUCCESS) {
                std::cout << "Available " << objectName << " counters:\n";
                const char* counter = counterBuffer.data();
                while (*counter != '\0') {
                    std::cout << "  " << counter << std::endl;
                    counter += strlen(counter) + 1;
                }
                
                std::cout << "\nAvailable " << objectName << " instances:\n";
                const char* instance = instanceBuffer.data();
                while (*instance != '\0') {
                    std::cout << "  " << instance << std::endl;
                    instance += strlen(instance) + 1;
                }
            }
        } else if (status == PDH_CSTATUS_NO_OBJECT) {
            std::cout << "Object '" << objectName << "' does not exist on this system.\n";
        } else {
            std::cout << "Error enumerating '" << objectName << "': " << std::hex << status << std::dec << std::endl;
        }
    }
}

void MemPchReporter::InitializePerfCounters() {
    if (PdhOpenQuery(nullptr, 0, &m_perfQuery) == ERROR_SUCCESS) {
        // Add performance counters for dedicated and shared memory usage
        LONG dedicatedStatus = PdhAddCounter(m_perfQuery, "\\GPU Engine(*)\\Dedicated Usage", 0, &m_dedicatedMemoryCounter);
        LONG sharedStatus = PdhAddCounter(m_perfQuery, "\\GPU Engine(*)\\Shared Usage", 0, &m_sharedMemoryCounter);

        //verify the status
        if (dedicatedStatus == ERROR_SUCCESS && sharedStatus == ERROR_SUCCESS) {
            m_perfCountersInitialized = true;
            std::cout << "Performance counters initialized successfully\n";
        } else {
            std::cout << "Failed to add performance counters\n";
        }

        if (dedicatedStatus == ERROR_SUCCESS && sharedStatus == ERROR_SUCCESS) {
            m_perfCountersInitialized = true;
            std::cout << "Performance counters initialized successfully\n";
        } else {
            std::cout << "Failed to add performance counters\n";
        }
    } else {
        std::cout << "Failed to open PDH query\n";
    }
}

void MemPchReporter::StartMonitoring() {
    if (m_perfCountersInitialized && !m_monitoringThread.joinable()) {
        m_stopMonitoring = false;
        m_monitoringThread = std::thread(&MemPchReporter::MonitoringThreadFunction, this);
        std::cout << "Monitoring started\n";
    }
}

void MemPchReporter::StopMonitoring() {
   if (m_monitoringThread.joinable()) {
        m_stopMonitoring = true;
        m_monitoringThread.join();
        std::cout << "Monitoring stopped\n";
    }
}

void MemPchReporter::UpdateUsage() {
    if (m_perfCountersInitialized) {
        if (PdhCollectQueryData(m_perfQuery) == ERROR_SUCCESS) {
            ReportUsage();
        } else {
            std::cout << "Failed to collect query data\n";
        }
    }
}

void MemPchReporter::MonitoringThreadFunction() {
    while (!m_stopMonitoring) {
        UpdateUsage();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
void MemPchReporter::ShutdownPerfCounters() {
    if (m_perfCountersInitialized) {
        PdhCloseQuery(m_perfQuery);
        m_perfCountersInitialized = false;
    }
}

void MemPchReporter::ReportUsage() {
     if (m_perfCountersInitialized) {
        PDH_FMT_COUNTERVALUE dedicatedValue;
        PDH_FMT_COUNTERVALUE sharedValue;
        
        PDH_STATUS dedicatedStatus = PdhGetFormattedCounterValue(m_dedicatedMemoryCounter, PDH_FMT_LARGE, nullptr, &dedicatedValue);
        PDH_STATUS sharedStatus = PdhGetFormattedCounterValue(m_sharedMemoryCounter, PDH_FMT_LARGE, nullptr, &sharedValue);
        
        if (dedicatedStatus == ERROR_SUCCESS && sharedStatus == ERROR_SUCCESS) {
            std::cout << "Dedicated Memory Usage: " << dedicatedValue.largeValue / (1024 * 1024) << " MB | ";
            std::cout << "Shared Memory Usage: " << sharedValue.largeValue / (1024 * 1024) << " MB\n";
        } else {
            std::cout << "Failed to get counter values\n";
        }
    }
}



int main(int argc, char* argv[])
{
    MemPchReporter reporter;
    
    reporter.StartMonitoring();
    
    std::this_thread::sleep_for(std::chrono::seconds(3));
    
    reporter.StopMonitoring();
}