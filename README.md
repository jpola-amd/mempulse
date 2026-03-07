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

# API 

Api declaration may be found in [mempulse/mempulse.h](mempulse/mempulse.h)

Example of usage may be found in [examples/example.cpp](examples/example.cpp)

Example of cmake for integration with project [examples/CMakeLists.txt](examples/CMakeLists.txt)

Full project may be found in `example` directory.

### Environment variables

* `MEMPULSE_TRIM_VISIBLE_MEMORY` - optional soft cap for reported local/dedicated memory.
    Useful for experiments that intentionally avoid allocating close to the full visible budget.
    Accepts raw bytes or units like `56GiB`, `60GB`, `1024MiB`.


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

        err = MempulsePrintDeviceMemoryInfo(&info);
        check(err);
    }

    err = MempulseShutdown(context);
    check(err);

    return 0;
}

```

# Command line utility
Mempulse provides command line utulity `gpumempulse`. 

Example of usage `gpumempulse`:
```
gpumempulse

Device Id:              0
Device Name:            AMD Radeon RX 6800 XT
Total Memory Local:     16279 MB (15.8978 GB)
Total Memory Shared:    31412 MB (30.6761 GB)
Used Memory Local:      1951 MB (1.90613 GB)
Used Memory Shared:     299 MB (0.292656 GB)
Free Memory: Local:     14327 MB (13.9917 GB)
Free Memory: Shared:    31112 MB (30.3835 GB)
```

# Building
## Dependencies on Linux
* HIP 

See [Quick start installation guide](https://rocm.docs.amd.com/projects/install-on-linux/en/latest/install/quick-start.html)

* drm library
```
apt get install libdrm-dev libdrm-amdgpu-dev
```
## Dependencies on Windows
* HIP

See [AMD HIP SDK for Windows](https://www.amd.com/en/developer/resources/rocm-hub/hip-sdk.html)

This version tested with HIP 6.2.4

* GDI (Graphics Device Interface) 

Shoudl be included with Windows Kit SDK

* D3DKMT

Should be included with Windows Driver Kit (WDK).

## Configure project
```
cmake -S . -B build
```

It is possible to specify additional parameters via -D 

| Option                       | Description                                          | Default    |
| ---------------------------- | ---------------------------------------------------- | ---------- |
| `MEMPULSE_EXPORT_API`        | Export dll symbols                                   | `ON`       |
| `BUILD_BACKEND_DRM`          | Build DRM backend (Linux only)                       | `${UNIX}`  |
| `BUILD_BACKEND_HIP`          | Build HIP backend                                    | `ON`       |
| `BUILD_BACKEND_D3DKMT`       | Build D3D KMT backend (Windows only)                 | `${WIN32}` |
| `BUILD_TESTS`                | Build tests                                          | `ON`       |
| `BUILD_APPLICATION`          | Build command line application                       | `ON`       |
| `ENABLE_ASAN`                | Enable address sanitizer via `libasan`               | `OFF`      |
| `FORCE_COLORED_OUTPUT`       | Force GCC/LLVM diagnostic color output               | `OFF`      |
| `ENABLE_INTERNAL_HIP_MODULE` | Internal HIP dependency handling                     | `OFF`      |
| `HIP_DIR`                    | Path to ROCm `hip-config.cmake` file                 | `""`       |
| `HIP_SDK`                    | Path to ROCm root directory                          | `""`       |


Example of building mempulse library on windows, with ROCM version 7.1. 
Installation directory is c:/tmp/mempulse

```
cmake -DHIP_DIR=c:/Program\ Files/AMD/ROCm/7.1/lib/cmake/hip -DCMAKE_INSTALL_PREFIX=c:/tmp/mempulse -S . -B build

```

### Internal HIP Dependency Handling

`mempulse` can manage the HIP dependency internally using the
`cmake/AddHip.cmake` module.

This internal mechanism does **not** perform any HIP installation discovery
or validation. Instead, it assumes that the HIP SDK (headers, libraries, and
runtime) is already available at the path specified by `HIP_DIR`.

The main purpose of this mechanism is to support project integrators who do
not want to depend on the standard ROCm HIP CMake package and prefer to link
against a predefined HIP SDK location.

To enable the internal HIP handling:

1. Enable the `ENABLE_INTERNAL_HIP_MODULE` CMake option.
2. Set the `HIP_SDK` variable to the root directory of the HIP SDK.

Example for windows:

```
cmake -S . -DENABLE_INTERNAL_HIP_MODULE=1 -B build -DHIP_SDK=c:/Program\ Files/AMD/ROCm/7.1/
```

## Build 
```
cmake --build build --config Release
```

## Installation

To install the library, use the `install` target provided by CMake:
```
cmake --install build --strip --prefix /tmp/root
```

Installation without stripping
```
cmake --install build --prefix /tmp/root
```

## Packaging

To create a release package (e.g. .zip or .tar.gz), use the package target:

```
cmake --build build --target package
```


