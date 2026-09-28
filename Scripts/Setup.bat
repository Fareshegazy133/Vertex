@echo off
setlocal

rem ============================================================================
rem Vertex setup
rem   1. Fetches third-party sources at the version pinned in their *.Module.lua
rem   2. Generates Vertex.sln
rem
rem Safe to run any time: sources already at the pinned commit are verified
rem locally, not downloaded again.
rem
rem Usage:  Scripts\Setup.bat             normal setup
rem         Scripts\Setup.bat --refetch   also move existing third-party clones
rem                                       to their pinned commit (after a version bump)
rem ============================================================================

pushd "%~dp0.."
set "PREMAKE=Build\Premake\Bin\Windows\premake5.exe"

echo [1/2] Fetching third-party sources...
"%PREMAKE%" --file=Build/Premake/Main.lua fetch %*
if errorlevel 1 goto :failed

echo.
echo [2/2] Generating Visual Studio solution...
"%PREMAKE%" --file=Build/Premake/Main.lua vs2022
if errorlevel 1 goto :failed

echo.
echo Setup complete. Open Vertex.sln.
popd
exit /b 0

:failed
echo.
echo Setup FAILED. Read the first error above.
popd
exit /b 1
