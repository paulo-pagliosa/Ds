@echo off
rem Builds the cgvis library on Windows (Visual Studio).
rem Usage: build.bat [Release|Debug]   (default: Release)

setlocal

set "CONFIG=%~1"
if "%CONFIG%"=="" set "CONFIG=Release"

if /I not "%CONFIG%"=="Release" if /I not "%CONFIG%"=="Debug" (
  echo Usage: %~nx0 [Release^|Debug]
  exit /b 1
)

rem Run from the script's folder, wherever it is called from
cd /d "%~dp0"

rem Multi-config generator: one build folder, configuration chosen at build time
cmake -S . -B build || exit /b 1
cmake --build build --config %CONFIG% --parallel || exit /b 1
