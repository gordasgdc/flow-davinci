# Windows development

Supported build toolchain: Windows x64, Visual Studio 2022 C++ tools and Windows SDK,
CMake, PowerShell and Qt 5.15.2 MSVC x64 development files.

From the repository root, the existing entry point is unchanged:

```powershell
.\build-unbound-release.cmd
```

The public semantic version comes from the root `VERSION` file; override it with
`-Version 0.2.0-dev.1`. `-QtPath` overrides the default
`%USERPROFILE%\Qt\5.15.2\msvc2019_64`. The command configures the
root CMake project, builds under `build/windows/unbound/<version>/`, runs launcher,
shared policy, assembled workspace, production-DLL and native window fixtures,
and audits dependencies. It writes the production binaries, generated workflow
scripts and provenance under immutable `release/win/<version>/` directories.
A failing fixture blocks packaging; every independent result is retained under
`build/windows/fixtures/fixture-receipt.json`. Existing releases are never overwritten.
Pane fixture builds use `build/windows/pane-tests/`.

EXE and DLL file/product version strings contain the complete semantic version.
Their numeric Windows resources map `MAJOR.MINOR.PATCH` to `MAJOR,MINOR,PATCH,0`;
`-BuildNumber` overrides only that final numeric revision (0..65535). Each public
numeric component must also fit 0..65535. UTC build IDs remain in resource comments
and `release-manifest.json`, alongside the version, revision, dependencies and hashes.
Diagnostics use the same numeric version with a diagnostic suffix in the strings.

The current prototype installer is `infrastructure/windows/prototype-installer.iss`.
The GitHub Windows workflow supplies `BuildVersion=0.0.2` and `BuildRevision=4` to
Inno Setup and produces a separate Windows x64 executable plus SHA-256 checksums.
It never publishes a GitHub release. Standard administrator approval installs
OpenGate runtime files and only OpenGate workflow scripts in Resolve's documented
ProgramData workflow directory. It refuses installation while Resolve is running.
Settings, presets and shortcuts remain in the user's profile across uninstall.
The older ZIP packager remains available for historical two-binary builds; it is
not the managed-workspace prototype distribution path.

Start-menu support export produces a Desktop ZIP without stopping Resolve or
modifying settings. It includes rotated events, launcher failures, bounded native
structure inventories, session/incident records, provenance and checksums. It also
records whether installed workflow targets exist. No automatic upload occurs.

Source is in `src/platforms/windows/`; shared behavior is in `src/core/`.
Build/release implementation is in `infrastructure/windows/`. Both root release
wrappers remain usable from other working directories.

The Windows launcher still waits for Resolve, injects the adjacent DLL and exits.
The exported `UnboundStart` calling convention and return values are unchanged.
The export delegates to the Windows managed service, which creates shared
PanelControls on Resolve's GUI thread. `WorkspaceHostWindow` reuses the existing
native window mechanisms with a normal caption for managed panel names.
The original captionless pane implementation remains in an unshipped legacy
fixture DLL; its frame and movement assertions are preserved. Its details follow
for maintainers of those reused Windows mechanisms.

Windows 11 uses DWM's visible border thickness without subtracting a pixel;
side and bottom compensation stops at client content. Resolve's custom client
rectangle includes the DWM top outline, so the top compensation uses DWM's
thickness even when ClientToScreen reports no top margin. Snapping to the
work-area top places that outline outside the work area, alongside floating panes.
The main window commits its negotiated top snap after the native move loop ends
if release clamps it by the outline thickness; cancellation, maximization and
Alt bypass do not replay the snap. Captionless pane restoration preserves valid
saved client rectangles instead of Qt's generic title-bar safety margin.
Windows 10 retains its one-physical-pixel translucent-outline compensation.
Dragging their Dock button uses the nonmodal movement path; an ordinary click
still docks. Native snapping and placement coordinate semantics are preserved.
Plain title labels and unused native header background use that same movement
path after the drag threshold. Header controls retain immediate native input;
editable/selectable text and custom control widgets are excluded. Header event
filters are scoped to recognized header subtrees; application-wide filtering
exists only for an active mouse gesture, including Escape and page-hide cleanup.

Window activation raises the main window and visible, non-minimized floating panes
as a group. The selected window keeps focus; peer order and geometry are preserved.
Modal dialogs and subsequent focus changes prevent a queued group raise. This is
event-driven and adds no polling. The native regression test is
`pane-float-smoke.exe --group-activation-native` with `QT_QPA_PLATFORM=windows`.

For development tools, see the [diagnostic build](../../tests/platforms/windows/diagnostic/README.md)
and [Inspector probe](../../tests/platforms/windows/inspector-probe/README.md).
