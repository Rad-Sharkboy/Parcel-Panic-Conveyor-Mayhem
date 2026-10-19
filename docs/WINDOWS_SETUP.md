# Windows setup for editing the code

To only play, download the ZIP from [Releases](https://github.com/Rad-Sharkboy/Parcel-Panic-Conveyor-Mayhem/releases). These tools are for building or editing.

1. Install **VS Code** and its **C/C++** and **CMake Tools** extensions.
2. Install **CMake 3.22+** from [cmake.org](https://cmake.org/download/). Choose the option to add it to PATH.
3. Install **MinGW-w64**. If your Rig Builder compiler already works, you can use it here.

If you need MinGW, install [MSYS2](https://www.msys2.org/), open its **UCRT64** terminal and run:

```text
pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-make mingw-w64-ucrt-x86_64-gdb
```

Add `C:\msys64\ucrt64\bin` to your Windows user PATH (adjust it if MSYS2 is elsewhere). PATH tells Windows where to find commands. Restart VS Code afterward.

Open the project folder in VS Code. In its terminal, `g++ --version`, `mingw32-make --version` and `cmake --version` should work. Then run:

```powershell
.\build.bat
.\build\ParcelPanic.exe
```

The first build downloads Raylib. Keep `build/assets` beside the EXE. Press Ctrl+Shift+B to rebuild after edits. F5 starts the supplied GDB debugging setup; Visual Studio is not required.

If you change compilers and CMake complains, delete only the generated `build` folder and build again.
