#include "mempulse/mempulse.h"
#include <iostream>

#define check(err) if (err != MEMPULSE_SUCCESS) return err;

int main(int /*argc*/, char** /*argv*/) {
	MempulseError err;
	MempulseContext context;

	err = MempulseInitialize(&context);
	check(err);

	int deviceCount;
	err = MempulseGetAvailabeDeviceCount(context, &deviceCount);
	check(err);

	for (int i = 0; i < deviceCount; ++i) {
		char deviceName[255];

		err = MempulseGetDeviceName(context, i, deviceName, sizeof(deviceName));
		check(err);

		std::cout << "Device Id:\t\t" << i << "\n";
		std::cout << "Device Name:\t\t" << deviceName << "\n";

		MempulseDeviceMemoryInfo info;
		err = MempulseGetDeviceMemoryInfo(context, i, &info);
		check(err);

		err = MempulsePrintDeviceMemoryInfo(&info);
		check(err);
	}

	err = MempulseShutdown(context);
	check(err);

    return 0;
}
