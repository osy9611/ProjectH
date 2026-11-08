# Captures the drawn characters in MAP_Village_Day and MAP_Village_Night in one offscreen -game run (no window).
# Run build_artspec.py first. Screenshots land in $Out as Day_<shot>.png and Night_<shot>.png.
param([Parameter(Mandatory)][string]$Out, [int]$Timeout = 300)
$root = Resolve-Path "$PSScriptRoot\..\..\.."
New-Item -ItemType Directory -Force $Out | Out-Null
$env:ARTSPEC_OUT = (Resolve-Path $Out).Path
$exec = "py $($PSScriptRoot -replace '\\', '/')/artspec_driver.py"
$p = Start-Process 'C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe' -PassThru -ArgumentList @(
    "$root\Client\ProjectH.uproject", '/Game/Maps/Village/MAP_Village_Day', '-game', '-RenderOffscreen', '-nosplash', '-nosound', '-unattended',
    '-log=artspec.log', "-ExecCmds=`"$exec`"")
if (-not $p.WaitForExit($Timeout * 1000)) { Stop-Process -Id $p.Id -Force; 'killed on timeout' }
$log = "$root\Client\Saved\Logs\artspec.log"
Select-String -Path $log -Pattern '\[ArtSpec\]|Critical error|Unhandled Exception' | ForEach-Object { $_.Line.Substring(0, [Math]::Min(200, $_.Line.Length)) }
Get-ChildItem $Out -Filter *.png | ForEach-Object { $_.Name }
