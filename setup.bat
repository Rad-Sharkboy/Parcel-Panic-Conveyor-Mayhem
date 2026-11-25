@echo off
setlocal EnableExtensions DisableDelayedExpansion
rem Parcel Panic - autonomous Windows developer bootstrap, v1.0
rem Keep this file beside CMakeLists.txt. No companion scripts required.
set "PP_SETUP_FILE=%~f0"
set "PP_INSTALL_ONLY="
set "PP_DEBUG="
set "PP_RUN="
set "PP_UPDATE="
set "PP_ADD_PATH="
set "PP_NO_PAUSE="
set "PP_FRESH="
:parse
if "%~1"=="" goto execute
if /i "%~1"=="/help" goto help
if /i "%~1"=="/?" goto help
if /i "%~1"=="/install-only" (set "PP_INSTALL_ONLY=1"& shift & goto parse)
if /i "%~1"=="/debug" (set "PP_DEBUG=1"& shift & goto parse)
if /i "%~1"=="/run" (set "PP_RUN=1"& shift & goto parse)
if /i "%~1"=="/update" (set "PP_UPDATE=1"& shift & goto parse)
if /i "%~1"=="/add-path" (set "PP_ADD_PATH=1"& shift & goto parse)
if /i "%~1"=="/no-pause" (set "PP_NO_PAUSE=1"& shift & goto parse)
if /i "%~1"=="/fresh" (set "PP_FRESH=1"& shift & goto parse)
echo ERROR: Unknown argument. Use setup.bat /help.
exit /b 2
:help
echo Parcel Panic - Windows setup
echo   setup.bat                  Install missing tools and build Release
echo   setup.bat /run             Build and launch the game
echo   setup.bat /debug           Build Debug instead of Release
echo   setup.bat /install-only    Prepare tools without compiling the game
echo   setup.bat /update          Update the private MSYS2 installation
echo   setup.bat /fresh           Use a new build folder; preserve old output
echo   setup.bat /add-path        Also add tools to the current user's PATH
echo   setup.bat /no-pause        Do not pause on errors (automatic in CI)
echo   Optional environment: PARCEL_SETUP_HOME (private, space-free local path)
echo   Options may be combined. Internet required for first-time setup.
exit /b 0
:execute
set "PP_PS=%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe"
if exist "%SystemRoot%\Sysnative\WindowsPowerShell\v1.0\powershell.exe" set "PP_PS=%SystemRoot%\Sysnative\WindowsPowerShell\v1.0\powershell.exe"
if not exist "%PP_PS%" (
    echo ERROR: Windows PowerShell 5.1 is required.
    if not defined CI if not defined PP_NO_PAUSE pause
    exit /b 1
)
"%PP_PS%" -NoLogo -NoProfile -NonInteractive -Command "$ErrorActionPreference='Stop'; try { $s=[IO.File]::ReadAllText($env:PP_SETUP_FILE); $m='#=== PARCEL PANIC POWERSHELL ==='; $i=$s.LastIndexOf($m,[StringComparison]::Ordinal); if($i -lt 0){throw 'Embedded setup payload missing'}; & ([ScriptBlock]::Create($s.Substring($i+$m.Length))) } catch { Write-Host ('SETUP ERROR: '+$_.Exception.Message) -ForegroundColor Red; exit 1 }"
set "PP_EXIT=%ERRORLEVEL%"
if not "%PP_EXIT%"=="0" if not defined CI if not defined PP_NO_PAUSE pause
exit /b %PP_EXIT%
#=== PARCEL PANIC POWERSHELL ===
# Windows PowerShell 5.1 compatible. The batch wrapper reads this payload in memory.
# MSYS2 is deliberately private; no administrator or system-wide PATH changes.
# Official references: https://www.msys2.org/docs/installer/
# https://www.msys2.org/docs/updating/ ; https://www.msys2.org/docs/ci/
$ErrorActionPreference = 'Stop'
$ProgressPreference = 'SilentlyContinue'
$script:Phase = 'Preflight'
$script:Log = $null
$script:Lock = $null
$script:ProjectLock = $null
$script:OriginalPath = $env:Path
$script:Clock = [Diagnostics.Stopwatch]::StartNew()
$script:Stamp = (Get-Date -Format 'yyyyMMdd-HHmmss') + '-' + $PID
$script:Project = Split-Path -Parent $env:PP_SETUP_FILE
$script:ToolHome = $null
$script:Root = $null
$script:Bin = $null
$script:Owned = $false
$script:BuildDir = $null

