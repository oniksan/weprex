# Building Weprex
## Software requirements
To build Weprex using automated scripts, you must:
The following software was installed:
- docker

OS: Linux

## Assembly
To build, you need to run the script file [weprex_build.sh](weprex_build.sh) from `linux_build` folder, after which
Weprex assembly steps will begin from source codes followed by packaging
into a binary executable package of type AppImages.
All main assembly and packaging takes place in a Docker container.

## OS requirements to run Weprex for linux.
Minimum recommended version for running on Ubuntu(or like Debian) 20.04  and higher, or another OS on
Linux base with glibc>=2.31.