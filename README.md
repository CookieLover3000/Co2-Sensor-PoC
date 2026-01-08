# CO2 Sensor PoC

This is the repository for the PoC of my Co2 Sensor display system.

## Compiling the project


Install arm-none-eabi-gcc and arm-none-eabi-g++.

Ubuntu/Debian:

```
sudo apt update
sudo apt install cmake binutils-arm-none-eabi gcc-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib
```

Fedora:

```
sudo dnf update
sudo dnf install cmake arm-none-eabi-newlib arm-none-eabi-binutils-cs arm-none-eabi-gcc-cs arm-none-eabi-gcc-cs-c++
```

Arch:

```
sudo pacman -S cmake arm-none-eabi-gcc arm-none-eabi-binutils arm-none-eabi-newlib
```

Then run the following commands:

```
cmake --preset Debug
cmake --build --preset Debug
```