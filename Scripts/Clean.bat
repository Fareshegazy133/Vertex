@echo off
setlocal

rem ============================================================================
rem Vertex clean
rem   Deletes everything the build generates: Binaries\, Intermediate\, Vertex.sln.
rem   Keeps fetched third-party sources (ThirdParty\*\Source) and IDE state (.vs\, .idea\).
rem   Run Scripts\Setup.bat afterwards to regenerate.
rem ============================================================================

pushd "%~dp0.."

if exist "Binaries"     rmdir /s /q "Binaries"
if exist "Intermediate" rmdir /s /q "Intermediate"
if exist "Vertex.sln"   del /f /q "Vertex.sln"

echo Clean complete. Run Scripts\Setup.bat to regenerate.
popd
exit /b 0
