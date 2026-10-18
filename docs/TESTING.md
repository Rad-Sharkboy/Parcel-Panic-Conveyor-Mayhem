# Tests and verification

## Run on Windows

Open the project in VS Code and press **Ctrl+Shift+B**, or run:

```powershell
.\build.bat
```

The batch file builds the game and test executable, then runs CTest. A failed configure, build or test command stops the script with a nonzero exit code. To rerun the tests without rebuilding:

```powershell
ctest --test-dir build --output-on-failure
.\build\parcel_tests.exe
```

For a Debug build, use `.\build.bat Debug`. The F5 configuration uses this build and GDB. Both configurations execute explicit checks rather than assertions that disappear in Release.

## Test coverage

The C++ runner contains **16 groups and 6,938 checks**. CTest registers that runner as one test (`parcel_core_tests`), so 1/1 in CTest means the full executable completed. No checks are disabled or skipped.

Coverage includes queue empty/full/overflow/wraparound, 1,000 heap entries and stable ties, hash collisions/updates/tombstones/rehash, stack LIFO and undo, parcel generation and identity, occupancy, braking, correct/wrong docks, dispatch order, deadline expiry exactly once, fragile hazards, jams, rushes, clean restart and level skipping. See [test_cases.txt](../tests/test_cases.txt).

A test-only route solver changes only legal junction choices. It checks queue/cell/heap membership, status/location consistency and terminal counts every tick, without teleporting parcels or altering deadlines.

| Level | Generated | Delivered | Failed | Score | Final tick | Outcome |
|---|---:|---:|---:|---:|---:|---|
| Sort School | 20 | 20 | 0 | 7,400 | 287 | Won |
| Rush Junction | 38 | 38 | 0 | 16,400 | 305 | Won |
| Fragile Freight | 48 | 48 | 0 | 20,590 | 371 | Won |
| Mayhem Mode | 60 | 51 | 9 | 17,020 | 454 | Won |

The nine final-level losses are verified deadline expirations. They are allowed gameplay outcomes; the run exceeds the level's quota and target. Separate uncontrolled runs verify failure/termination, and persistent artificial obstruction verifies jam failure.

## What was actually verified

The original implementation was compiled and exercised in the cloud. Debug, Release and AddressSanitizer/UndefinedBehaviorSanitizer core runs passed with leak detection, and the graphical game rendered into a virtual display. An additional cloud input check exercised 26 keyboard/mouse outcomes, including pause, switching/undo, braking, demo mutations and level wrapping. Cloud-only helpers are kept outside the published project.

A MinGW-w64 cross-build also successfully produced a real **64-bit Windows `ParcelPanic.exe`** and `parcel_tests.exe`. Inspection of the game EXE found only Windows system DLL imports (`GDI32`, `KERNEL32`, `msvcrt`, `SHELL32`, `USER32`, `WINMM`), with no MinGW runtime DLL requirement. This verifies Windows-target compilation and linking, not native Windows execution. The cloud used its own generator/toolchain for that check; it could not execute Windows CMD or the produced EXEs.

The repository is now arranged for **Windows 11, VS Code and MinGW**. Its Windows workflow uses the same `build.bat` as the user and uploads `build/ParcelPanic.exe`. Native Windows execution, the batch file under Windows CMD, GDB integration, and a clean-machine EXE check have not been verified in this cloud session. Hosted CI is configured, not claimed to have run.

The verification compiler was GCC 14.2.0 with CMake 4.4.4 and Raylib 5.5. Project sources compiled with `-Wall -Wextra -Wpedantic`; upstream Raylib CMake compatibility and unused audio-loader compiler warnings were observed during dependency builds. A target of 60 FPS is configured, not a measured guarantee.

## Before publishing a playable release

1. Install the README's Windows tools and run `.\build.bat` from VS Code, including a project path containing spaces.
2. Open `build\ParcelPanic.exe` and play each level. Check mouse input, type/destination labels, planning undo and brake timing.
3. Exercise F1–F3, F4/Shift+F4 including from results, F5 restart, menu return, retry and next-level actions.
4. Try F5 debugging with GDB and check Windows display scaling/readability.
5. Copy the Release EXE to a clean Windows 11 computer and verify it starts without MinGW DLLs or asset files beside it.
6. Check the actual Windows CI result before describing the hosted build as passing. Attach the tested EXE to a GitHub Release if distributing a playable download.

Human difficulty testing remains useful: a route solver demonstrates solvability, not how every player will react under pressure.
