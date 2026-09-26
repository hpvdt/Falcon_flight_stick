@echo off
REM Run PlatformIO serial monitor and append output to serial_log.txt
REM Run from firmware_v2 folder: scripts\monitor_with_log.bat

cd /d "%~dp0.."
echo Serial monitor output will also be written to serial_log.txt
echo Press Ctrl+C to stop.
echo.

REM Windows has no built-in tee; we use PowerShell for that
powershell -NoProfile -Command "pio device monitor --baud 115200 2>&1 | Tee-Object -FilePath serial_log.txt"
pause
