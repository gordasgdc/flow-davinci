param(
    [string]$Date = (Get-Date -Format 'yyyy.MM.dd'),
    [int]$BuildNumber = 0,
    [string]$QtPath = "$env:USERPROFILE\Qt\5.15.2\msvc2019_64"
)
$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
$day = [datetime]::ParseExact($Date, 'yyyy.MM.dd', [Globalization.CultureInfo]::InvariantCulture)
if ($BuildNumber -lt 0 -or $BuildNumber -gt 65535) { throw 'BuildNumber must be 0 (automatic) or 1..65535.' }
if (!(Test-Path -LiteralPath (Join-Path $QtPath 'lib\cmake\Qt5\Qt5Config.cmake'))) { throw "Qt 5.15.2 development files not found at $QtPath" }
$releaseRoot = Join-Path $PSScriptRoot 'release\win'
New-Item -ItemType Directory -Force -Path $releaseRoot | Out-Null
$mutex = New-Object Threading.Mutex($false, 'Local\UnboundWindowsReleaseBuild')
$locked = $false
function Build-UnboundIcons {
    $Source = Join-Path $PSScriptRoot 'src\assets\filmhaus.png'
    $Destination = Join-Path $PSScriptRoot 'src\assets'
    Add-Type -AssemblyName System.Drawing
    New-Item -ItemType Directory -Force -Path $Destination | Out-Null
    $blue = [Drawing.ColorTranslator]::FromHtml('#00AEEF')
    $red = [Drawing.ColorTranslator]::FromHtml('#EF5350')
    $sourceImage = [Drawing.Bitmap]::new($Source)
    $master = [Drawing.Bitmap]::new($sourceImage.Width, $sourceImage.Height, [Drawing.Imaging.PixelFormat]::Format32bppArgb)
    try {
        if ($sourceImage.Width -ne $sourceImage.Height) { throw 'Expected a square source logo.' }
        for ($y=0; $y -lt $sourceImage.Height; $y++) {
            for ($x=0; $x -lt $sourceImage.Width; $x++) {
                $pixel = $sourceImage.GetPixel($x,$y)
                $fill = if ($pixel.B -gt $pixel.R) { $blue } else { $red }
                $master.SetPixel($x,$y,[Drawing.Color]::FromArgb($pixel.A,$fill.R,$fill.G,$fill.B))
            }
        }
        $master.Save((Join-Path $Destination 'filmhaus-resolve.png'),[Drawing.Imaging.ImageFormat]::Png)
        $sizes = @(16,20,24,32,40,48,64,96,128,256)
        $frames = @()
        foreach ($size in $sizes) {
            $bitmap = [Drawing.Bitmap]::new($size,$size,[Drawing.Imaging.PixelFormat]::Format32bppArgb)
            $graphics = [Drawing.Graphics]::FromImage($bitmap)
            $attributes = [Drawing.Imaging.ImageAttributes]::new()
            $stream = [IO.MemoryStream]::new()
            try {
                $graphics.Clear([Drawing.Color]::Transparent)
                $graphics.CompositingMode = [Drawing.Drawing2D.CompositingMode]::SourceCopy
                $graphics.CompositingQuality = [Drawing.Drawing2D.CompositingQuality]::HighQuality
                $graphics.InterpolationMode = [Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
                $graphics.PixelOffsetMode = [Drawing.Drawing2D.PixelOffsetMode]::HighQuality
                $attributes.SetWrapMode([Drawing.Drawing2D.WrapMode]::TileFlipXY)
                $graphics.DrawImage($master,[Drawing.Rectangle]::new(0,0,$size,$size),0,0,$master.Width,$master.Height,[Drawing.GraphicsUnit]::Pixel,$attributes)
                $bitmap.Save($stream,[Drawing.Imaging.ImageFormat]::Png)
                $bytes = $stream.ToArray()
                [IO.File]::WriteAllBytes((Join-Path $Destination "unbound-$size.png"),$bytes)
                $frames += ,$bytes
            } finally {
                $stream.Dispose(); $attributes.Dispose(); $graphics.Dispose(); $bitmap.Dispose()
            }
        }
        # Modern Windows ICO: PNG-compressed 32-bit RGBA frames, 256 encoded as 0.
        $output = [IO.File]::Create((Join-Path $Destination 'unbound.ico'))
        $writer = [IO.BinaryWriter]::new($output)
        try {
            $writer.Write([uint16]0); $writer.Write([uint16]1); $writer.Write([uint16]$sizes.Count)
            $offset = 6 + 16*$sizes.Count
            for ($i=0; $i -lt $sizes.Count; $i++) {
                $dimension = if ($sizes[$i] -eq 256) { 0 } else { $sizes[$i] }
                $writer.Write([byte]$dimension); $writer.Write([byte]$dimension)
                $writer.Write([byte]0); $writer.Write([byte]0)
                $writer.Write([uint16]1); $writer.Write([uint16]32)
                $writer.Write([uint32]$frames[$i].Length); $writer.Write([uint32]$offset)
                $offset += $frames[$i].Length
            }
            foreach ($frame in $frames) { $writer.Write([byte[]]$frame) }
        } finally { $writer.Dispose(); $output.Dispose() }
        Write-Host "Created recolored master, PNG variants, and unbound.ico in $Destination"
    } finally { $master.Dispose(); $sourceImage.Dispose() }
}
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
    Build-UnboundIcons
    $build = Join-Path $PSScriptRoot "build\unbound\$version"
    Invoke-Checked 'cmake' @('-S', (Join-Path $PSScriptRoot 'src'), '-B', $build,
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
    $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
    $vs = & $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
    if (!$vs) { throw 'Cannot locate Visual Studio tools for dependency audit.' }
    $toolset = Get-ChildItem -LiteralPath (Join-Path $vs 'VC\Tools\MSVC') -Directory | Sort-Object Name -Descending | Select-Object -First 1
    $dumpbin = Join-Path $toolset.FullName 'bin\Hostx64\x64\dumpbin.exe'
    $allowed = @('VERSION.dll','KERNEL32.dll','USER32.dll','SHELL32.dll','GDI32.dll','ole32.dll','dwmapi.dll','COMCTL32.dll',
        'Qt5Core.dll','Qt5Gui.dll','Qt5Widgets.dll','MSVCP140.dll','VCRUNTIME140.dll','VCRUNTIME140_1.dll')
    $dependencyReport = @{}
    foreach ($file in @($exe, $dll)) {
        $report = & $dumpbin /dependents $file
        if ($LASTEXITCODE -ne 0) { throw "Dependency audit failed: $file" }
        $dependencies = @($report | ForEach-Object {
            if ($_ -match '^\s+([\w.-]+\.dll)\s*$') { $Matches[1] }
        })
        if (!$dependencies.Count) { throw "No dependencies found by audit: $file" }
        foreach ($dependency in $dependencies) {
            if ($dependency -notin $allowed -and $dependency -notmatch '^api-ms-win-crt-[\w-]+\.dll$') {
                throw "Unexpected runtime dependency $dependency in $file"
            }
            if ($file -eq $exe -and $dependency -notin @('VERSION.dll','KERNEL32.dll','USER32.dll')) {
                throw "Launcher must depend only on Windows: $dependency"
            }
        }
        $dependencyReport[[IO.Path]::GetFileName($file)] = $dependencies
    }
    # Tests exercise the same shared pane code plus the standalone DLL bootstrap.
    $testBuild = Join-Path $PSScriptRoot 'build\pane-tests'
    Invoke-Checked 'cmake' @('-S', (Join-Path $PSScriptRoot 'tests\pane-float'), '-B', $testBuild,
        '-G', 'Visual Studio 17 2022', '-A', 'x64', "-DCMAKE_PREFIX_PATH=$QtPath")
    Invoke-Checked 'cmake' @('--build', $testBuild, '--config', 'Release', '--', '/m:1', '/nr:false')
    $env:PATH = "$QtPath\bin;$env:PATH"
    $env:QT_QPA_PLATFORM = 'offscreen'
    $test = Join-Path $testBuild 'Release\pane-float-smoke.exe'
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
    Invoke-Checked $test @('--discovery-test', $dll)
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
