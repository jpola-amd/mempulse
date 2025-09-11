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


# Build

List of dependencies are:
* HIP
* Boost (for command line application)
* GTest (for testing)

## Linux

### Dependencies

[hip installation](https://rocm.docs.amd.com/projects/install-on-linux/en/latest/install/quick-start.html)

* HIP for ubuntu 24.04

```
wget https://repo.radeon.com/amdgpu-install/6.4.3/ubuntu/noble/amdgpu-install_6.4.60403-1_all.deb
sudo apt install ./amdgpu-install_6.4.60403-1_all.deb
sudo apt update
sudo apt install python3-setuptools python3-wheel
sudo usermod -a -G render,video $LOGNAME # Add the current user to the render and video groups
sudo apt install rocm
```

* boost (for command line application)
```
apt-get install libboost-dev
```

* gtest (for testing)

```
apt-get install libgtest-dev
```

### Build mempulse

Create build directory
```
mkdir build
```

Build project with command

```
cmake -DCMAKE_BUILD_TYPE=debug -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_INSTALL_PREFIX=/tmp/mempulse ..
```

## Windows


### Dependencies

Installation via `vcpkg` command line

#### vcpkg

Clone somewhere vcpkg

```
git clone https://github.com/microsoft/vcpkg.git
```

Go to cloned directory 

```
cd vcpkg
```

Execute bootstrap
```
./bootstrap-vcpkg.sh # For Linux/macOS
./bootstrap-vcpkg.bat # For Windows
```
Install integration 
```
 ./vcpkg integrate install
```

Install dependencies
```
vcpkg install gtest
vckpg install boost
```

### Build

Go to project directory, create build dir
```
mkdir build
```
Go to project directory
```
cd build
```

Build project. Please, note: `CMAKE_TOOLCHAIN_FILE` should contain path to your vcpkg
```
cmake -DCMAKE_TOOLCHAIN_FILE=C:/workspace/vcpkg/scripts/buildsystems/vcpkg.cmake ..
```

Open visual studio, open solution file. Solution file should be in `build` directory

Build project
