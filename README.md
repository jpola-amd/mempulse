# mempulse
MemPulse is a cross-platform diagnostic tool that monitors and reports GPU video memory usage and budget. Designed for developers working with HIP-based compute workloads, MemPulse integrates platform-specific D3DKMT on Windows and hipMemGetInfo on Linux to deliver accurate and actionable insights into GPU memory consumption and availability.

## AMD Radeon RX 9070 XT Memory Usage Comparison
### Windows Task Manager
* Dedicated GPU Memory: **7.8** / 16.0 GB <--- 
* Shared GPU Memory: 1.4 / 39.8 GB
* Total GPU Memory: 9.2 / 55.8 GB

### MemPulse (D3DKMTMemory Reporter)

* Total Memory Local (VRAM): 16,195 MB (15.82 GB) <--- See Task manager stats above
* Total Memory Shared: 0 MB (0 GB)
* Used Memory Local: 8,049 MB (**7.86 GB**) <--- See Task manager stats above
* Used Memory Shared: 0 MB (0 GB)
* Free Memory Local: 8,146 MB (7.96 GB)
* Free Memory Shared: 0 MB (0 GB)

#### Note:
Mempulse automatically detects whether the gpu is discrete or integrated gpu.
For Integrated gpus the shared memory segments are taken into account.
The “Local” memory in MemPulse corresponds to “Dedicated GPU Memory” in Task Manager.
The “Shared” memory in MemPulse corresponds to “Shared GPU Memory” in Task Manager.
However it may depend on the driver vendor how the shared is reported. For AMD integrated
gpus the available shared memory is reported as dedicated memory.
Small differences may occur due to sampling time, rounding, and how each tool interprets memory states.
MemPulse provides a detailed breakdown of VRAM usage and availability, closely matching the values shown in Task Manager for dedicated GPU memory.

# Building

Building notes in `BUILD.me` file

# API

Api declaration ma be found in `mempulse/mempulse.h`

## Example

For full example with cmake, see  examples/README.md

```cpp
#include "mempulse/mempulse.h"

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

        MempulseDeviceMemoryInfo info;
        err = MempulseGetDeviceMemoryInfo(context, i, &info);
        check(err);

        MempulsePrintDeviceMemoryInfo(info);
    }

    err = MempulseShutdown(context);
    check(err);

    return 0;
}

```
