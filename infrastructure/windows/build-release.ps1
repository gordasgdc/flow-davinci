param(
    [string]$Date = (Get-Date -Format 'yyyy.MM.dd'),
    [int]$BuildNumber = 0,
    [string]$QtPath = "$env:USERPROFILE\Qt\5.15.2\msvc2019_64"
)
$ErrorActionPreference = 'Stop'
$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
Set-StrictMode -Version Latest
$day = [datetime]::ParseExact($Date, 'yyyy.MM.dd', [Globalization.CultureInfo]::InvariantCulture)
if ($BuildNumber -lt 0 -or $BuildNumber -gt 65535) { throw 'BuildNumber must be 0 (automatic) or 1..65535.' }
if (!(Test-Path -LiteralPath (Join-Path $QtPath 'lib\cmake\Qt5\Qt5Config.cmake'))) { throw "Qt 5.15.2 development files not found at $QtPath" }
$releaseRoot = Join-Path $repoRoot 'release\win'
New-Item -ItemType Directory -Force -Path $releaseRoot | Out-Null
$mutex = New-Object Threading.Mutex($false, 'Local\UnboundWindowsReleaseBuild')
$locked = $false
. (Join-Path $PSScriptRoot 'generate-icons.ps1')
. (Join-Path $PSScriptRoot 'audit-dependencies.ps1')
function Invoke-Checked([string]$Program, [string[]]$Arguments) {
    & $Program @Arguments
    if ($LASTEXITCODE -ne 0) { throw "$Program failed with exit code $LASTEXITCODE" }
}
try {
    try { $locked = $mutex.WaitOne(0) } catch [Threading.AbandonedMutexException] { $locked = $true }
    if (!$locked) { throw 'Another Unbound release build is running.' }
    if ($BuildNumber -eq 0) {
        $maxBuild = 0
        foreach ($folder in Get-ChildItem -LiteralPath $releaseRoot -Directory) {
            if ($folder.Name -match ('^' + [regex]::Escape($Date) + '\.(\d+)$')) {
                $maxBuild = [Math]::Max($maxBuild, [int]$Matches[1])
            }
        }
        $BuildNumber = $maxBuild + 1
    }
    if ($BuildNumber -gt 65535) { throw 'Daily build number exceeds Windows version limits.' }
    $version = '{0}.{1:D3}' -f $Date, $BuildNumber
    $destination = Join-Path $releaseRoot $version
    if (Test-Path -LiteralPath $destination) { throw "Release already exists: $destination. Existing releases are never overwritten." }
    Build-UnboundIcons -repoRoot $repoRoot
    $build = Join-Path $repoRoot "build\windows\unbound\$version"
    Invoke-Checked 'cmake' @('-S', $repoRoot, '-B', $build,
        '-G', 'Visual Studio 17 2022', '-A', 'x64', "-DCMAKE_PREFIX_PATH=$QtPath",
        "-DUNBOUND_VERSION=$version", "-DUNBOUND_YEAR=$($day.Year)", "-DUNBOUND_MONTH=$($day.Month)",
        "-DUNBOUND_DAY=$($day.Day)", "-DUNBOUND_BUILD=$BuildNumber")
    Invoke-Checked 'cmake' @('--build', $build, '--config', 'Release', '--', '/m:1', '/nr:false')
    Invoke-Checked (Join-Path $build 'Release\UnboundLauncherTests.exe') @()
    $exe = Join-Path $build 'Release\Unbound.exe'
    $dll = Join-Path $build 'Release\Unbound.dll'
    foreach ($file in @($exe, $dll)) {
        if ((Get-Item -LiteralPath $file).VersionInfo.ProductVersion -ne $version) { throw "Version resource mismatch: $file" }
    }
    $dependencyReport = Get-UnboundDependencies -exe $exe -dll $dll
    # Tests exercise the same shared pane code plus the standalone DLL bootstrap.
    $testBuild = Join-Path $repoRoot 'build\windows\pane-tests'
    Invoke-Checked 'cmake' @('-S', (Join-Path $repoRoot 'tests\core\pane-float'), '-B', $testBuild,
        '-G', 'Visual Studio 17 2022', '-A', 'x64', "-DCMAKE_PREFIX_PATH=$QtPath")
    Invoke-Checked 'cmake' @('--build', $testBuild, '--config', 'Release', '--', '/m:1', '/nr:false')
    $env:PATH = "$QtPath\bin;$env:PATH"
    $env:QT_QPA_PLATFORM = 'offscreen'
    $test = Join-Path $testBuild 'Release\pane-float-smoke.exe'
    Invoke-Checked (Join-Path $testBuild 'Release\audition-activation-test.exe') @()
    Invoke-Checked $test @()
    Invoke-Checked $test @('--main-placement-test')
    Invoke-Checked $test @('--settings-json-test')
    Invoke-Checked $test @('--header-button-test')
    Invoke-Checked $test @('--inspector-title-test')
    Invoke-Checked $test @('--nodes-canvas-test')
    Invoke-Checked $test @('--shutdown-test')
    Invoke-Checked $test @('--close-all-test')
    Invoke-Checked $test @('--standalone-test', $dll)
    Invoke-Checked $test @('--standalone-test', '--visible-start', $dll)
    Invoke-Checked $test @('--standalone-test', '--toggle-state-test', $dll)
    Invoke-Checked $test @('--standalone-test', '--color-selected-test', $dll)
    Invoke-Checked $test @('--reclaim-test', $dll)
    Invoke-Checked $test @('--fairlight-test', $dll)
    Invoke-Checked $test @('--fairlight-test', '--restore', $dll)
    Invoke-Checked $test @('--discovery-test', $dll)
    $env:QT_QPA_PLATFORM = 'windows'
    try {
        Invoke-Checked $test @('--audition-taskbar-native')
        Invoke-Checked $test @('--snap-native')
        Invoke-Checked $test @('--captionless-native')
        Invoke-Checked $test @('--frame-paint-native')
    }
    finally { $env:QT_QPA_PLATFORM = 'offscreen' }
    # Only these two runtime files are distributed; build records stay in build/.
    New-Item -ItemType Directory -Path $destination | Out-Null
    Copy-Item -LiteralPath $exe, $dll -Destination $destination
    $hashes = @(Get-FileHash -LiteralPath (Join-Path $destination 'Unbound.exe'), (Join-Path $destination 'Unbound.dll'))
    @{ Version=$version; BuiltAt=(Get-Date).ToString('o'); Qt=$QtPath; Files=$hashes; Dependencies=$dependencyReport } |
        ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $build 'release-manifest.json')
    Write-Host "Unbound $version ready: $destination"
} finally {
    if ($locked) { $mutex.ReleaseMutex() }
    $mutex.Dispose()
}
