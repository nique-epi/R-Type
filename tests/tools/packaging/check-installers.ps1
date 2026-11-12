# Checks the Windows installer of the server once it is installed.
#
# Usage: pwsh tests/tools/packaging/check-installers.ps1 <folder holding the packages>
#
# The CI runs it right after check-package.sh, on Windows only. It installs
# the installer of the server in silent mode, then answers two questions, and
# stops at the first "no":
#
#   1. Does the Start menu shortcut point at an executable that exists, inside
#      the folder the installer was given? A shortcut whose target is missing
#      shows no icon and fails with "cannot find the file".
#
#   2. Does that executable start, run through the target of the shortcut?
#
# The installer of the client is written by vpk (packaging/VelopackClient.cmake),
# not by cpack: it takes other arguments and is not checked here.

param(
  [Parameter(Mandatory = $true)]
  [string] $PackagesFolder
)

$ErrorActionPreference = 'Stop'

$shortcutTitle = 'R-Type Server'

function Fail([string] $Message) {
  Write-Host "FAILED: $Message"
  exit 1
}

function Passed([string] $Message) {
  Write-Host "OK: $Message"
}

function Install-Server {
  $installer = Get-ChildItem -Path $PackagesFolder -Filter 'R-Type-server-*-win64.exe' |
    Select-Object -First 1
  if (-not $installer) {
    Fail "no installer of the server in $PackagesFolder"
  }
  $folder = Join-Path ([System.IO.Path]::GetTempPath()) 'rtype-installed-server'
  $process = Start-Process -FilePath $installer.FullName `
    -ArgumentList '/S', "/D=$folder" -Wait -PassThru
  if ($process.ExitCode -ne 0) {
    Fail "the installer of the server exited with code $($process.ExitCode)"
  }
  return $folder
}

function Find-Shortcut {
  $roots = @(
    [Environment]::GetFolderPath('CommonPrograms'),
    [Environment]::GetFolderPath('Programs')
  )
  foreach ($root in $roots) {
    $found = Get-ChildItem -Path $root -Recurse -Filter "$shortcutTitle.lnk" `
      -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($found) {
      return $found.FullName
    }
  }
  Fail "no Start menu shortcut named $shortcutTitle.lnk"
}

$folder = Install-Server
$link = Find-Shortcut
$shortcut = (New-Object -ComObject WScript.Shell).CreateShortcut($link)
$target = $shortcut.TargetPath

if (-not (Test-Path -LiteralPath $target -PathType Leaf)) {
  Fail "the shortcut of the server points at $target, which does not exist"
}
if (-not $target.StartsWith($folder, [System.StringComparison]::OrdinalIgnoreCase)) {
  Fail "the shortcut of the server points at $target, outside $folder"
}
$iconFile = ($shortcut.IconLocation -split ',')[0]
if ($iconFile -and -not (Test-Path -LiteralPath $iconFile -PathType Leaf)) {
  Fail "the shortcut of the server takes its icon from $iconFile, which does not exist"
}
Passed 'the shortcut of the server points at an executable that exists'

$run = Start-Process -FilePath $target -ArgumentList '--help' `
  -Wait -PassThru -NoNewWindow -RedirectStandardOutput ([System.IO.Path]::GetTempFileName())
if ($run.ExitCode -ne 0) {
  Fail "the server started from its shortcut exited with code $($run.ExitCode)"
}
Passed 'the server starts from the target of its shortcut'
