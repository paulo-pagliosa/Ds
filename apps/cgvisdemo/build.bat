@echo off
rem Builds cgvisdemo on Windows (Visual Studio).
rem Usage: build.bat [Release|Debug]   (default: Release)
rem The cg and cgvis libraries must be built first, in the same configuration.

setlocal

set "CONFIG=%~1"
if "%CONFIG%"=="" set "CONFIG=Release"

if /I "%CONFIG%"=="Release" (
  set "SUFFIX="
) else if /I "%CONFIG%"=="Debug" (
  set "SUFFIX=D"
) else (
  echo Usage: %~nx0 [Release^|Debug]
  exit /b 1
)

rem Run from the script's folder, wherever it is called from
cd /d "%~dp0"

rem Fail early if a library has not been built yet
for %%L in ("..\..\cg\lib\cg%SUFFIX%.lib" "..\..\cgvis\lib\cgvis%SUFFIX%.lib") do (
  if not exist %%L (
    echo Error: %%~L not found. Build it ^(%CONFIG%^) first.
    exit /b 1
  )
)

rem Multi-config generator: one build folder, configuration chosen at build time
cmake -S . -B build || exit /b 1
cmake --build build --config %CONFIG% --parallel || exit /b 1
