# Runs build_explore.py and prints its result lines.
$root = Resolve-Path "$PSScriptRoot\..\.."
Start-Process 'C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor-Cmd.exe' -Wait -WindowStyle Hidden -ArgumentList @(
    "$root\Client\ProjectH.uproject", '-run=pythonscript', "-script=$PSScriptRoot\build_explore.py", '-unattended', '-nullrhi', '-nosplash', '-log=build_explore.log')
Get-Content "$root\Client\Saved\HD2D_Explore\build.txt" -Encoding utf8
Select-String -Path "$root\Client\Saved\Logs\build_explore.log" -Pattern 'LogPython: Error' | Select-Object -Last 4 | ForEach-Object { $_.Line }
