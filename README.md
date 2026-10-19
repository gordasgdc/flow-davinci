# OpenGate — by CineWaltz

OpenGate is the CineWaltz macOS panel/workspace fork, built on **Unbound by Wolfgang Woehl**. Wolfgang created the foundation; his MIT copyright and license remain in [LICENSE](LICENSE). OpenGate is a separate project and should not be confused with the original Unbound. Historical Windows instructions and internal compatibility identifiers below retain the original name.

See the explicit [OpenGate next-pass acceptance list](doc/platforms/opengate-next-pass.md), including **restoring the last view when Resolve reopens**.

## Current integration work — October 18, 2026

`macos-panel-discovery-and-layout` combines separate feature branches for ongoing
work. It is **not a completed or newly installed release**. See the
[integration ledger](doc/platforms/macos-integration-ledger.md) for each feature's
implementation and verification status. Build manifests identify the exact
included feature commits; they do not certify acceptance. The preserved installed
snapshot is described below. Master and upstream have not been merged.

The current candidate starts with Panel Manager closed and the native workspace
visible on a fresh configuration. Nested tab-door selection and reversible narrow
Inspector scrolling have fixture coverage. Original-window rearrangement, live
Resolve acceptance and the responsiveness target remain unfinished.

## Preserved experimental macOS snapshot — October 17, 2026

This branch, `macos-responsive-group-transfers`, is an experimental macOS build
shared for ongoing development; it has not been merged into main. Arrangement
and docking flow still need refinement, which Jordan plans to address in the next
pass. A reported clipping issue can let Timeline content overlap Inspector in a
combined window; that issue is not fixed in this snapshot.

The previous `macos-tethered-timeline-and-viewers` build contains useful experiments
and remains available for comparison and rollback. Jordan reports that it feels
less snappy than this iteration. That is user feedback, not a completed performance
validation: this branch's responsiveness target and full live regression checklist
remain open. See the [current development status](doc/platforms/macos-next-pass-status.md)
and [verification checklist](doc/platforms/macos-workspace-checklist.md).

Unbound lets you undock DaVinci Resolve panels into independent, resizable windows.
Arrange them across your displays and restore your custom workspace between sessions.
Unbound remembers window positions, sizes, docking and panel-toggle states.

For now we have builds for Windows 10/11. No MacOS and Linux builds yet.

## How to use

Download from [Releases](../../releases), unzip and run
`Unbound.exe`. Or start Resolve and _then_ run `Unbound.exe`. Unbound waits
silently if Resolve is not running yet. This will add a Dock/Undock button
to supported panels.

Open a project, then use a panel's Dock/Undock button. Resolve's native panel
toggles remain available to control the space in the main window.

Native minimum panel sizes apply. Nodes may need panning after docking into a
narrower area.

Settings are stored in `%LOCALAPPDATA%\Unbound\Unbound.json`.
See [settings documentation](<doc/Unbound settings.md>).

## Compatibility

Unbound is unofficial and depends on Resolve's internal widget hierarchy and layouts.
The oldest tested configuration is Resolve 21.1.0.14 on Windows x64 (10 and 11);
other tested versions include 21.1.1.10. Save work before testing a new configuration.

Windows may nag about the Unbound helper when running it. That's a good thing - OS
should notice and say something when DLLs attach to applications. That said: Unbound is
minimally intrusive - does not do any harm - does not phone home. If unsure: let your
agent check the open source.

## Build

Requires Windows x64, Visual Studio 2022 C++ build tools and Windows SDK, CMake on
PATH, PowerShell, and **Qt 5.15.2 MSVC x64 development files** (headers, import
libraries and CMake configuration).

```powershell
.\build-unbound-release.cmd
```

The script regenerates icons from `src/assets/unbound-symbol.svg`, builds,
runs regression tests and audits imports, then puts the two release binaries in
`release/win/<version>/`. The default semantic version comes from `VERSION`
(currently `0.1.0`); `-Version 0.2.0-dev.1` overrides it. Existing releases are
never overwritten. Use `-QtPath` to override the default
`%USERPROFILE%\Qt\5.15.2\msvc2019_64`. Windows build output is in `build/windows/`.
The root command delegates to `infrastructure/windows/`.

Packaging is a separate, explicit step after committing and tagging the release:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File infrastructure/windows/package-release.ps1
```

This creates `release/packages/Unbound-0.1.0-windows-x64.zip` with a single enclosing
folder, ready to attach to GitHub release `v0.1.0`.

## Source layout

- `src/core/`: shared Qt pane behavior, settings and snapping calculations.
- `src/platform/`: contracts used by shared code.
- `src/platforms/windows/`: the Windows launcher, bootstrap, native window backend
  and resources.
- `src/platforms/macos/`: experimental Mac helper, panel sessions and launcher;
  the full release backend is unfinished.
- `infrastructure/<platform>/`: platform build, audit and packaging workflows.
- `tests/core/`: shared fixtures; `tests/platforms/<platform>/`: native adapters,
  launcher tests and diagnostic tools.

See [architecture](doc/architecture.md), [Windows development](doc/platforms/windows.md)
and [macOS development status](doc/platforms/macos.md). The root CMake project
selects one backend; macOS configuration currently stops with an explicit
not-implemented message.

## License

Unbound source and symbol artwork: [MIT](LICENSE).
