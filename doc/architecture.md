# OpenGate source and platform boundaries

The accepted Mac revision 3 is the behavioral reference. Windows parity extends
that implementation through focused adapters. The numbered
[native-entry contract](platforms/opengate-native-entry-flow-acceptance.md) defines
user-visible behavior; the [evidence ledger](platforms/windows-managed-workspace-progress.md)
separates implementation, compilation, fixtures, packaging, installation and live
acceptance. Older documents describing Mac as unimplemented are historical.

## Shared workspace behavior

`src/core/panelcontrols.h` is the accepted workspace coordinator, mechanically
extracted from Mac. Mac compatibility headers retain the existing entry points.
`ManagedPanelSession`, `PanelGroup`, `NativeArrangement`, `NativeWorkspaceBinding`
and the layout-tree modules preserve real Resolve widgets, signals, native slots,
constraints and reversible ownership. There is one managed layout engine.

Panel callers, native header toggles, command dispatch, grouped visibility and
retained-host recall share the same session state. Workspace restoration,
snapshots, immutable Resolve Layout and user preset policy also live in core.
Settings paths are platform-specific; existing identifiers and schemas remain
compatible. No installer resets arrangements, shortcuts or native preferences.

Native toolbar/footer leases, independent band visibility, panel-title gestures,
dock scopes/previews, temporary expansion and native return reuse the accepted
modules. Unsupported pages retain native behavior. The older Windows Color and
Fairlight floating implementation is preserved for regression coverage, not
silently advertised as shared managed-page support.

Diagnostics reuse `InteractionDiagnostics`, `DiagnosticSink`, `CrashSession`,
workflow request/result logging and bounded read-only widget inventories. Events
carry OpenGate identity, build, module, process, session, sequence and UTC time.
An input or observed signal is not itself proof that the intended native operation
completed. Text-editor key values and contents are excluded.

## Platform adapters and bootstrap

`src/platform/nativewindowcontract.h` declares native chrome, pinning, movement,
visibility, window order and request-wake operations. Coordinate conversions and
OS input APIs belong under `src/platforms/<platform>/`. Resolve workflow/preset
paths, widget aliases, native spacing and modifier labels have explicit adapters;
Mac values are preserved rather than generalized into Windows assumptions.

On Mac, the existing IO/OFX loader reaches the widget probe and `ControlService`,
which initializes shared `PanelControls`. Cocoa handling remains in
`nativechrome.mm`. The existing universal build, signing, notarization and
transactional installer paths remain in use.

On Windows, the Qt-independent launcher waits for Resolve, loads its adjacent
`Unbound.dll` and invokes `UnboundStart`. `managedworkspace.cpp` preserves the
idempotent GUI-thread handoff and owns service lifetime, diagnostics and command
bridges. It creates the same shared `PanelControls` used by Mac.
`WorkspaceHostWindow` adapts the existing `PaneFloatingWindow` implementation;
Windows movement, resize, contact snapping, taskbar identity, native ownership
and physical/Qt logical coordinate conversion remain Windows code. Managed hosts
have native captions; the historical captionless mechanisms still have tests.

The Windows title-access adapter requires a recognized visible permanent project
strip. It rejects parked or ambiguous owners and records missing structure for a
tester. Inspector discovery prefers the common inner widget and recognizes the
older Windows outer frame without renaming native objects. Actual Windows Resolve
structure and GPU content remain live verification gates.

## Build, packaging and tests

Windows builds with VS2022, Qt 5.15.2 MSVC x64 and PowerShell. Root build wrappers
remain compatible. `infrastructure/windows/build-release.ps1` builds the shipped
DLL and test-only legacy helper, runs all independent fixtures serially, retains
a complete JSON receipt and blocks packaging if any assertion fails. Only the
production binaries, workflow scripts and provenance enter release output; the
legacy helper is never shipped. See [Windows development](platforms/windows.md).

The shared assembled first-open fixture runs directly and through the Windows
production DLL, including fresh-process persistence. Native Windows adapter
fixtures verify HWND ownership and window behavior. Equivalent checks remain
required for every new platform adapter; compilation is insufficient.

Mac's complete applicable matrix consists of hidden suites and serial displayed
suites. Displayed suites require Resolve closed. Every receipt records source
identity and cleanliness; failed runs are retained. Responsiveness is a separate
held gate, with no unmeasured performance claim. Candidate packages are built only
after matching regression evidence and do not establish live acceptance.

Each future cross-platform feature requires one behavioral specification,
deliberate shared-policy/platform boundaries, equivalent platform coverage, Mac
regression verification and explicit gaps/live-testing status. Extend this common
foundation rather than creating parallel controllers, layout engines or stores.

Wolfgang Woehl's inherited copyright, MIT license and applicable notices remain.
Public product labels say OpenGate; compatibility identifiers may remain Unbound.
Repository identity migration is a separate deferred task, not part of parity.
