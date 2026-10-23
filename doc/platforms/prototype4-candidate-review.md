# OpenGate 0.0.2 prototype.4 candidate review

Target: separate Mac and Windows installer assets on the existing OpenGate
repository. Repository identity migration is deferred. Main and the accepted Mac
prototype.3 asset remain stable. This document is preparation, not a release or
live-acceptance claim. Exact final artifacts/checksums are pending final checks.

## Behavioral evidence and boundaries

The numbered native-entry contract is authoritative. Mac runtime source is frozen
at `0348946`; its 52 hidden suites pass with clean, unchanged source. Final serial
displayed suites passed 9/9; the earlier 7/9 failure is preserved. This checkpoint
is superseded for packaging by the requested Mac log-directory change and shared
startup handshake extraction, both requiring new verification. Windows `0348946` run `39897083542` passes all 44
invocations, including hidden and displayed production-DLL first-open flows,
outer-Inspector discovery, native HWND ownership, workflow wrappers, packaging
and support export. Later Keyframes/delayed-start/section-docking additions and
final artifact naming/export changes require their own runner result.

| Contract/action | Production entry and existing shared owner | Automated evidence | Live status |
| --- | --- | --- | --- |
| First open, built-in protection, Save/Update, unsaved restart | Mac ControlService / Windows UnboundStart → PanelControls → WorkspaceRestoration, WorkspaceSettings, ArrangementStore | first-open through Windows DLL; direct Mac first-open; preset policy and fresh-process restart | Windows pending |
| Title drag, vacancy reclaim, float, hide/recall | NativePanelTitleDrags, ManagedPanelSession, WorkspacePanelCallers, NativePanelToggles | first-open, panel-caller flow, retained-session and ownership fixtures | Actual Resolve widgets/input pending on Windows |
| Tab/group selection, individual/section docking and previews | PanelGroup, NativeArrangement, DockScopeTargets, DropPolicy | shared stack-docking assertions; displayed production first-open section variant failed; corrected DLL-boundary fixture awaiting runner | GPU/rendering and physical neutral release pending |
| Temporary maximize/restore and nested Mixer | PanelExpansionController and existing session restore | displayed first-open/Mixer route, exact host/geometry checks | Physical/remapped keys and native rendering pending |
| Keyframes, mounted/detached callers | Existing panel session, native placement control and visibility policy | Mac and Windows Keyframes flows pass | Windows Resolve Keyframes structure pending |
| Workspace Toolbar/Page Navigation, permanent Panels | Existing native leases, PinnedWorkspaceHeader, FooterPanel, WorkspaceHeaders; Windows title-strip adapter | band combinations, viewport bounds, native title owner and parked-slot rejection | Actual Windows permanent title-strip structure pending |
| Page return, Native return, temporary Native maximize, persistent Native gesture, re-entry | PageWorkspaceState, NativeWorkspaceBinding, restoration/ownership modules | assembled first-open flow through production Windows DLL and Mac fixture | Windows supported/unsupported page transitions need live run |
| Click/action/operation diagnostics and export | Shared InteractionDiagnostics, DiagnosticSink, CrashSession, request/results; Windows bootstrap logger/exporter | shared diagnostic fixture, DLL event trail, export allowlist/hash/no-mutation fixture | Tester supplies ZIP after actual session |

Synthetic fixtures cannot establish native GPU painting, playback, physical or
remapped keyboard correctness, or compatibility with an unseen Resolve widget
hierarchy. An observed signal is not evidence that a native operation completed.
The candidate must disclose these gaps; a successful build alone is insufficient.

## Jordan's focused Mac regression check

Use the normal installer; do not reset the current profile. Quit Resolve for
installation, then verify:

1. Last working arrangement and user presets return, including unsaved changes.
2. Drag a panel title, float it, hide it and recall it from Panels; confirm its
   host and location are retained. Select an inactive tab without relocating it.
3. Dock beside one panel and a whole section on each edge. Preview must match
   release; release in neutral space must leave the panel floating.
4. Hide each workspace band independently. Permanent Panels stays accessible,
   maximize respects the content area and Timeline keeps its toolbar.
5. Drag/maximize Mixer while nested in Timeline and after moving it elsewhere.
   Repeat hide/recall and placement controls for Keyframes.
6. Save a new preset, update it, then verify Resolve Layout cannot be overwritten.
   Restart; return from another page; verify the same working arrangement.
7. Use Original Resolve UI, temporarily maximize/restore a panel, then perform a
   persistent Native title gesture and re-enter OpenGate. Check content ownership,
   native painting and absence of duplicate titles/empty hosts.
8. Check ordinary-launch/remapped hotkeys, actual Timeline playback and rendering.
   Export support logs with a timestamp and steps for any regression.

Accepted revision 3 remains the rollback package; installation/uninstallation
preserves user arrangements. No installation is performed by this development run.

## Windows tester's equivalent live check

Record Windows version, display scaling/monitor arrangement, GPU and exact Resolve
edition/version. Save work and quit Resolve before double-clicking the Windows
installer. It requests standard elevation for shared Workflow Integration scripts.
Start Resolve normally. Automatic Windows IO-plugin startup is implemented but
awaiting runner and live verification; the recovery launcher remains in Start. Do not run a legacy Unbound helper alongside
this candidate in the same Resolve process.

Follow the same eight steps above, starting with automatic Resolve Layout on the
first managed launch. Also check:

- Windows caption/title dragging, resize borders, taskbar focus, pinning, Alt snap
  bypass and mixed-DPI/negative-coordinate monitor placement.
- The permanent project strip's Panels control with both workspace bands hidden.
- Native header buttons, flat Panels entries and Workflow Integration shortcuts
  produce the same panel state and highlight, with no repeated native toggling.
- Missing panels or titles: export logs immediately; WindowInventory/PageInventory
  records help adapt actual Windows structures without replicating native content.
- Upgrade/downgrade and uninstall must preserve the user profile. No fresh-user
  reset is required. Any separately requested fresh-profile test needs a verified,
  reversible backup first.

After reproducing a problem, choose **Start → OpenGate → Export OpenGate Support
Logs** and send the Desktop ZIP with the approximate time, page, panel, expected
result and actual result. Include screenshots for painting/geometry failures.
Logs identify OpenGate, module, build, session, sequence, input, observed signals
and operation outcomes. Export uploads nothing and leaves Resolve running.

## Candidate/release gate

Before publication: retain final passing Windows build/fixture/package/export
receipts, matching full Mac regression receipts, signed/notarized Mac package
verification, and downloadable installer hashes. Preserve every failed receipt.
Record packaged, installed, live accepted and published independently. No Windows
live acceptance is claimed until a tester completes the actual Resolve flow.
