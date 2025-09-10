#include "mempulse/mempulse.h"

using namespace mempulse;

void process_hip_device(MemoryReporter* reporter)
{
	if (!reporter)
		throw std::runtime_error("reporter is null");

	std::cout << "Device Id:\t\t" << reporter->GetDeviceId() << std::endl;
    std::cout << "Device Name:\t\t" << reporter->GetDeviceName() << std::endl;

    mempulse::MemoryInfo memInfo = reporter->GetMemoryInfo();
    std::cout << memInfo;
}

void app_main() {
	Initialize();

	int deviceCount = MemoryReporterFactory::GetDeviceCount();
    if (deviceCount <= 0)
        throw std::runtime_error("no HIP devices found");

    for (int i = 0; i < deviceCount; ++i) {
		const int hipDeviceId = i;
		try {
        	auto reporter = mempulse::MemoryReporterFactory::CreateMemoryReporter(hipDeviceId, false);
			process_hip_device(reporter.get());
		} catch(const std::exception& e) {
			std::cerr << "can't get memory info for HIP Device ID: " << hipDeviceId << std::endl;
			std::cerr << "error:\t" << e.what() << std::endl;
		}
	}
}


int main(int argc, char** argv) {
	try {
		app_main();
	} catch (std::exception& e) {
		std::cerr << "error " << e.what() << std::endl;
		return -1;
	}

    return 0;
}
