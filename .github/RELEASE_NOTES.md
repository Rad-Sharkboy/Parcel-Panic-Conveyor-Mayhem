# Parcel Panic: Conveyor Mayhem — V2.1

## Download and play

1. Download **ParcelPanic-Windows-x64.zip** below.
2. Extract the whole ZIP, then open **ParcelPanic/ParcelPanic.exe**.
3. Keep the **assets** folder beside the EXE. No compiler or editor is needed to play.

The ZIP contains the game, its required assets and a short controls guide. GitHub's separate **Source code** downloads contain the project files rather than the playable game.

## V2.1 changelog

A minor update focused on development accessibility, code quality, reliability and maintainability.

### Added

- Autonomous Windows setup system (`setup.bat`) for building the project on fresh Windows installations.
- Automatic detection, installation and configuration of missing development dependencies, including MSYS2, MinGW-w64, GCC, CMake and Raylib.
- Download integrity verification, dependency reuse, automatic error recovery and adaptive parallel compilation.
- Detailed diagnostic logging and post-build executable and asset verification.
- Optional setup modes for debugging, clean builds, toolchain updates and automatic game launching.

### Improved

- Minor source code refinements and internal implementation improvements.
- Improved code readability, maintainability and overall project consistency.
- Simplified development environment configuration and first-time setup experience.

### Build from source

Run **setup.bat** from the project root to automatically prepare the development environment and compile the game.

An internet connection is required for the initial dependency installation. No manual compiler or Raylib installation is required on supported Windows systems.

The existing **build.bat** remains available for developers with an already configured toolchain.

### Project updates

- Retained the automated Windows build, packaging and release workflow.
- Preserved the existing gameplay experience and core data structure implementations.

## Contributor

- **[@Sabtainn](https://github.com/Sabtainn)** — Module 2 (Parcel Tracking and Dispatch) and shared work on Module 3 (Game Integration and Presentation).
