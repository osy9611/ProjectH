# Runs build_battle.py with one of its camera candidates and prints its result lines.
param([string]$Camera = 'explore')
$root = Resolve-Path "$PSScriptRoot\..\.."
$env:HD2D_BATTLE_CAMERA = $Camera
Start-Process 'C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor-Cmd.exe' -Wait -WindowStyle Hidden -ArgumentList @(
    "$root\Client\ProjectH.uproject", '-run=pythonscript', "-script=$PSScriptRoot\build_battle.py", '-unattended', '-nullrhi', '-nosplash', '-log=build_battle.log')
Get-Content "$root\Client\Saved\HD2D_Explore\build_battle.txt" -Encoding utf8
Select-String -Path "$root\Client\Saved\Logs\build_battle.log" -Pattern 'LogPython: Error' | Select-Object -Last 4 | ForEach-Object { $_.Line }
