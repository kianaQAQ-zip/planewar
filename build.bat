@echo off
REM PlaneWar M0 build helper (MinGW-w64 + Git Bash / CMD)
REM IMPORTANT: do NOT double-click this file. Run it from a terminal (Git Bash or
REM Developer Command Prompt) where cmake + g++.exe are on PATH, so you can read
REM any error. This script pauses on failure so the window stays open.

where cmake >nul 2>nul
if errorlevel 1 (
  echo [ERROR] cmake not found on PATH. Install CMake and add to PATH, or run this
  echo         from a terminal that already has it (e.g. Git Bash).
  goto :fail
)

where g++.exe >nul 2>nul
if errorlevel 1 (
  echo [ERROR] g++.exe (MinGW-w64) not found on PATH. Install MinGW-w64 and add its
  echo         bin\ directory to PATH.
  goto :fail
)

if "%SFML_DIR%"=="" (
  echo [ERROR] SFML_DIR is not set. Example:
  echo   set SFML_DIR=C:\SFML-2.6.2\lib\cmake\SFML
  goto :fail
)

cmake -G "MinGW Makefiles" -S . -B build -DSFML_DIR="%SFML_DIR%"
if errorlevel 1 goto :fail

cmake --build build --config Release
if errorlevel 1 goto :fail

echo.
echo [OK] Build succeeded. Copy SFML bin\*.dll next to build\PlaneWar.exe, then run it.
echo.
pause
exit /b 0

:fail
echo.
echo [BUILD FAILED] Read the messages above. Fix them and re-run.
echo   Tip: run from a terminal (Git Bash), not by double-clicking.
echo.
pause
exit /b 1
