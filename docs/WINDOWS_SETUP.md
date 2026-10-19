# Windows build and release notes

The full software requirements, MinGW installation steps, PATH setup and VS Code build/play instructions are in the [README](../README.md). Use those as the main setup guide.

## If the build does not start

- Check `g++ --version`, `mingw32-make --version` and `cmake --version` in a new VS Code terminal. PATH tells Windows where to find these commands; restart VS Code after changing it.
- Build the whole project with Ctrl+Shift+B or `.\build.bat`. Single-file Run Code cannot link the other source files and Raylib.
- The first build needs internet access to download Raylib.
- If CMake reports a compiler/generator mismatch after you change compilers, delete only the generated `build` folder and build again.
- Keep `assets` beside the EXE so the bundled fonts load. The VS Code F5 setup also requires GDB.

## Development downloads

[Releases](https://github.com/Rad-Sharkboy/Parcel-Panic-Conveyor-Mayhem/releases) contains playable Windows ZIPs. Download `ParcelPanic-Windows-x64.zip` from Assets, extract it completely and run the EXE inside. GitHub's automatic **Source code** ZIP contains code, not the built game. No compiler is needed to play the packaged build.

The release package contains the EXE, fonts/licenses and [play instructions](PLAY.txt). The current pre-release is [v0.1.0-dev.1](https://github.com/Rad-Sharkboy/Parcel-Panic-Conveyor-Mayhem/releases/tag/v0.1.0-dev.1).

For the next development release, push a new tag such as `v0.1.0-dev.2`. The [Windows workflow](../.github/workflows/ci.yml) builds the game and publishes the ZIP as a pre-release. Use a new tag for each version and wait for the workflow to succeed before sharing it. A successful build still needs a local Windows play check.
