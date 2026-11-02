# Runs a village map with the real game mode and GameInstance at 1920x1080, holds keys
# through window messages, and saves window captures plus a CSV profile.
# Steps look like "shot:start" "keys:D:4:run_right:8" (key:seconds:tag:frames) "wait:3".
param(
    [Parameter(Mandatory)][string]$Map,
    [Parameter(Mandatory)][string]$Out,
    [string[]]$Steps = @('wait:5', 'shot:idle'),
    [int]$LoadWait = 45,
    [string]$Cmds = '',
    [string]$MapDir = '/Game/Maps/Village'
)
$root = Resolve-Path "$PSScriptRoot\..\.."
New-Item -ItemType Directory -Force $Out | Out-Null
$exe = 'C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe'
$cmdline = @("$root\Client\ProjectH.uproject", "$MapDir/$Map", '-game', '-windowed', '-ResX=1920', '-ResY=1080', '-nosplash', '-nosound', '-unattended', "-log=$Map.log")
if ($Cmds) { $cmdline += "-ExecCmds=`"$Cmds`"" }
$p = Start-Process $exe -ArgumentList $cmdline -PassThru

Add-Type -AssemblyName System.Drawing
Add-Type @'
using System;using System.Runtime.InteropServices;
public class W{
[DllImport("user32.dll")]public static extern bool PrintWindow(IntPtr h,IntPtr d,uint f);
[DllImport("user32.dll")]public static extern bool GetClientRect(IntPtr h,out R r);
[DllImport("user32.dll")]public static extern bool PostMessage(IntPtr h,uint m,IntPtr w,IntPtr l);
[DllImport("user32.dll")]public static extern bool SetForegroundWindow(IntPtr h);
[DllImport("user32.dll")]public static extern uint MapVirtualKey(uint c,uint t);
[DllImport("user32.dll")]public static extern IntPtr GetForegroundWindow();
[DllImport("user32.dll")]public static extern void mouse_event(uint f,uint x,uint y,uint d,UIntPtr e);
[DllImport("user32.dll")]public static extern void keybd_event(byte v,byte s,uint f,UIntPtr e);
[DllImport("user32.dll")]public static extern bool SetCursorPos(int x,int y);
[DllImport("user32.dll")]public static extern bool ClientToScreen(IntPtr h,ref P p);
[StructLayout(LayoutKind.Sequential)]public struct P{public int X,Y;}
[StructLayout(LayoutKind.Sequential)]public struct R{public int L,T,Rt,B;}}
'@

function Hwnd { $p.Refresh(); $p.MainWindowHandle }
function Shot([string]$tag) {
    $h = Hwnd
    $r = New-Object W+R; [void][W]::GetClientRect($h, [ref]$r)
    $b = New-Object Drawing.Bitmap ([Math]::Max($r.Rt, 1)), ([Math]::Max($r.B, 1))
    $g = [Drawing.Graphics]::FromImage($b); $d = $g.GetHdc()
    [void][W]::PrintWindow($h, $d, 3)
    $g.ReleaseHdc($d); $b.Save("$Out\$tag.png"); $g.Dispose(); $b.Dispose()
}
function Key([string]$k, [bool]$down) {
    $vk = [uint32][char]$k
    $scan = [W]::MapVirtualKey($vk, 0)
    $l = 1 -bor ($scan -shl 16)
    if (-not $down) { $l = $l -bor 0xC0000000 }
    [void][W]::PostMessage((Hwnd), $(if ($down) { 0x100 } else { 0x101 }), [IntPtr]$vk, [IntPtr]([int64]$l -band 0xFFFFFFFF))
}

Start-Sleep $LoadWait
if ($p.HasExited -or (Hwnd) -eq [IntPtr]::Zero) { throw 'game exited during load' }
# Windows only grants foreground to a process that just received input, hence the Alt tap.
for ($try = 0; $try -lt 60 -and [W]::GetForegroundWindow() -ne (Hwnd); $try++) {
    [W]::keybd_event(0x12, 0, 0, [UIntPtr]::Zero); [W]::keybd_event(0x12, 0, 2, [UIntPtr]::Zero)
    [void][W]::SetForegroundWindow((Hwnd))
    Start-Sleep 1
}
"foreground=$([W]::GetForegroundWindow() -eq (Hwnd))"
Start-Sleep 1
foreach ($s in $Steps) {
    $a = $s -split ':'
    switch ($a[0]) {
        'wait' { Start-Sleep ([double]$a[1]) }
        'shot' { Shot $a[1] }
        'click' { $pt = New-Object W+P; $pt.X = [int]$a[1]; $pt.Y = [int]$a[2]; [void][W]::ClientToScreen((Hwnd), [ref]$pt); [void][W]::SetCursorPos($pt.X, $pt.Y); [W]::mouse_event(2, 0, 0, 0, [UIntPtr]::Zero); Start-Sleep -Milliseconds 100; [W]::mouse_event(4, 0, 0, 0, [UIntPtr]::Zero) }
        'keys' {
            $sw = [Diagnostics.Stopwatch]::StartNew()
            $keys = $a[1].ToCharArray() | ForEach-Object { "$_" }
            $keys | ForEach-Object { Key $_ $true }
            $n = 0
            while ($sw.Elapsed.TotalSeconds -lt [double]$a[2]) {
                # A held key auto-repeats in Windows; the engine treats a repeat as still pressed.
                if ($a.Count -gt 3 -and $n -lt [int]$a[4]) { Shot ('{0}_{1:d2}' -f $a[3], $n); $n++ }
                else { Start-Sleep -Milliseconds 100 }
            }
            $keys | ForEach-Object { Key $_ $false }
        }
    }
}
"title=$($p.MainWindowTitle) exited=$($p.HasExited)"
# A clean exit flushes the CSV profile; killing the process loses it.
[void]$p.CloseMainWindow()
if (-not $p.WaitForExit(30000)) { Stop-Process -Id $p.Id -Force -ErrorAction SilentlyContinue; 'killed after close timeout' }
Start-Sleep 2
$log = "$root\Client\Saved\Logs\$Map.log"
Copy-Item $log "$Out\$Map.log" -Force
"ensures=" + @(Select-String -Path $log -Pattern 'Ensure condition failed').Count
$missing = @(Select-String -Path $log -Pattern "Failed to find object|Can't find file|Failed to load '/|LoadErrors")
"missing=" + $missing.Count
$missing | Select-Object -First 5 | ForEach-Object { $_.Line }
