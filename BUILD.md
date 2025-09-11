# Build mempulse library

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
