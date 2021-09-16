# DivinityMachine

![Build Status](https://github.com/PowerBall253/DivinityMachine/actions/workflows/build.yml/badge.svg)

Cross-platform texture converter from DDS to DOOM Eternal's TGA format.

To read more about the DOOM Eternal TGA format, click [here](https://wiki.eternalmods.com/books/reverse-engineering-file-formats/page/tga-file-extension-bimage).

NOTE: As of now, this tool only supports BC1 format DDS files.

## Usage

```
DivinityMachine [textures to convert]
```

Alternatively, drag and drop the texture files you want to convert into the executable.

## Compiling

The project uses CMake to compile. It also needs MSVC or the MinGW toolchain on MSYS to compile on Windows.

First clone the repo by running:

```
git clone https://github.com/PowerBall253/DivinityMachine.git
```

Then, generate the makefile by running:

```
cd DivinityMachine
cmake -B "build" # Append "-G 'MSYS Makefiles'" on MSYS
```

Afterwards you can build with:

```
cmake --build "build" --config Release
```

Finally, on Linux and macOS you can run

```
strip "build/DivinityMachine"
```

to reduce the binary's filesize.

The EternalResourceExtractor executable will be in the `build` folder in Linux/MinGW/macOS and in the `build\Release` folder in MSVC.

## Credits

* SamPT: For all the help with the DDS and TGA formats.
