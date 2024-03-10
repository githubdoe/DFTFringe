# DFTFringe

[![build-windows](https://github.com/githubdoe/DFTFringe/actions/workflows/build-windows.yml/badge.svg?branch=master)](https://github.com/githubdoe/DFTFringe/actions/workflows/build-windows.yml) [![build-linux](https://github.com/githubdoe/DFTFringe/actions/workflows/build-linux.yml/badge.svg?branch=master)](https://github.com/githubdoe/DFTFringe/actions/workflows/build-linux.yml)


# Introduction

DFTFringe Telescope Mirror interferometry analysis Program.
This is the successor to OpenFringe which is a Telescope Mirror interferometry analysis program.  

This code is distributed with DLLs from: 
- QT
- OpenCV
- QWT
- Armadillo
- Lapack
- BLAS

It also integrates code from:
- [Michael Peck Zernike analysis tools](https://github.com/mlpeck/zernike)
- [Bezier](https://github.com/oysteinmyrmo/bezier)
- [Boost](https://www.boost.org/)
- [Spdlog](https://github.com/gabime/spdlog)

You can find tutorials at https://youtu.be/wWdG3wrGAbM

Additional information and help is availlable at https://groups.io/g/Interferometry

# How to install DFTFringe on windows

:point_right: Follow the link and use the installer:
[link to latest release](https://github.com/githubdoe/dftfringe/releases/latest).

# How to build DFTFringe on Linux

```
sudo apt update
sudo apt install -y apt-utils build-essential wget qt5-qmake qt5-qmake-bin qt5-assistant qtbase5-dev qtmultimedia5-dev libqt5charts5 libqt5charts5-dev libqt5multimedia* libqt5datavisualization5-dev libqt5datavisualization5 libopencv-core-dev libopencv-core4.5d libopencv-dev libqwt-qt5-6 libqwt-qt5-dev libarmadillo-dev libarmadillo10
qmake
make -j4
```

# How to build DFTFringe on MacOS

:building_construction: Under construction :building_construction:

# How to build DFTFringe on Windows

:warning: For Windows installation it is recommended that you use the prebuilt installer provided here: [link to latest release](https://github.com/githubdoe/dftfringe/releases/latest).

There are many different solutions combinations for building. Pick the one you prefer/need. You can mix different options at each stage.

|                                  | Option A         | Option B    | Option C |
| -------------------------------- | ---------------- | ----------- | -------- |
| Stage 1: getting compiler and QT | aqt command line | QT IDE      | WSL |
| Stage 2: DLL dependencies        | full build       | lazy method | :x: |
| Stage 3: Building DFTFringe      | command line     | QT IDE      | :x: |
| Stage 4: Making installer        | distributing     | :x:         | :x: |

## Stage 1: getting compiler and QT
### Option A: using command line and aqt
### Option B: using QT IDE
### Option C: using Windows Subsystem for Linux (WSL)
## Stage 2: DLL dependencies
### Option A: full build
### Option B: getting DLLs from official installer
## Stage 3: Building DFTFringe
### Option A: using command line
### Option B: using IDE
## Stage 4: Making installer




### Create a folder

Personnaly I use `C:\buildingDFTFringe` to have a short path. All tools and dependencies will be stored here. It's important that inside folder structure is indentical for the project to build but you can change the base path.

### Get minGw

The easiest way I have found to install it from command line requires to have Python installed. It then uses [aqtinstall](https://aqtinstall.readthedocs.io/en/latest/).

From within `C:\buildingDFTFringe` do the following:  
```
pip install aqtinstall
aqt install-tool windows desktop tools_mingw qt.tools.win64_mingw810
aqt install-tool windows desktop tools_ifw
```

Add minGW to your Path: `C:\buildingDFTFringe\Tools\mingw810_64\bin`

### Get QT

Here again, I have found easier to use Python and [aqtinstall](https://aqtinstall.readthedocs.io/en/latest/) to get QT.

From within `C:\buildingDFTFringe` do the following: 
``` 
aqt install-qt windows desktop 5.15.2 win64_mingw81 -m qtcharts qtdatavis3d
aqt install-qt windows desktop 5.15.2 win64_mingw81 --archives qtbase qtsvg
```

### Build OpenCV

Get [OpenCV](https://opencv.org/) source code version 4.6.0 in your prefered way (clone the repo, download the archive, homing pigeon, ...) and have it in folder named `C:\buildingDFTFringe\openCV`

Then from within `C:\buildingDFTFringe` do the following:  

``` 
cmake -G "MinGW Makefiles" -S openCV -B build_openCV -D WITH_QT=ON -D WITH_OPENGL=ON -D Qt5_DIR=:./5.15.2/mingw81_64/lib/cmake/Qt5
cmake -G "MinGW Makefiles" -S openCV -B build_openCV
cmake --build ./build_openCV -j4
cmake --install ./build_openCV
```

### Build Qwt

Get [Qwt](https://qwt.sourceforge.io/) source code version 6.1.6 in your prefered way (clone the repo, download the archive, homing pigeon, ...) and have it in folder named `C:\buildingDFTFringe\qwt-6.1.6`

Then from within `C:\buildingDFTFringe\qwt-6.1.6` do the following:  
```
..\5.15.2\mingw81_64\bin\qmake.exe
mingw32-make -j4
```

### Build Lapack and Blas

Get [Lapack](https://www.netlib.org/lapack/) source code version 3.11.0 in your prefered way (clone the repo, download the archive, homing pigeon, ...) and have it in folder named `C:\buildingDFTFringe\lapack`. Lapack comes with Blas.

Then from within `C:\buildingDFTFringe` do the following:  
```
cmake -G "MinGW Makefiles" -S lapack -B build_lapack -D BUILD_SHARED_LIBS=ON -D CMAKE_SHARED_LINKER_FLAGS="-Wl,--allow-multiple-definition"
cmake --build ./build_lapack -j4
```

### Build Armadillo

Get [Armadillo](https://arma.sourceforge.net/) source code version 12.6.7 in your prefered way (clone the repo, download the archive, homing pigeon, ...) and have it in folder named `C:\buildingDFTFringe\armadillo-12.6.7`.

Then from within `C:\buildingDFTFringe` do the following:  
```
cmake -D CMAKE_PREFIX_PATH=C:/buildingDFTFringe/build_lapack -G "MinGW Makefiles" -S armadillo-12.6.7 -B build_armadillo
cmake --build ./build_armadillo -j4
```

It's important that Armadillo known the path to Lapack to work correctly. Here we use CMAKE_PREFIX_PATH but you can also add it to your Path or use other methods.

### Build DFTFringe

:writing_hand: This will be done in this PR




## TODO remove this

What will be described here is the same method used to build the installer [GitHub action workflow](https://github.com/githubdoe/DFTFringe/blob/master/.github/workflows/build-windows.yml). It relies heavily on command line but you can transpose everything to your prefered approch. For example using CMake-gui instead of cmake command line. Versions might have changed, rely on the workflow if you want to duplicate with same versions as the repository.