@echo off
rem Builds cgdemo on Windows (Visual Studio).
rem Usage: build.bat [Release|Debug]   (default: Release)
rem The cg library must be built first, in the same configuration.

setlocal

set "CONFIG=%~1"
if "%CONFIG%"=="" set "CONFIG=Release"

if /I "%CONFIG%"=="Release" (
  set "CG_LIB=cg.lib"
) else if /I "%CONFIG%"=="Debug" (
  set "CG_LIB=cgD.lib"
) else (
  echo Usage: %~nx0 [Release^|Debug]
  exit /b 1
)

rem Run from the script's folder, wherever it is called from
cd /d "%~dp0"

rem Fail early if the cg library has not been built yet
if not exist "..\..\cg\lib\%CG_LIB%" (
  echo Error: ..\..\cg\lib\%CG_LIB% not found. Build cg ^(%CONFIG%^) first.
  exit /b 1
)

rem Multi-config generator: one build folder, configuration chosen at build time
cmake -S . -B build || exit /b 1
cmake --build build --config %CONFIG% --parallel || exit /b 1
