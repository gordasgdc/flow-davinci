# Unbound for DaVinci Resolve

Unbound lets you undock DaVinci Resolve panels into independent, resizable windows
on Windows. Arrange them across your displays and restore your custom workspace
between sessions. Unbound remembers window positions, sizes, docking and
panel-toggle states.

## Use

Keep `Unbound.exe` and `Unbound.dll` together and run `Unbound.exe`. Unbound waits
silently if Resolve is not running yet.
Open a project, then use a panel's dock/undock button. Resolve's native panel
toggles remain available to control the space in the main window.

Settings are stored in `%LOCALAPPDATA%\Unbound\Unbound.json`.
See [settings documentation](<doc/Unbound settings.md>).

## Compatibility

Unbound is unofficial and depends on Resolve's internal Qt widgets and layouts.
The tested configuration is Resolve 21.1.0.14, Windows x64, Qt 5.15.2; other
versions are unverified. Save work before testing a new configuration.
Native minimum panel sizes apply. Nodes may need panning after docking into a
narrower area.

## Build

Requires Windows x64, Visual Studio 2022 C++ build tools and Windows SDK, CMake on
PATH, PowerShell, and **Qt 5.15.2 MSVC x64 development files** (headers, import
libraries and CMake configuration; Qt Creator is not required).

```powershell
.\build-unbound-release.cmd -QtPath C:\%USERPROFILE%\Qt\5.15.2\msvc2019_64
```

The script regenerates icons from `src/assets/unbound-symbol.svg`, builds,
runs regression tests and audits imports, then puts the two release binaries in
`release/win/YYYY.MM.DD.NNN/`. Use `-Date YYYY.MM.DD -BuildNumber N` for an explicit
version. Windows build output is in `build/windows/`; published release paths
remain unchanged. The root command delegates to `infrastructure/windows/`.

## Source layout

- `src/core/`: shared Qt pane behavior, settings and snapping calculations.
- `src/platform/`: contracts used by shared code.
- `src/platforms/windows/`: the Windows launcher, bootstrap, native window backend
  and resources.
- `src/platforms/macos/`: reserved for the macOS backend; not implemented yet.
- `infrastructure/<platform>/`: platform build, audit and packaging workflows.
- `tests/core/`: shared fixtures; `tests/platforms/<platform>/`: native adapters,
  launcher tests and diagnostic tools.

See [architecture](doc/architecture.md), [Windows development](doc/platforms/windows.md)
and [macOS development status](doc/platforms/macos.md). The root CMake project
selects one backend; macOS configuration currently stops with an explicit
not-implemented message.

## License

Unbound source and symbol artwork: [MIT](LICENSE).
See [NOTICE](NOTICE) for artwork and dependency notices.
