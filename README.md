# <img src="img/pwf2tools-x64.png" align="center"> libpwf2tools - A suite of libraries for modding PTR2! </img>

libpwf2tools is a suite of modding tools for the game "PaRappa the Rapper 2" recreated as normal C++ libraries. It is released under the GPLv3 license. It's free to use and open source (and always will be!). The icon is drawn by [@p4ckles](https://twitter.com/p4ckles). It currently only consists of libisomod. libpwf2int, libpwf2spm, and libpwf2tex will come soon.

## Currently finished -
  * libisomod

# Installing and using
Currently, you must install from source. To build libpwf2tools currently, you need the following dependencies on your system: **CMake 3.5** (or newer) and a **C/C++ compiler**. At the root of the directory, you will need to run these commands:

    mkdir build
    cd build
    cmake ..
    make
	# Optional
	make install

## Supported files
  * Archives
    - `ISO9660 archives`

## isomod

isomod deals with the manipulation of ISO9660 files. It is not exclusive to PaRappa the Rapper 2. It can pull or inject files into an ISO9660 file without the need of extracting and recreating the entire file.