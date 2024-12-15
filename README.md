# Compiling
This project depends on [cmake](https://cmake.org/) and [vcpkg](https://github.com/microsoft/vcpkg) and [ninja](https://ninja-build.org/).

The compiler used is the Visual C++ compiler provided by microsoft. In order to use it, you need to download Visual Studio, then open the `x86 Native Tools Command Prompt for VS 2022` and use that to run the build steps. If you instead opt to create solution files and build directly in Visual Studio, you can ignore this.

## Building
In order to build this project, use:
```
cmake -B build -S . -G Ninja
```

Then, go into the newly created build folder and run:
```
ninja
```

If you want to instead use Visual Studio directly to build, use:
```
cmake -B build -S .
```

When opening the solution file, make sure that you're compiling for x86/Win32 and that the RuntimeLibrary set is `MT_Static<Release|Debug> (/MT or /MTd)`

