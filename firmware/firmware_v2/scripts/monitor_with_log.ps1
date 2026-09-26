# Run PlatformIO serial monitor and tee output to serial_log.txt
# Usage: from firmware_v2 folder, run: .\scripts\monitor_with_log.ps1
# Or from repo root: .\firmware\firmware_v2\scripts\monitor_with_log.ps1

$ProjectDir = Split-Path -Parent $PSScriptRoot
if ($PWD.Path -ne $ProjectDir) {
    Push-Location $ProjectDir
    $Pop = $true
}

$logPath = Join-Path $ProjectDir "serial_log.txt"
Write-Host "Serial monitor output will also be written to: $logPath"
Write-Host "Press Ctrl+C to stop."
Write-Host ""

try {
    pio device monitor --baud 115200 2>&1 | Tee-Object -FilePath $logPath
}
finally {
    if ($Pop) { Pop-Location }
}
