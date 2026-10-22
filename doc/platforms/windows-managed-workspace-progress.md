# Windows managed workspace: implementation and evidence

Baseline: accepted main `28ff7d635d1ab5c0bddc6e18c4b9524e466ca481`.
Worktree: `windows-managed-workspace`; branch `feature/windows-managed-workspace`.
Push only to `personal` (Jordan's OpenGate); `origin` is Wolfgang's Unbound.
No accepted Mac installation, settings or release asset is changed by this work.

## Baseline evidence (2026-10-22)

Main was clean at the accepted commit. `git diff 88988d4 75e8217 -- src`
and `git diff 75e8217 28ff7d6 -- src` were empty.
The actual reference receipts are in the `stack-dock-scopes` worktree:

- `build/macos/verification/75e8217205-hidden-20261022T172058Z/receipt.json`: 47 passes, clean source.
- `build/macos/verification/75e8217205-displayed-20261022T173004Z/receipt.json`: 9 passes, clean source.
- [Windows run 39809501592](https://github.com/jordantetewsky/OpenGate/actions/runs/39809501592): queried through GitHub, success at `e3caabf7761cf39830b489c708af3f8fe98cabb5`; older backend, not parity evidence.

Read-only installed evidence: package receipt `com.cinewaltz.opengate.prototype`
version 0.0.2 exists. Current installed release manifest records runtime
`88988d479d22de0ca311a2d97ef193886a3bc010`, packaging `75e8217`, universal
Mac revision 3. This verifies provenance, not an independent live acceptance run.
Resolve initially ran as PID 43278; Jordan then closed it and `pgrep -x Resolve`
returned 1. No agent termination or installation occurred.

Older architecture documents describing Mac as unimplemented are historical.
Current Mac production entry is the IO/OFX bootstrap -> widget probe ->
`ControlService` -> `PanelControls`. Windows entry remains launcher ->
`UnboundStart` -> core `Controller`. Similar names do not imply equivalent flows.

## Parity map

Paths below are relative to `src/`; Mac module names refer to `platforms/macos`
unless marked core. The numbered native-entry document remains authoritative.

| Contract | Mac entry and working modules | Windows/current gap | Reuse boundary and verification |
| --- | --- | --- | --- |
| Automatic first open; unsaved restart | `PanelControls`, `resolvelayout`, `workspacerestoration`, `workspacesettings`; core `PageWorkspaceState` | `Controller::discover` restores individual floating flags only | Reuse managed startup/restoration; adapt discovered Windows content and readiness. First-open/restart assembled fixture on both platforms plus live Resolve. |
| Immutable Resolve Layout, writable user presets | `ArrangementStore`, `arrangementcontrols`; core `WorkspacePresetPolicy` | `PaneSettings` has no managed preset route | Reuse store/policy, adapt settings directory only; do not overwrite legacy files. Save/Update/rename/delete protection and downgrade round trip. |
| Native return and exact ownership | `MacPanelSession`, `NativePanelPosition`, `NativeRootLayout`, `NativeArrangement` | `SidebarFloatSession::restore` restores transferred layout, no managed tree | Extend existing lease/session boundaries; preserve original splitter/grid/stack positions and constraints. Exact-owner assertions after assembled Native return. |
| Native temporary maximize; persistent title entry | `panelexpansioncontroller`, `nativepaneltitledrags`, `nativepaneldrag` | Native Windows host resize/drag works; no managed entry transaction | Reuse mode/gesture policy and temporary expansion; adapt native pointer and window coordinates. No preset reset; cancellation/page-hide assertions and physical-input live check. |
| Permanent Panels; independent bands | `workspaceheaders`, `pinnedworkspaceheader`, `surfacepagenavigation`, `workspaceheadermigration` | No primary managed workspace or bands | Reuse real native band leases, adapt Windows main title strip, not a second toolbar UI. Four band combinations and viewport maximize checks. |
| Header/menu/hotkey synchronization | `nativepaneltoggles`, `workspacecallers`, `panelcommands`; core `WorkspacePanelCallers` | Floating decoration state and native toggles; no complete shared caller integration | Route all entry points to one retained-session visibility state. Menu/header/hotkey equivalent transitions including stale queued page requests. |
| Hide/recall same host/location | `MacPanelSession::setHidden`, `groupvisibility`, `panellayoutvisibility` | Page-hide retention exists; close currently docks and destroys host | Explicit hide differs from close/dock. Keep host and placement on hide; verify object identity and geometry, not only visible flags. |
| Inactive tabs; grouped hide/show | `GroupVisibility`, `PanelGroup`, `NativeArrangement` | No grouped/tabbed session API | Reuse layout surfaces and logical visibility; adapt actual sessions to transfer content, never infer visibility from selected tabs alone. Nested selection and group-state tests. |
| Split/tab tree and titles | `panellayoutsurface`, `grouplayout`, `panellayouttitles`, `panellayoutsnapshot` | No managed tree | These existing implementations now live in core with Mac compatibility includes. Shared policy fixture runs on each OS; Mac assembled regression still required. |
| Individual/whole section docking, four edges, neutral release | `dockscopetargets`, `droppolicy`, `nativedocksurfaces`, `docksurfaces` | Windows snapping is window contact, not panel docking | Shared target/tree/preview policy extracted unchanged; keep OS contact snapping separate. Actual release callback and stale-target ownership tests on both platforms. |
| Drag, float, maximize/restore | `floatinghost`, `nativepaneldrag`, `panelexpansion` | `PaneFloatingWindow` handles native nonmodal movement/borders/shell identity | Windows QWidget host can render shared surfaces, but current session exposes no group donation/mounting. Extend session API with ownership tests before switching production startup; preserve Windows frame coordinates. |
| Mixer/Keyframes nested/mounted; Timeline toolbar | `panelsession`, `timelinecontrol`, `panelaccessorylease`, `timelinetoolbarviewport`, `footerpanel` | Windows special Mixer full-layout transfer, no player/Timeline managed inventory | Reuse native accessory/lease rules; Windows discovery must establish actual content roots and toolbar relation. Same nested Mixer/Keyframes sequence and narrow layout, plus GPU live acceptance. |
| Supported page state, unsupported native | `workspacepages`, `editpagelifecycle`; core `PageWorkspaceState` | Edit/Color/Fairlight floating restoration, different scope from Mac | Keep explicit adapter support; never transplant Edit IDs to another page. Repeated return/restart and unsupported-page Native invariants. |
| Click/action/event diagnostics | `InteractionDiagnostics`, `ViewerDiagnostics`, `DiagnosticSink`, `CrashSession` | Previously no equivalent production event pipeline | Existing implementation extracted to core; OS directory/module/window identity adapters. Same synthetic interaction trail on Mac/Windows; exporter fixture and launcher failure trail. |

## Decisions and Windows mechanism audit

Mechanical extraction and behavioral integration are separate commits.
The Qt tree renderer owns containers, not Resolve content; it already validates
leaves before mutation and is reusable without a second engine. JSON tree keys,
preset identities and visibility semantics remain unchanged. Compatibility headers
retain existing Mac include paths and class/function names.

`SidebarFloatSession` preserves full layouts and Windows `PaneFloatingWindow`
preserves native nonmodal dragging, resize borders, shell identity and placement.
They are useful mechanisms, but are insufficient unchanged: no group donation,
embedded native root transaction, covered-tab state, or persistent workspace bands.
Close currently means dock. This must not be mistaken for retained-panel hide.
The next runtime boundary must reconcile the existing Mac session/group lifecycle
with these mechanisms, rather than adding another controller or changing the
experience to fit floating windows. No bulk Mac backend copy is proposed.

The launcher cannot use Qt before injection, so its small Windows-only logger
uses the same diagnostic envelope and a separate `launcher.jsonl`. The helper
uses the shared Mac writer/observer, rotation, immediate operation flushes,
button/action connections, viewer signal discovery, and typed-input exclusion.
Native crash-file discovery on Windows is explicitly pending; an unclean marker
does not establish a crash or OpenGate causation.

## Evidence ledger

| Scope | Implemented | Compiled | Fixture tested | Packaged | Installed | Live accepted | Published |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Accepted Mac baseline | yes | reference | 47 hidden + 9 displayed reference | revision 3 reference | observed provenance | Jordan accepted | existing revision 3 |
| Shared layout extraction | yes | local Mac | focused shared layout and stack suites; full matrix pending | no | no | no | no |
| Shared diagnostics + Windows integration/export | in progress | local shared fixture | local diagnostic trail passed; Windows pending | no | no | no | no |
| Windows assembled managed workspace | incomplete | pending | pending | no | no | no | no |

Initial tool runs were not successes: stack-docking compiled but aborted because
the sandbox exposed no screens; instrumentation compiled but refused because
the sandbox could not query processes. Shared diagnostic fixture first failed
its typing assertion because its hidden window lacked a focused receiver.
The fixture now explicitly sets/checks the active window and editor focus;
no production assertion or threshold was weakened. Its rerun passes. Tool
transcripts retain these failures; final matrix logs/receipts are append-only.

## Live checklists and candidate gates

For Mac, after the full unchanged-source hidden and serial displayed matrices:
first open -> title drag/vacancy reclaim -> float/hide/recall -> nested tab/group
-> all four individual/section edges and neutral release -> maximize/restore ->
each band combination -> immutable Update protection and writable Save as New ->
unsaved restart -> page return -> Native exact content return -> temporary Native
maximize -> persistent Native drag -> OpenGate re-entry. Include nested Mixer,
Keyframes, narrow Timeline/toolbar and remapped keys. Check native GPU painting.

Windows requires the same sequence on its real platform fixture and then a
Windows Resolve tester. Jordan has no Windows machine; tester recruitment is
pending. GitHub Windows builds/fixtures can proceed independently. Export logs
after each problem via Start > OpenGate > Export OpenGate Support Logs; record
UTC time, page, panel, expected/observed result and screenshots for rendering.
No candidate readiness, release replacement or install is implied by this branch.

## Future feature rule

Each cross-platform feature requires one user-visible behavioral specification,
deliberate shared-policy/platform-adapter boundaries, equivalent platform coverage,
Mac regression checks, explicit remaining platform gaps and separate live status.
Build success on two platforms is never cross-platform completion. Extend this
common foundation; retain Wolfgang Woehl's attribution and MIT notices.
