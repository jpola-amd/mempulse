# Build example

To build example project as independend project
1. Configure, build and install somewhere `mempulse` library. See root [README.md](../README.md)
2. Build project
```
cmake -DMempulse_DIR=/tmp/root/lib/cmake/Mempulse/ -S . -B build
cmake --build build
```
3. mempulse library dependency

If library installed on system (or added to PATH on windows) there are no additional actions required 

If library not installed on system:
* For windows: 

copy `mempulse.dll` library to `./build/Debug` or `./build/Release` directory (depends from build type)

* For linux: 

copy `libmempulse.so` library to `./build/Debug` or `./build/Release` directory (depends from build type)