function Say([string]$Message, [string]$Color = 'Gray') {
    $line = '[{0:HH:mm:ss}] {1}' -f (Get-Date), $Message
    Write-Host $line -ForegroundColor $Color
    if ($script:Log) { Add-Content -LiteralPath $script:Log -Value $line -Encoding UTF8 }
}
function Phase([string]$Name) {
    $script:Phase = $Name
    Say "=== $Name ===" Cyan
}
function Require([bool]$Condition, [string]$Message) {
    if (-not $Condition) { throw $Message }
}
function Invoke-Native([string]$Exe, [string[]]$Arguments, [switch]$AllowFailure) {
    Say ('> ' + $Exe + ' ' + ($Arguments -join ' ')) DarkGray
    # In PS 5.1 native stderr can be wrapped as ErrorRecord. Do not mistake
    # compiler diagnostics/progress messages for a failed process.
    $ErrorActionPreference = 'Continue'
    $global:LASTEXITCODE = 0
    & $Exe @Arguments 2>&1 | ForEach-Object {
        $line = $_.ToString()
        Write-Host $line
        if ($script:Log) { Add-Content -LiteralPath $script:Log -Value $line -Encoding UTF8 }
    }
    $code = $global:LASTEXITCODE
    if ($code -ne 0 -and -not $AllowFailure) { throw "Exit $code from $Exe (see log)." }
    return $code
}
function Retry([string]$Name, [scriptblock]$Action, [int]$Attempts = 3) {
    for ($i=1; $i -le $Attempts; $i++) {
        try { return (& $Action) }
        catch {
            if ($i -eq $Attempts) { throw }
            Say "$Name failed ($i/$Attempts): $($_.Exception.Message)" Yellow
            Start-Sleep -Seconds (3 * $i)
        }
    }
}
function Acquire-Lock([string]$File) {
    for ($i=0; $i -lt 20; $i++) {
        try { return [IO.File]::Open($File, 'OpenOrCreate', 'ReadWrite', 'None') }
        catch [IO.IOException] {
            if ($i -eq 0) { Say "Another setup may be active; waiting for $File" Yellow }
            Start-Sleep -Seconds 3
        }
    }
    throw "Cannot acquire setup lock: $File. Close other setup processes and retry."
}
function Check-Space([string]$Folder, [long]$Minimum, [string]$Purpose) {
    $drive = New-Object IO.DriveInfo ([IO.Path]::GetPathRoot($Folder))
    Require ($drive.DriveType -eq [IO.DriveType]::Fixed) "$Purpose requires a local fixed drive: $Folder"
    $free = $drive.AvailableFreeSpace
    Say ('{0}: {1:N1} GiB available on {2}' -f $Purpose, ($free / 1GB), $drive.Name)
    Require ($free -ge $Minimum) "Insufficient free disk space for $Purpose."
}
function Is-ToolPath([string]$Path) {
    # MSYS2 recommends an ASCII, space-free installation path. Exclude shell
    # metacharacters too, and keep the path short for deep package trees.
    return ($Path -match '^[A-Za-z]:\\[A-Za-z0-9_.~\\-]+$' -and $Path.Length -le 110)
}
function Get-ToolHome {
    if ($env:PARCEL_SETUP_HOME) {
        Require ([IO.Path]::IsPathRooted($env:PARCEL_SETUP_HOME)) 'PARCEL_SETUP_HOME must be an absolute local path.'
        $candidate = [IO.Path]::GetFullPath($env:PARCEL_SETUP_HOME).TrimEnd('\')
        Require (Is-ToolPath $candidate) 'PARCEL_SETUP_HOME must be short, ASCII and space-free (e.g. D:\ParcelPanicTools).'
        return $candidate
    }
    $base = [Environment]::GetFolderPath('LocalApplicationData')
    $candidate = Join-Path $base 'ParcelPanicTools'
    if (Is-ToolPath $candidate) { return $candidate }
    # Use Windows short-name alias only if one is actually available.
    try {
        $fso = New-Object -ComObject Scripting.FileSystemObject
        $short = $fso.GetFolder($base).ShortPath
        $candidate = Join-Path $short 'ParcelPanicTools'
        if (Is-ToolPath $candidate) { return $candidate }
    } catch { Say 'Windows short-path alias unavailable.' DarkGray }
    # A space-free project folder provides a safe non-admin fallback.
    $candidate = Join-Path $script:Project 'build\ParcelPanicTools'
    if (Is-ToolPath $candidate) { return $candidate }
    throw 'No suitable space-free tool folder found. Set PARCEL_SETUP_HOME to a writable path such as D:\ParcelPanicTools and rerun.'
}
function Set-ToolPath([string]$Bin) {
    # Only native UCRT64 tools on the build PATH, not MSYS /usr/bin.
    # Retain the original PATH after our toolchain to support system utilities.
    $env:Path = "$Bin;$script:OriginalPath"
}
function Test-Toolchain([string]$Bin) {
    if (-not $Bin) { return $false }
    foreach ($name in @('gcc.exe','g++.exe','mingw32-make.exe','cmake.exe','windres.exe','objdump.exe')) {
        if (-not (Test-Path -LiteralPath (Join-Path $Bin $name) -PathType Leaf)) { return $false }
    }
    $oldPath = $env:Path
    try {
        Set-ToolPath $Bin
        $ErrorActionPreference = 'Continue'
        $ver = & (Join-Path $Bin 'cmake.exe') --version 2>$null
        if ($LASTEXITCODE -ne 0) { return $false }
        if (-not (($ver -join ' ') -match 'cmake version (\d+\.\d+(?:\.\d+)?)')) { return $false }
        if ([version]$Matches[1] -lt [version]'3.22') { return $false }
        $target = & (Join-Path $Bin 'g++.exe') -dumpmachine 2>$null
        return ($LASTEXITCODE -eq 0 -and ($target -join '') -eq 'x86_64-w64-mingw32')
    } catch { return $false }
    finally { $env:Path = $oldPath }
}
function Get-ExistingBin {
    # Never update a pre-existing shared MSYS2 installation without consent.
    $candidates = @((Join-Path $env:SystemDrive 'msys64\ucrt64\bin'))
    $candidates += @($script:OriginalPath -split ';' | ForEach-Object { $_.Trim().Trim('"').TrimEnd('\') } | Where-Object { $_ -match '\\ucrt64\\bin$' })
    foreach ($bin in ($candidates | Select-Object -Unique)) {
        if ((Is-ToolPath $bin) -and (Test-Toolchain $bin)) { return $bin }
    }
    return $null
}
function Get-ReleaseAsset {
    # Select a dated release, not the repository's rolling nightly "latest".
    $api = 'https://api.github.com/repos/msys2/msys2-installer/releases?per_page=20'
    $releases = @(Retry 'Release metadata' { $result = Invoke-RestMethod -Uri $api -UseBasicParsing -TimeoutSec 60 -Headers @{'User-Agent'='ParcelPanic-Setup/1.0';'Accept'='application/vnd.github+json'}; foreach ($entry in $result) { $entry } })
    $release = $releases | Where-Object { $_.tag_name -match '^\d{4}-\d{2}-\d{2}$' -and -not $_.draft -and -not $_.prerelease } | Sort-Object tag_name -Descending | Select-Object -First 1
    Require ($null -ne $release) 'No dated stable MSYS2 release found.'
    $asset = $release.assets | Where-Object { $_.name -match '^msys2-base-x86_64-\d{8}\.sfx\.exe$' } | Select-Object -First 1
    Require ($null -ne $asset) 'Stable MSYS2 x64 archive is missing.'
    $url = [string]$asset.browser_download_url
    Require ($url -match '^https://github\.com/msys2/msys2-installer/releases/download/\d{4}-\d{2}-\d{2}/msys2-base-x86_64-\d{8}\.sfx\.exe$') 'Unexpected MSYS2 download URL.'
    $expected = $null
    if ([string]$asset.digest -match '^sha256:([a-fA-F0-9]{64})$') { $expected = $Matches[1] }
    if (-not $expected) {
        $checksum = Retry 'Release checksum' { (Invoke-WebRequest -Uri ($url + '.sha256') -UseBasicParsing -TimeoutSec 60).Content }
        if ($checksum -match '(?im)^\s*([0-9a-f]{64})(?:\s|$)') { $expected = $Matches[1] }
    }
    Require (-not [string]::IsNullOrWhiteSpace($expected)) 'Official SHA-256 unavailable; refusing unverified download.'
    Say "Selected MSYS2 release $($release.tag_name)."
    return @{Url=$url; Hash=$expected; Name=[string]$asset.name}
}
function Install-MSYS2 {
    $asset = Get-ReleaseAsset
    $downloads = Join-Path $script:ToolHome 'downloads'
    [void](New-Item -ItemType Directory -Path $downloads -Force)
    $archive = Join-Path $downloads $asset.Name
    if ((Test-Path -LiteralPath $archive) -and (Get-FileHash -LiteralPath $archive -Algorithm SHA256).Hash -ne $asset.Hash) {
        Remove-Item -LiteralPath $archive -Force
    }
    if (-not (Test-Path -LiteralPath $archive)) {
        Retry 'MSYS2 download' {
            $part = "$archive.part"
            if (Test-Path -LiteralPath $part) { Remove-Item -LiteralPath $part -Force }
            try {
                Say "Downloading $($asset.Url)"
                Invoke-WebRequest -Uri $asset.Url -UseBasicParsing -TimeoutSec 900 -OutFile $part
                Require ((Get-FileHash -LiteralPath $part -Algorithm SHA256).Hash -eq $asset.Hash) 'MSYS2 SHA-256 mismatch.'
                Move-Item -LiteralPath $part -Destination $archive
            } finally { if (Test-Path -LiteralPath $part) { Remove-Item -LiteralPath $part -Force } }
        }
    }
    Require ((Get-FileHash -LiteralPath $archive -Algorithm SHA256).Hash -eq $asset.Hash) 'MSYS2 archive integrity check failed.'
    Say 'Verified official release SHA-256.' Green
    $stage = Join-Path $script:ToolHome ('extract-' + [guid]::NewGuid().ToString('N'))
    [void](New-Item -ItemType Directory -Path $stage)
    try {
        # Official self-extracting archive is a GUI-subsystem executable;
        # Start-Process -Wait is essential before inspecting its output.
        $p = Start-Process -FilePath $archive -ArgumentList @('-y', ('-o' + $stage)) -Wait -PassThru
        Require ($p.ExitCode -eq 0) "MSYS2 extraction failed (exit $($p.ExitCode))."
        $extracted = Join-Path $stage 'msys64'
        Require (Test-Path -LiteralPath (Join-Path $extracted 'usr\bin\bash.exe')) 'Extracted MSYS2 layout is unexpected.'
        Require (-not (Test-Path -LiteralPath $script:Root)) 'Private MSYS2 destination unexpectedly exists.'
        Move-Item -LiteralPath $extracted -Destination $script:Root
    } finally {
        if (Test-Path -LiteralPath $stage) { Remove-Item -LiteralPath $stage -Recurse -Force -ErrorAction SilentlyContinue }
    }
}
function Invoke-Bash([string]$Command, [switch]$AllowFailure) {
    # Fresh login shell for each pass, because MSYS2 runtime updates can
    # terminate the current shell and must be continued in a new process.
    $env:MSYSTEM = 'MSYS'
    $env:MSYS2_PATH_TYPE = 'minimal'
    $env:CHERE_INVOKING = '1'
    $env:MSYS = 'winsymlinks:lnk'
    Push-Location -LiteralPath $script:Root
    try { return (Invoke-Native (Join-Path $script:Root 'usr\bin\bash.exe') @('--login','-c',$Command) -AllowFailure:$AllowFailure) }
    finally { Pop-Location }
}
function Update-MSYS2 {
    $successes = 0
    for ($i=1; $i -le 6; $i++) {
        Say "MSYS2 full upgrade pass $i/6."
        $code = Invoke-Bash 'pacman -Syu --noconfirm --color never' -AllowFailure
        if ($code -eq 0) { $successes++ } else { $successes=0; Say "Upgrade returned $code; reopening shell." Yellow }
        if ($successes -ge 2) { return }
        Start-Sleep -Seconds 4
    }
    throw 'MSYS2 could not finish two consecutive upgrade passes. Check connectivity, clock, package locks and the log; rerun setup.'
}
function Install-Packages {
    # --needed makes this idempotent. Pacman retains its package signature checks.
    $cmd = 'pacman -S --needed --noconfirm --color never mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-make mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-binutils'
    Retry 'Toolchain package installation' { [void](Invoke-Bash $cmd) }
}
function Verify-Compiler {
    $gcc = Join-Path $script:Bin 'gcc.exe'
    $gpp = Join-Path $script:Bin 'g++.exe'
    [void](Invoke-Native $gcc @('--version'))
    [void](Invoke-Native $gpp @('--version'))
    [void](Invoke-Native (Join-Path $script:Bin 'cmake.exe') @('--version'))
    [void](Invoke-Native (Join-Path $script:Bin 'mingw32-make.exe') @('--version'))
    $probe = Join-Path $script:LogDir ('probe-' + $PID)
    [void](New-Item -ItemType Directory -Path $probe)
    try {
        $source = Join-Path $probe 'probe.cpp'
        $exe = Join-Path $probe 'probe.exe'
        Set-Content -LiteralPath $source -Encoding ASCII -Value @'
#include <optional>
#include <iostream>
int main() {
    std::optional<int> value = 17;
    std::cout << "C++17 toolchain OK\n";
    return value.value_or(0) == 17 ? 0 : 1;
}
'@
        [void](Invoke-Native $gpp @('-std=c++17','-static','-static-libgcc','-static-libstdc++',$source,'-o',$exe))
        [void](Invoke-Native $exe @())
    } finally { Remove-Item -LiteralPath $probe -Recurse -Force -ErrorAction SilentlyContinue }
    Say 'Compiler smoke test passed.' Green
}
function Add-UserPath {
    # Opt-in only. Preserve every existing registry PATH entry verbatim,
    # including expandable %VARIABLE% tokens (do not use SETX).
    $key = [Microsoft.Win32.Registry]::CurrentUser.CreateSubKey('Environment')
    try {
        $old = [string]$key.GetValue('Path','',[Microsoft.Win32.RegistryValueOptions]::DoNotExpandEnvironmentNames)
        $entries = @($old -split ';' | ForEach-Object { [Environment]::ExpandEnvironmentVariables($_.Trim().Trim('"')).TrimEnd('\') })
        if ($entries -notcontains $script:Bin.TrimEnd('\')) {
            $new = if ($old) { $script:Bin + ';' + $old } else { $script:Bin }
            Require ($new.Length -lt 32767) 'User PATH would exceed Windows environment limit.'
            $kind = [Microsoft.Win32.RegistryValueKind]::ExpandString
            if ($key.GetValueNames() -contains 'Path') { $kind = $key.GetValueKind('Path') }
            $key.SetValue('Path',$new,$kind)
            Say 'Updated user PATH. Sign out/in if new terminals do not see it.' Green
            Say 'Machine PATH entries may still take precedence; setup itself uses explicit tool paths.' Yellow
        } else { Say 'Toolchain is already present in user PATH.' }
    } finally { $key.Dispose() }
}
function Cache-Value([string]$Cache, [string]$Key) {
    $line = Get-Content -LiteralPath $Cache | Where-Object { $_ -match ('^' + [regex]::Escape($Key) + ':[^=]+=') } | Select-Object -First 1
    if ($line) { return ($line -split '=',2)[1] }
    return ''
}
function Same-Path([string]$A,[string]$B) {
    if (-not $A -or -not $B) { return $false }
    try { return ([IO.Path]::GetFullPath($A.Replace('/','\')).TrimEnd('\') -ieq [IO.Path]::GetFullPath($B.Replace('/','\')).TrimEnd('\')) }
    catch { return $false }
}
function Compatible-Build([string]$Directory) {
    if (-not (Test-Path -LiteralPath $Directory)) { return $true }
    $cache = Join-Path $Directory 'CMakeCache.txt'
    if (-not (Test-Path -LiteralPath $cache)) {
        return (-not (Test-Path -LiteralPath (Join-Path $Directory 'CMakeFiles')))
    }
    return ((Cache-Value $cache 'CMAKE_GENERATOR') -eq 'MinGW Makefiles' -and
        (Same-Path (Cache-Value $cache 'CMAKE_HOME_DIRECTORY') $script:Project) -and
        (Same-Path (Cache-Value $cache 'CMAKE_C_COMPILER') (Join-Path $script:Bin 'gcc.exe')) -and
        (Same-Path (Cache-Value $cache 'CMAKE_CXX_COMPILER') (Join-Path $script:Bin 'g++.exe')) -and
        (Same-Path (Cache-Value $cache 'CMAKE_MAKE_PROGRAM') (Join-Path $script:Bin 'mingw32-make.exe')) -and
        (Same-Path (Cache-Value $cache 'CMAKE_RC_COMPILER') (Join-Path $script:Bin 'windres.exe')))
}
function Select-BuildDir {
    $normal = Join-Path $script:Project 'build'
    $alternate = Join-Path $normal 'setup-ucrt64'
    if (-not $env:PP_FRESH) {
        if (Compatible-Build $normal) { return $normal }
        Say 'Existing CMake cache uses different tools or source path; preserving it.' Yellow
        if (Compatible-Build $alternate) { return $alternate }
    }
    return (Join-Path $normal ('setup-ucrt64-' + $script:Stamp))
}
function Configure-Build {
    $cmake = Join-Path $script:Bin 'cmake.exe'
    $mode = if ($env:PP_DEBUG) { 'Debug' } else { 'Release' }
    # Keep the repo's MinGW Makefiles generator, target and FetchContent pin.
    $argv = @('-S',$script:Project,'-B',$script:BuildDir,'-G','MinGW Makefiles',
        "-DCMAKE_BUILD_TYPE=$mode",'-DCMAKE_EXPORT_COMPILE_COMMANDS=ON',
        '-DPARCEL_BUILD_GAME=ON','-DPARCEL_SANITIZERS=OFF',
        ('-DCMAKE_C_COMPILER=' + (Join-Path $script:Bin 'gcc.exe').Replace('\','/')),
        ('-DCMAKE_CXX_COMPILER=' + (Join-Path $script:Bin 'g++.exe').Replace('\','/')),
        ('-DCMAKE_RC_COMPILER=' + (Join-Path $script:Bin 'windres.exe').Replace('\','/')),
        ('-DCMAKE_MAKE_PROGRAM=' + (Join-Path $script:Bin 'mingw32-make.exe').Replace('\','/')))
    Say "Configuring $mode build: $script:BuildDir"
    # One bounded retry also covers transient Raylib download failures. CMake
    # verifies the SHA-256 declared in the project's own CMakeLists.txt.
    Retry 'CMake configuration' { [void](Invoke-Native $cmake $argv) } 2
    $jobs = [Math]::Max(1,[Math]::Min(8,[Environment]::ProcessorCount))
    try {
        $ram = [long](Get-CimInstance Win32_OperatingSystem).FreePhysicalMemory * 1KB
        $jobs = [Math]::Max(1,[Math]::Min($jobs,[int][Math]::Floor($ram / 1GB)))
    } catch { $jobs = [Math]::Min($jobs,4) }
    Say "Building with $jobs parallel job(s)."
    $code = Invoke-Native $cmake @('--build',$script:BuildDir,'--parallel',[string]$jobs) -AllowFailure
    if ($code -ne 0 -and $jobs -gt 1) {
        Say 'Parallel build failed; retrying with one job (useful on low-memory PCs).' Yellow
        [void](Invoke-Native $cmake @('--build',$script:BuildDir,'--parallel','1'))
    } elseif ($code -ne 0) { throw "Game compilation failed (exit $code)." }
    # POST_BUILD may not execute on an incremental build; synchronize assets
    # explicitly so updates to fonts, textures, etc. are not missed.
    [void](Invoke-Native $cmake @('-E','copy_directory',(Join-Path $script:Project 'assets'),(Join-Path $script:BuildDir 'assets')))
}
function Verify-Game {
    $exe = Join-Path $script:BuildDir 'ParcelPanic.exe'
    Require (Test-Path -LiteralPath $exe -PathType Leaf) 'Build did not produce ParcelPanic.exe.'
    Require ((Get-Item -LiteralPath $exe).Length -gt 1024) 'Game executable is unexpectedly small.'
    # Read PE machine field: AMD64 = 0x8664. Do not launch GUI for verification.
    $stream = [IO.File]::OpenRead($exe)
    $reader = New-Object IO.BinaryReader($stream)
    try {
        Require ($reader.ReadUInt16() -eq 0x5A4D) 'Game is not a Windows PE executable.'
        $stream.Position=0x3c
        $offset=$reader.ReadInt32()
        Require ($offset -ge 64 -and $offset -le ($stream.Length-6)) 'Invalid PE header offset.'
        $stream.Position=$offset
        Require ($reader.ReadUInt32() -eq 0x4550) 'Invalid PE signature.'
        Require ($reader.ReadUInt16() -eq 0x8664) 'Game is not an x64 Windows executable.'
    } finally { $reader.Dispose() }
    $assets = Join-Path $script:Project 'assets'
    $outAssets = Join-Path $script:BuildDir 'assets'
    $files = @(Get-ChildItem -LiteralPath $assets -File -Recurse -Force)
    Require ($files.Count -gt 0) 'No source assets found.'
    foreach ($file in $files) {
        $relative = $file.FullName.Substring($assets.Length).TrimStart('\')
        $dest = Join-Path $outAssets $relative
        Require (Test-Path -LiteralPath $dest -PathType Leaf) "Missing built asset: $relative"
        Require ((Get-FileHash -LiteralPath $file.FullName -Algorithm SHA256).Hash -eq (Get-FileHash -LiteralPath $dest -Algorithm SHA256).Hash) "Asset differs: $relative"
    }
    $objdump = Join-Path $script:Bin 'objdump.exe'
    $imports = & $objdump -p $exe 2>$null
    Require ($LASTEXITCODE -eq 0) 'Could not inspect executable imports.'
    $dlls = @($imports | ForEach-Object { if ($_ -match '^\s*DLL Name:\s*(.+?)\s*$') { $Matches[1] } })
    Require ($dlls.Count -gt 0) 'No PE imports found; executable inspection failed.'
    Say ('Imported DLLs: ' + ($dlls -join ', ')) DarkGray
    foreach ($dll in $dlls) {
        Require ($dll -notmatch '^(libgcc_s.*|libstdc\+\+.*|libwinpthread.*|raylib.*|msys-2\.0|cygwin1)\.dll$') "Unexpected non-portable runtime dependency: $dll"
    }
    Say "Verified x64 executable and $($files.Count) bundled asset(s)." Green
    Say ('Executable SHA-256: ' + (Get-FileHash -LiteralPath $exe -Algorithm SHA256).Hash)
    return $exe
}

$exitCode = 0
try {
    Write-Host ''
    Write-Host '  PARCEL PANIC / CONVEYOR MAYHEM' -ForegroundColor Cyan
    Write-Host '  Autonomous Windows source-build setup v1.0' -ForegroundColor White
    Write-Host ''
    $arch = if ($env:PROCESSOR_ARCHITEW6432) { $env:PROCESSOR_ARCHITEW6432 } else { $env:PROCESSOR_ARCHITECTURE }
    Require ($arch -eq 'AMD64') 'This setup targets x64 Windows PCs, not ARM64 or 32-bit Windows.'
    Require ($PSVersionTable.PSVersion -ge [version]'5.1') 'Windows PowerShell 5.1 or newer is required.'
    $osBuild = [int](Get-ItemProperty 'HKLM:\SOFTWARE\Microsoft\Windows NT\CurrentVersion').CurrentBuildNumber
    Require ($osBuild -ge 17763) 'Windows 10 version 1809 or newer is required.'
    Require (-not ($env:PP_INSTALL_ONLY -and $env:PP_RUN)) '/install-only and /run cannot be combined.'
    foreach ($item in @('CMakeLists.txt','build.bat','src\main.cpp','include','assets','resources\parcel-panic.rc.in')) {
        Require (Test-Path -LiteralPath (Join-Path $script:Project $item)) "Missing $item. Extract the full project and place setup.bat in its root."
    }
    Require ($script:Project -match '^[A-Za-z]:\\') 'Copy the repository to a local Windows drive before setup.'
    $script:LogDir = Join-Path $script:Project 'build\setup-logs'
    [void](New-Item -ItemType Directory -Path $script:LogDir -Force)
    $script:Log = Join-Path $script:LogDir ('setup-' + $script:Stamp + '.log')
    Say "Project: $script:Project"
    Say "Windows build: $osBuild | Architecture: $arch"
    Say "Log: $script:Log"
    $script:ProjectLock = Acquire-Lock (Join-Path $script:LogDir 'project.lock')
    if (-not $env:PP_INSTALL_ONLY) { Check-Space $script:Project 1GB 'Project build' }
    [Net.ServicePointManager]::SecurityProtocol = [Net.ServicePointManager]::SecurityProtocol -bor [Net.SecurityProtocolType]::Tls12
    Phase 'Dependency discovery'
    $script:ToolHome = Get-ToolHome
    $script:Root = Join-Path $script:ToolHome 'msys64'
    $privateBin = Join-Path $script:Root 'ucrt64\bin'
    if (-not $env:PP_UPDATE -and -not $env:PARCEL_SETUP_HOME -and -not (Test-Path -LiteralPath $script:Root)) {
        $script:Bin = Get-ExistingBin
    }
    if ($script:Bin) {
        Say "Reusing existing UCRT64 tools without modifying their installation: $script:Bin" Green
    } else {
        [void](New-Item -ItemType Directory -Path $script:ToolHome -Force)
        $script:Lock = Acquire-Lock (Join-Path $script:ToolHome 'setup.lock')
        $script:Owned = $true
        $script:Bin = $privateBin
        $marker = Join-Path $script:ToolHome 'toolchain-ready.txt'
        $ready = (Test-Path -LiteralPath $marker) -and (Test-Toolchain $script:Bin)
        if (-not $ready -or $env:PP_UPDATE) {
            Phase 'Private MSYS2 installation and package updates'
            Check-Space $script:ToolHome 4GB 'MSYS2 toolchain installation/update'
            if (Test-Path -LiteralPath $marker) { Remove-Item -LiteralPath $marker -Force }
            if (-not (Test-Path -LiteralPath $script:Root)) { Install-MSYS2 }
            Require (Test-Path -LiteralPath (Join-Path $script:Root 'usr\bin\bash.exe')) 'Private MSYS2 is damaged. Rename its msys64 folder to preserve it, then rerun setup.'
            [void](Invoke-Bash 'true')
            Update-MSYS2
            Install-Packages
            Require (Test-Toolchain $script:Bin) 'Installed toolchain failed dependency validation.'
            Set-Content -LiteralPath $marker -Encoding ASCII -Value (Get-Date -Format o)
        } else { Say "Using cached private toolchain: $script:Bin" Green }
    }
    Phase 'Toolchain verification'
    Set-ToolPath $script:Bin
    # Prevent inherited MSYS variables from leaking into native build tools.
    foreach ($variable in @('MSYSTEM','MSYS2_PATH_TYPE','CHERE_INVOKING','MSYS','CC','CXX','RC','CMAKE_GENERATOR','CMAKE_TOOLCHAIN_FILE','CMAKE_GENERATOR_PLATFORM','CMAKE_GENERATOR_TOOLSET','MAKEFLAGS','MFLAGS')) {
        Remove-Item -LiteralPath ('Env:' + $variable) -ErrorAction SilentlyContinue
    }
    try { Verify-Compiler } catch {
        if ($script:Owned -and (Test-Path -LiteralPath $marker)) { Remove-Item -LiteralPath $marker -Force -ErrorAction SilentlyContinue }
        if (-not $script:Owned) { Say 'Existing external tools failed; rerun with /update to create a private toolchain.' Yellow }
        throw
    }
    if ($env:PP_ADD_PATH) { Add-UserPath }
    if (-not $env:PP_INSTALL_ONLY) {
        Phase 'Raylib configuration and game build'
        $script:BuildDir = Select-BuildDir
        Configure-Build
        Phase 'Build and asset verification'
        $game = Verify-Game
        Say "Game ready: $game" Green
        if ($env:PP_RUN) {
            Phase 'Launch'
            [void](Start-Process -FilePath $game -WorkingDirectory $script:BuildDir -PassThru)
            Say 'Game process started; gameplay itself is not tested by setup.' Green
        }
    } else { Say 'Dependencies installed and verified; game build intentionally skipped.' Green }
    Say 'SETUP COMPLETE' Green
    Say ('Elapsed: {0:hh\:mm\:ss}' -f $script:Clock.Elapsed)
    Say "Log: $script:Log"
    if (-not $env:PP_ADD_PATH) { Say 'PATH changes were session-only. Rerun setup.bat to build again, or use /add-path for build.bat.' }
} catch {
    $exitCode=1
    Say "SETUP FAILED during $script:Phase" Red
    Say $_.Exception.Message Red
    if ($_.ScriptStackTrace) { Say $_.ScriptStackTrace DarkGray }
    if ($script:Log) { Say "Diagnostic log: $script:Log" Yellow }
    Say 'No source files or previous build directories were deleted. Fix the reported issue and rerun setup.' Yellow
    Say 'First-time setup requires GitHub/MSYS2 network access, writable directories and permission to execute downloaded tools.' Yellow
} finally {
    if ($script:Lock) { $script:Lock.Dispose() }
    if ($script:ProjectLock) { $script:ProjectLock.Dispose() }
    $env:Path = $script:OriginalPath
}
exit $exitCode
