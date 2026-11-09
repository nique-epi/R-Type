# Checks the Windows installers of the game once they are installed.
#
# Usage: pwsh tests/tools/packaging/check-installers.ps1 <folder holding the packages>
#
# The CI runs it right after check-package.sh, on Windows only. For the client
# and for the server it installs the installer in silent mode, then answers
# two questions, and stops at the first "no":
#
#   1. Does the Start menu shortcut point at an executable that exists, inside
#      the folder the installer was given? A shortcut whose target is missing
#      shows no icon and fails with "cannot find the file".
#
#   2. Does that executable start, run through the target of the shortcut?

param(
  [Parameter(Mandatory = $true)]
  [string] $PackagesFolder
)

$ErrorActionPreference = 'Stop'

$programs = @(
  @{ Part = 'client'; Title = 'R-Type' },
  @{ Part = 'server'; Title = 'R-Type Server' }
)

function Fail([string] $Message) {
  Write-Host "FAILED: $Message"
  exit 1
}

function Passed([string] $Message) {
  Write-Host "OK: $Message"
}

function Install-Program([string] $Part) {
  $installer = Get-ChildItem -Path $PackagesFolder -Filter "R-Type-$Part-*-win64.exe" |
    Select-Object -First 1
  if (-not $installer) {
    Fail "no installer of the $Part in $PackagesFolder"
  }
  $folder = Join-Path ([System.IO.Path]::GetTempPath()) "rtype-installed-$Part"
  $process = Start-Process -FilePath $installer.FullName `
    -ArgumentList '/S', "/D=$folder" -Wait -PassThru
  if ($process.ExitCode -ne 0) {
    Fail "the installer of the $Part exited with code $($process.ExitCode)"
  }
  return $folder
}

function Find-Shortcut([string] $Title) {
  $roots = @(
    [Environment]::GetFolderPath('CommonPrograms'),
    [Environment]::GetFolderPath('Programs')
  )
  foreach ($root in $roots) {
    $found = Get-ChildItem -Path $root -Recurse -Filter "$Title.lnk" `
      -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($found) {
      return $found.FullName
    }
  }
  Fail "no Start menu shortcut named $Title.lnk"
}

foreach ($program in $programs) {
  $part = $program.Part
  $folder = Install-Program $part
  $link = Find-Shortcut $program.Title
  $shortcut = (New-Object -ComObject WScript.Shell).CreateShortcut($link)
  $target = $shortcut.TargetPath

  if (-not (Test-Path -LiteralPath $target -PathType Leaf)) {
    Fail "the shortcut of the $part points at $target, which does not exist"
  }
  if (-not $target.StartsWith($folder, [System.StringComparison]::OrdinalIgnoreCase)) {
    Fail "the shortcut of the $part points at $target, outside $folder"
  }
  $iconFile = ($shortcut.IconLocation -split ',')[0]
  if ($iconFile -and -not (Test-Path -LiteralPath $iconFile -PathType Leaf)) {
    Fail "the shortcut of the $part takes its icon from $iconFile, which does not exist"
  }
  Passed "the shortcut of the $part points at an executable that exists"

  $run = Start-Process -FilePath $target -ArgumentList '--help' `
    -Wait -PassThru -NoNewWindow -RedirectStandardOutput ([System.IO.Path]::GetTempFileName())
  if ($run.ExitCode -ne 0) {
    Fail "the $part started from its shortcut exited with code $($run.ExitCode)"
  }
  Passed "the $part starts from the target of its shortcut"
}
