# Windows development

Supported build toolchain: Windows x64, Visual Studio 2022 C++ tools and Windows SDK,
CMake, PowerShell and Qt 5.15.2 MSVC x64 development files.

From the repository root, the existing entry point is unchanged:

```powershell
.\build-unbound-release.cmd -QtPath C:\Qt\5.15.2\msvc2019_64
```

`-Date` and `-BuildNumber` retain their existing meaning. The command configures the
root CMake project, builds under `build/windows/unbound/<version>/`, runs launcher
and pane fixtures, audits dependencies and publishes only `Unbound.exe` and
`Unbound.dll` under `release/win/<version>/`. Existing releases are never overwritten.
Pane fixture builds use `build/windows/pane-tests/`.

Source is in `src/platforms/windows/`; shared behavior is in `src/core/`.
Build/release implementation is in `infrastructure/windows/`. Both root release
wrappers remain usable from other working directories.

The Windows launcher still waits for Resolve, injects the adjacent DLL and exits.
The exported `UnboundStart` calling convention and return values are unchanged.
The export delegates to shared GUI-thread initialization. Native snapping, dark
captions, drag handling and placement coordinate semantics are preserved.

Window activation raises the main window and visible, non-minimized floating panes
as a group. The selected window keeps focus; peer order and geometry are preserved.
Modal dialogs and subsequent focus changes prevent a queued group raise. This is
event-driven and adds no polling. The native regression test is
`pane-float-smoke.exe --group-activation-native` with `QT_QPA_PLATFORM=windows`.

For development tools, see the [diagnostic build](../../tests/platforms/windows/diagnostic/README.md)
and [Inspector probe](../../tests/platforms/windows/inspector-probe/README.md).
The restructure requires validation with the Windows release command; macOS cannot
run the MSVC build or Win32 fixtures. Never stop the user's Resolve session to test.
