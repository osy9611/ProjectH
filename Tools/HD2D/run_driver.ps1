# Launches a map in -game with battle_driver.py and captures the window whenever the driver asks.
# The driver cannot capture UMG itself, so it drops a <tag>.req file into $Out and this script answers with <tag>.png.
param(
    [Parameter(Mandatory)][string]$Out,
    [Parameter(Mandatory)][string]$Steps,
    [string]$Map = '/Game/Maps/Village/MAP_Village_Day',
    [int]$Timeout = 240,
    [string]$Cmds = ''
)
$root = Resolve-Path "$PSScriptRoot\..\.."
New-Item -ItemType Directory -Force $Out | Out-Null
Remove-Item "$Out\*.req" -ErrorAction SilentlyContinue
$env:HD2D_STEPS = $Steps
$env:HD2D_OUT = $Out
$exec = "py $($PSScriptRoot -replace '\\', '/')/battle_driver.py"
if ($Cmds) { $exec = "$Cmds,$exec" }
$p = Start-Process 'C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe' -PassThru -ArgumentList @(
    "$root\Client\ProjectH.uproject", $Map, '-game', '-windowed', '-ResX=1920', '-ResY=1080', '-nosplash', '-nosound', '-unattended',
    '-log=driver.log', "-ExecCmds=`"$exec`"")

Add-Type -AssemblyName System.Drawing
Add-Type @'
using System;using System.Runtime.InteropServices;
public class WD{
[DllImport("user32.dll")]public static extern bool PrintWindow(IntPtr h,IntPtr d,uint f);
[DllImport("user32.dll")]public static extern bool GetClientRect(IntPtr h,out R r);
[DllImport("user32.dll")]public static extern bool SetProcessDPIAware();
[DllImport("user32.dll")]public static extern bool ClientToScreen(IntPtr h,ref P p);
[DllImport("user32.dll")]public static extern bool SetCursorPos(int x,int y);
[DllImport("user32.dll")]public static extern bool SetForegroundWindow(IntPtr h);
[DllImport("user32.dll")]public static extern void mouse_event(uint f,uint x,uint y,uint d,UIntPtr e);
[DllImport("user32.dll")]public static extern void keybd_event(byte v,byte s,uint f,UIntPtr e);
[StructLayout(LayoutKind.Sequential)]public struct P{public int X,Y;}
[StructLayout(LayoutKind.Sequential)]public struct R{public int L,T,Rt,B;}}
'@
# Screen coordinates are virtualized for a process that is not DPI aware.
[void][WD]::SetProcessDPIAware()
function Click([int]$x, [int]$y) {
    $p.Refresh(); $h = $p.MainWindowHandle
    # Windows only grants foreground to a process that just received input, hence the Alt tap.
    [WD]::keybd_event(0x12, 0, 0, [UIntPtr]::Zero); [WD]::keybd_event(0x12, 0, 2, [UIntPtr]::Zero)
    [void][WD]::SetForegroundWindow($h)
    $pt = New-Object WD+P; $pt.X = $x; $pt.Y = $y; [void][WD]::ClientToScreen($h, [ref]$pt)
    [void][WD]::SetCursorPos($pt.X, $pt.Y); Start-Sleep -Milliseconds 150
    [WD]::mouse_event(1, 1, 0, 0, [UIntPtr]::Zero); Start-Sleep -Milliseconds 150
    [WD]::mouse_event(2, 0, 0, 0, [UIntPtr]::Zero); Start-Sleep -Milliseconds 80; [WD]::mouse_event(4, 0, 0, 0, [UIntPtr]::Zero)
}
function Shot([string]$tag) {
    $p.Refresh(); $h = $p.MainWindowHandle
    $r = New-Object WD+R; [void][WD]::GetClientRect($h, [ref]$r)
    $b = New-Object Drawing.Bitmap ([Math]::Max($r.Rt, 1)), ([Math]::Max($r.B, 1))
    $g = [Drawing.Graphics]::FromImage($b); $d = $g.GetHdc()
    [void][WD]::PrintWindow($h, $d, 3)
    $g.ReleaseHdc($d); $b.Save("$Out\$tag.png"); $g.Dispose(); $b.Dispose()
}

$sw = [Diagnostics.Stopwatch]::StartNew()
while (-not $p.HasExited -and $sw.Elapsed.TotalSeconds -lt $Timeout) {
    Get-ChildItem "$Out\*.req" -ErrorAction SilentlyContinue | ForEach-Object {
        if ($_.BaseName -match '^click_(\d+)_(\d+)$') { Click $Matches[1] $Matches[2] } else { Shot $_.BaseName }
        Remove-Item $_.FullName
    }
    Start-Sleep -Milliseconds 100
}
if (-not $p.HasExited) { Stop-Process -Id $p.Id -Force; 'killed on timeout' }
Start-Sleep 2
$log = "$root\Client\Saved\Logs\driver.log"
Copy-Item $log "$Out\driver.log" -Force
"ensures=" + @(Select-String -Path $log -Pattern 'Ensure condition failed').Count
"missing=" + @(Select-String -Path $log -Pattern "Failed to find object|Can't find file|Failed to load '/|LoadErrors").Count
Select-String -Path $log -Pattern '\[Driver\]|Critical error|Unhandled Exception|LoadMap:|\[Callstack\].*ProjectH' |
    ForEach-Object { $_.Line.Substring(0, [Math]::Min(230, $_.Line.Length)) }
