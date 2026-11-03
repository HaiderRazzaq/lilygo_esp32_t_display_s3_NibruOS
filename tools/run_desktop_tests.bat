@echo off
setlocal
where cmake >nul 2>&1
if errorlevel 1 (
  if exist "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" (
    set "PATH=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin;%PATH%"
  ) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" (
    set "PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin;%PATH%"
  )
)
where cmake >nul 2>&1
if errorlevel 1 (
  python "%~dp0run_desktop_tests.py" %*
  exit /b
)
echo ===================================================
echo   NibruOS Desktop Automated Test Runner
echo ===================================================

set VCVARS="C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat"
if not exist %VCVARS% (
  set VCVARS="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"
)
if exist %VCVARS% (
  call %VCVARS% x64
)

cmake -B build -G "Ninja" -DCMAKE_MAKE_PROGRAM="C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe"
if %ERRORLEVEL% neq 0 (
  cmake -B build
)
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%

cmake --build build --target nibru_tests
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%

.\build\nibru_tests.exe %*
