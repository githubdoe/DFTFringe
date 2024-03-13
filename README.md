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
| Stage 1: getting compiler and QT | [aqt command line](https://github.com/githubdoe/DFTFringe?tab=readme-ov-file#option-a-using-command-line-and-aqt) | [QT IDE](https://github.com/githubdoe/DFTFringe?tab=readme-ov-file#option-b-using-qt-ide)  | [WSL](https://github.com/githubdoe/DFTFringe?tab=readme-ov-file#option-c-using-windows-subsystem-for-linux-wsl) |
| Stage 2: DLL dependencies        | [full build](https://github.com/githubdoe/DFTFringe?tab=readme-ov-file#option-a-full-build) | [lazy method](https://github.com/githubdoe/DFTFringe?tab=readme-ov-file#option-b-getting-dlls-from-official-installer) | :x: |
| Stage 3: Building DFTFringe      | [command line](https://github.com/githubdoe/DFTFringe?tab=readme-ov-file#option-a-using-command-line) | [QT IDE](https://github.com/githubdoe/DFTFringe?tab=readme-ov-file#option-b-using-ide)      | :x: |
| Stage 4: Copy necessary DLLs     | [command line](https://github.com/githubdoe/DFTFringe?tab=readme-ov-file#option-a-using-command-line-1) | [holistic approach](https://github.com/githubdoe/DFTFringe?tab=readme-ov-file#option-b-holistic-approach) | [from release](https://github.com/githubdoe/DFTFringe?tab=readme-ov-file#option-c-copying-from-release) |
| Stage 5: Making installer        | [QT installer framework](https://github.com/githubdoe/DFTFringe?tab=readme-ov-file#stage-5-making-installer) | :x:         | :x: |

## Stage 1: getting compiler and QT

### Option A: using command line and aqt

#### Create a folder

Personnaly I use `C:\buildingDFTFringe` to have a short path. All tools and dependencies will be stored here. It's important that inside folder structure is indentical for the project to build but you can change the base path.

#### Get minGw

The easiest way I have found to install it from command line requires to have Python installed. It then uses [aqtinstall](https://aqtinstall.readthedocs.io/en/latest/).

From within `C:\buildingDFTFringe` do the following:  
```
pip install aqtinstall
aqt install-tool windows desktop tools_mingw qt.tools.win64_mingw810
```

Add minGW to your Path: `C:\buildingDFTFringe\Tools\mingw810_64\bin`
Add Qmake to your Path: `C:\buildingDFTFringe\5.15.2\mingw81_64\bin`

#### Get QT

Here again, I have found easier to use Python and [aqtinstall](https://aqtinstall.readthedocs.io/en/latest/) to get QT.

From within `C:\buildingDFTFringe` do the following:
```
aqt install-qt windows desktop 5.15.2 win64_mingw81 -m qtcharts qtdatavis3d
aqt install-qt windows desktop 5.15.2 win64_mingw81 --archives qtbase qtsvg
```

#### Get Qt creator

This is not a necessary step but if you are building on your own computer you probably want to debug and need Qt creator IDE.
```
aqt install-tool windows desktop tools_qtcreator
```

### Option B: using QT IDE

Download the **open source** version of Qt framework https://www.qt.io/download-open-source
Run the installer and go through it.
When it's time to select which components to install in custom installation you must first tick the `archive` checkbox and click on `filter`. Without this, you won't be able to install the old Qt 5.15.2 version.  

**Which components to choose ?**  
You will need `QT 5.15.2`.  
For faster installation, you probably do not need `Qt design studio`.   
In the details of Qt 5.15.2 you only need `MinGW 8.1.0 64-bit`, `QT Charts` and `Qt Data Visualization`.  
You probably want to install `Qt Creator`. This is Qt's IDE and if you are here it's probably because you need to debug (breakpoints, step by step, variables, ...).  
You may want to install `Cmake`, `Ninja` and `Qt Installer Framework`.

### Option C: using Windows Subsystem for Linux (WSL)

Since Windows 10, Windows provides a native Linux subsystem. You don't need a virtual machine or a dual boot and can run any Linux application directly inside Windows. It works also for graphical applications.  
More information about installation are available here: https://learn.microsoft.com/en-us/windows/wsl/install  

All what you need to do is:
```
wsl --install
```

Once a Linux distribution is installed, you only need this to run same one each time:
```
wsl
```

For the rest of the build process you can now rely on [Linux build process](how-to-build-dftfringe-on-linux).

## Stage 2: DLL dependencies

### Option A: full build

What will be described here is the same method used to build the installer [GitHub action workflow](https://github.com/githubdoe/DFTFringe/blob/master/.github/workflows/build-windows.yml). It relies heavily on command line but you can transpose everything to your prefered approch. For example using CMake-gui instead of cmake command line. Versions might have changed, rely on the workflow if you want to duplicate with same versions as the repository.

If you haven't already, you will need to install latest version of [CMake](https://cmake.org/) first.

#### Build OpenCV

Get [OpenCV](https://opencv.org/) source code version 4.6.0 in your prefered way (clone the repo, download the archive, homing pigeon, ...) and have it in folder named `C:\buildingDFTFringe\openCV`

Then from within `C:\buildingDFTFringe` do the following:  

``` 
cmake -G "MinGW Makefiles" -S openCV -B build_openCV -D WITH_QT=ON -D WITH_OPENGL=ON -D Qt5_DIR=:./5.15.2/mingw81_64/lib/cmake/Qt5
cmake -G "MinGW Makefiles" -S openCV -B build_openCV
cmake --build ./build_openCV -j4
cmake --install ./build_openCV
```
You may need to adapt `Qt5_DIR` to you actual installation directory. If you installed Qt with the official isntaller the path is `C:/Qt/5.15.2/mingw81_64/lib/cmake/Qt5`.

#### Build Qwt

Get [Qwt](https://qwt.sourceforge.io/) source code version 6.1.6 in your prefered way (clone the repo, download the archive, homing pigeon, ...) and have it in folder named `C:\buildingDFTFringe\qwt-6.1.6`.

Then from within `C:\buildingDFTFringe\qwt-6.1.6` do the following:  
```
qmake.exe
mingw32-make -j4
```

#### Build Lapack and Blas

Get [Lapack](https://www.netlib.org/lapack/) source code version 3.11.0 in your prefered way (clone the repo, download the archive, homing pigeon, ...) and have it in folder named `C:\buildingDFTFringe\lapack`. Lapack comes with Blas.

Then from within `C:\buildingDFTFringe` do the following:  
```
cmake -G "MinGW Makefiles" -S lapack -B build_lapack -D BUILD_SHARED_LIBS=ON -D CMAKE_SHARED_LINKER_FLAGS="-Wl,--allow-multiple-definition"
cmake --build ./build_lapack -j4
```

#### Build Armadillo

Get [Armadillo](https://arma.sourceforge.net/) source code version 12.6.7 in your prefered way (clone the repo, download the archive, homing pigeon, ...) and have it in folder named `C:\buildingDFTFringe\armadillo-12.6.7`.

Then from within `C:\buildingDFTFringe` do the following:  
```
cmake -D CMAKE_PREFIX_PATH=C:/buildingDFTFringe/build_lapack -G "MinGW Makefiles" -S armadillo-12.6.7 -B build_armadillo
cmake --build ./build_armadillo -j4
```

It's important that Armadillo knows the path to Lapack to work correctly. Here we use CMAKE_PREFIX_PATH but you can also add it to your Path or use other methods.

### Option B: getting DLLs from official installer

#### Build OpenCV

Get [OpenCV](https://opencv.org/) **installer (not source code)** 4.6.0 in your prefered way (typically from their [GitHub](https://github.com/opencv/opencv/releases) or [website](https://opencv.org/releases/)). Run the installer `opencv-4.6.0-vc14_vc15.exe`.

Copy content from `yourExtractionLocation\opencv\build\include` to `C:\buildingDFTFringe\build_openCV\install\include`.  

#### Build Qwt

Get [Qwt](https://qwt.sourceforge.io/) source code version 6.1.6 in your prefered way (clone the repo, download the archive, homing pigeon, ...) and have it in folder named `C:\buildingDFTFringe\qwt-6.1.6`.

No additional modification required here.

#### Build Armadillo

Get [Armadillo](https://arma.sourceforge.net/) source code version 12.6.7 in your prefered way (clone the repo, download the archive, homing pigeon, ...) and have it in folder named `C:\buildingDFTFringe\armadillo-12.6.7`.

Then follow the information in `DFTFringe.pro` to create appropriate folders for header files and DLLs.  
Copy content from `armadillo-12.6.7\include` to `build_armadillo\tmp\include`. 

#### Copy all the DLLs

Previous steps have permitted to get necessary header files to build the code. DLLs are still requires.  
Install DFTFringe from official installer and copy the DLLs from install folder to the folder expected by `DFTFringe.pro`:  
```
qwt-6.1.6\lib\qwt.dll
build_lapack\bin\libblas.dll
build_lapack\bin\liblapack.dll
build_openCV\install\x64\mingw\bin\libopencv_calib3d460.dll
build_openCV\install\x64\mingw\bin\libopencv_core460.dll
build_openCV\install\x64\mingw\bin\libopencv_features2d460.dll
build_openCV\install\x64\mingw\bin\libopencv_highgui460.dll
build_openCV\install\x64\mingw\bin\libopencv_imgcodecs460.dll
build_openCV\install\x64\mingw\bin\libopencv_imgproc460.dll
```

## Stage 3: Building DFTFringe

### Option A: using command line

Get [DFTFringe](https://github.com/githubdoe/DFTFringe) source code in your prefered way (clone the repo, download the archive, homing pigeon, ...) and have it in folder named `C:\buildingDFTFringe\DFTFringe`.

From within `C:\buildingDFTFringe\DFTFringe` do the following:  
```
qmake.exe
mingw32-make -j4
```

### Option B: using IDE

Open Qt Creator  
Open the project file `DFTFringe.pro`  
Build  

## Stage 4: Copy necessary DLLs

### Option A: using command line

From within `C:\buildingDFTFringe` do the following:  
Note `Copy-Item` is for powershell.  
```
windeployqt.exe DFTFringe\Release\DFTFringe.exe

Copy-Item ".\build_lapack\bin\liblapack.dll" -Destination ".\DFTFringe\Release"
Copy-Item ".\build_lapack\bin\libblas.dll" -Destination ".\DFTFringe\Release"
Copy-Item ".\build_openCV\install\x64\mingw\bin\libopencv_calib3d460.dll" -Destination ".\DFTFringe\Release"
Copy-Item ".\build_openCV\install\x64\mingw\bin\libopencv_core460.dll" -Destination ".\DFTFringe\Release"
Copy-Item ".\build_openCV\install\x64\mingw\bin\libopencv_features2d460.dll" -Destination ".\DFTFringe\Release"
Copy-Item ".\build_openCV\install\x64\mingw\bin\libopencv_flann460.dll" -Destination ".\DFTFringe\Release"
Copy-Item ".\build_openCV\install\x64\mingw\bin\libopencv_highgui460.dll" -Destination ".\DFTFringe\Release"
Copy-Item ".\build_openCV\install\x64\mingw\bin\libopencv_imgcodecs460.dll" -Destination ".\DFTFringe\Release"
Copy-Item ".\build_openCV\install\x64\mingw\bin\libopencv_imgproc460.dll" -Destination ".\DFTFringe\Release"
Copy-Item ".\qwt-6.1.6\lib\qwt.dll" -Destination ".\DFTFringe\Release"
Copy-Item ".\5.15.2\mingw81_64\bin\Qt5OpenGL.dll" -Destination ".\DFTFringe\Release"
Copy-Item ".\tools\mingw810_64\bin\libquadmath-0.dll" -Destination ".\DFTFringe\Release"
Copy-Item ".\DFTFringe\ColorMaps" -Destination ".\DFTFringe\Release\ColorMaps" -Recurse
mkdir ".\DFTFringe\Release\res"
Copy-Item ".\DFTFringe\res\help" -Destination ".\DFTFringe\Release\res\help" -Recurse
Copy-Item ".\DFTFringe\RevisionHistory.html" -Destination ".\DFTFringe\Release"
```

### Option B: holistic approach

Run your newly built DFTFringe.exe.  
Identify missing DLLs by reading error message.  
Copy the missing DLLs from wherever they are to the build folder. Take care to use correct version of each DLL.  
Continue to do so until it works.

### Option C: copying from release

Use the official installer to install DFTFringe.  
From the install folder, copy these file to your build folder:  
- All `.dll` files
- `ColorMaps` folder
- `imageformats` folder
- `platform` folder
- `res` folder

## Stage 5: Making installer

:warning: This step is described only for documentation purpose. It's very unlikelly you need to make an installer manually on your computer. Only official releases should get installer.

First you need to get Qt Installer Framework. The easiest way I have found to install it from command line requires to have Python installed. It then uses [aqtinstall](https://aqtinstall.readthedocs.io/en/latest/). You can also install it with official Qt installer.

From within `C:\buildingDFTFringe` do the following: 
```
pip install aqtinstall
aqt install-tool windows desktop tools_ifw
```

:building_construction: Under construction :building_construction:
- Need to copy all DLLs (see github action build-windows.yml)
- Create the installer 
- To test on a developper computer, remember to remove path to DLLs (compiler, ...)

:building_construction: Under construction :building_construction:

# How to analyse crashlog

:building_construction: Under construction :building_construction: