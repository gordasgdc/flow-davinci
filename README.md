# Unbound

Unbound lets you undock DaVinci Resolve panes into independent, resizable windows
on Windows. Arrange them across your displays, snap them against Resolve's main
window or each other, and restore your workspace between sessions.

- Dock or undock with the overlapping-squares button beside a pane's menu.
- Floating window titles show the pane name alongside the Resolve icon.
- Snap window edges when moving or resizing, including display work-area edges.
  Hold **Alt** to bypass snapping.
- Unbound remembers window positions, sizes, docking and pane-toggle states.
- The floating Edit Inspector lets its native title label elide long filenames
  in the middle instead of using their full length as the window's minimum width.
- Unbound hides floating panes when leaving their page and restore them on return.
- Activating Resolve's main window or a floating pane brings the visible window
  group forward, keeping focus on the selected window. Hidden and minimized panes
  keep their state. This works with taskbar previews, Alt+Tab and direct clicks.

**Edit:** Media Pool, Effects, Sound Library, Keyframe Editor, Edit Index,
Inspector, Metadata and Mixer. **Color:** Nodes, Gallery, LUTs and Media Pool.
**Fairlight:** Media Pool, Effects, Index, Groups, Sound Library, ADR, Mixer,
Metadata and Inspector. Fairlight's dock button sits immediately left of the
native options menu (Create Group for Groups). Meters is not supported.

Floating Fairlight Media Pool audition retains audio, smooth waveform updates
and Control Room metering through an event-driven activation correction. The
event-only fix was validated on Resolve 21.1.1.10 / Windows 10 / Xeon
E5-2687W v4, including Resolve/Unbound restart. It uses public Qt
events without private memory access or polling. See the
[investigation findings](doc/fairlight-audition-investigation-2026-10-13.txt).

## Use

Keep `Unbound.exe` and `Unbound.dll` together and run Unbound as the same ordinary
Windows user as Resolve. Unbound waits silently if Resolve is not running yet.
Open a project, then use a pane's dock/undock button. Closing its floating window
also docks it. Resolve's native pane toggles remain available to control the space
in the main window. A pane docked on an inactive page is shown when you return.

Settings are stored in `%LOCALAPPDATA%\Unbound\Unbound.json`.
See [settings documentation](<doc/Unbound settings.md>). Restart Resolve to load an
updated Unbound DLL.

## Compatibility

Unbound is unofficial and depends on Resolve's internal Qt widgets and layouts.
The tested configuration is Resolve 21.1.0.14, Windows x64, Qt 5.15.2; other
versions are unverified. Save work before testing a new configuration.
Native minimum pane sizes apply. Nodes may need panning after docking into a
narrower area. Media Pool can widen during startup restoration and overlap an
adjacent pane. Color Clips and panes on other pages are not supported.

## Playback performance

Discovery caches panes and native selectors and handles widget creation/page
changes through queued events, without a recurring discovery timer. Playback
was reported stable on Windows 10 / Xeon E5-2687W v4 / Resolve 21.1.1.10 after
this change, including with an undocked pane.

Normal releases have no diagnostic switches. The separate diagnostic source,
build command and investigation notes are in [Windows diagnostics](tests/platforms/windows/diagnostic/README.md).

## Build

Requires Windows x64, Visual Studio 2022 C++ build tools and Windows SDK, CMake on
PATH, PowerShell, and **Qt 5.15.2 MSVC x64 development files** (headers, import
libraries and CMake configuration; Qt Creator is not required).

```powershell
.\build-unbound-release.cmd -QtPath C:\Qt\5.15.2\msvc2019_64
```

The default Qt path is `%USERPROFILE%\Qt\5.15.2\msvc2019_64`. The script regenerates
icons from `src/assets/filmhaus.png`, builds,
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
not-implemented message. This reorganization does not add macOS support.

## License

Unbound source: [MIT](LICENSE).
Filmhaus logo and derived icons: Copyright (c) 2026 Wolfgang Woehl.
See [NOTICE](NOTICE) for artwork and dependency notices.
