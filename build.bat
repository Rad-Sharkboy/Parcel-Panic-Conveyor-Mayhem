@echo off
setlocal
pushd "%~dp0"
if errorlevel 1 exit /b 1

where cmake >nul 2>&1
if errorlevel 1 goto missing_cmake
where gcc >nul 2>&1
if errorlevel 1 goto missing_compiler
where g++ >nul 2>&1
if errorlevel 1 goto missing_compiler
where mingw32-make >nul 2>&1
if errorlevel 1 goto missing_compiler

set "parcel_build_type=Release"
if /I "%~1"=="Debug" set "parcel_build_type=Debug"
echo Building Parcel Panic for Windows ^(%parcel_build_type%^) ...
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=%parcel_build_type% -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DPARCEL_BUILD_GAME=ON
if errorlevel 1 goto failed
cmake --build build --parallel 4
if errorlevel 1 goto failed

echo.
echo Build complete. Run build\ParcelPanic.exe to play.
popd
exit /b 0

:missing_cmake
echo ERROR: CMake was not found. Install CMake and add it to PATH, then restart VS Code.
goto failed

:missing_compiler
echo ERROR: MinGW-w64 gcc, g++, and mingw32-make must be on PATH.
echo See the Windows setup steps in README.md, then restart VS Code.
goto failed

:failed
echo.
echo Build stopped. Read the error above; the game has not been rebuilt successfully.
echo If you changed compiler or generator, delete only the generated build folder and try again.
popd
exit /b 1
