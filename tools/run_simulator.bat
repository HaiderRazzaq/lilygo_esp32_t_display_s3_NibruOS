@echo off
setlocal
echo ===================================================
echo   NibruOS Desktop Simulator Runner
echo ===================================================

if not exist build (
  mkdir build
)

set VCVARS="C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat"
if exist %VCVARS% (
  call %VCVARS% x64
) else (
  echo Warning: Default BuildTools vcvarsall.bat not found at %VCVARS%
  echo Trying system CMake / compiler...
)

echo Configuring CMake build...
cmake -B build -G "Ninja" -DCMAKE_MAKE_PROGRAM="C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe"
if %ERRORLEVEL% neq 0 (
  echo CMake configuration failed.
  exit /b %ERRORLEVEL%
)

echo Building NibruOS Desktop Simulator...
cmake --build build --target nibru_simulator
if %ERRORLEVEL% neq 0 (
  echo Build failed.
  exit /b %ERRORLEVEL%
)

echo Launching NibruOS Simulator...
.\build\nibru_simulator.exe %*
