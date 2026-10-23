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

## Checkpoint receipts and next ownership boundary

`25fe2645d039c8d2c683a2b08dc2b87489ee7380` has clean receipts in this worktree:
`25fe2645d0-hidden-20261022T193031Z` (49/49) and
`25fe2645d0-displayed-20261022T193554Z` (9/9). These include the two new
shared policy/diagnostic suites and the accepted assembled Mac flow.
[Windows run 39840425886](https://github.com/jordantetewsky/OpenGate/actions/runs/39840425886)
passed compilation, existing native-window suites, shared caller/layout/logging
fixtures, packaging and export checks. Its source manifest reported dirty status;
the build log was initially written into the repository root. No clean-source
candidate claim follows. Logs are retained locally under `build/windows/evidence`.

Dependent worktree `windows-retained-host-calls`, branch
`feature/windows-retained-host-calls`, isolates the next milestone from that
unchanged-source matrix. It is incorporated into this feature branch by fast
forward; main is untouched. Mac compatibility includes now also cover the same
pointer sampler and native caller policy. The Windows input-age adapter deliberately
omits uncalibrated ages: Qt 5.15.2's Windows mouse handler uses its elapsed process
clock, while Cocoa supplies uptime timestamps. No latency improvement is claimed.

Windows run [39842442083](https://github.com/jordantetewsky/OpenGate/actions/runs/39842442083)
compiled but failed the new production-bootstrap fixture at discovery: its fake
tree omitted the existing Windows `frameEditPageWidgetsContainer` root.
Production lookup and assertions were preserved; the fixture now models that root.
Failed logs are retained. The corrected run
[39843068499](https://github.com/jordantetewsky/OpenGate/actions/runs/39843068499)
passes bootstrap -> real layout lease -> native header hide -> hidden page return
-> flat Panels recall in the same host/frame/owner -> live signal -> page return
-> explicit dock/exact layout ownership. This is Windows Qt/DLL fixture evidence,
not live Resolve or complete managed-workspace parity. Its 49 event diagnostic
trail, settings and build log are retained locally. Installer/export also passed.
The manifest still reports dirty source; subsequent runs record exact status and
keep dependency installer logging outside the checkout rather than hiding it.

That new caller route is development-gated by `OPENGATE_WINDOWS_MANAGED_CALLERS=1`
until shared managed startup/ownership is integrated. It is temporary staging,
not a second supported product mode or completion of the task. The existing
default Windows floating-backend suites remain unchanged and continue to run.
The target is the accepted Mac experience using the same shared engine/session
policy, with Windows native adapters. The gate must not become a permanent fork.

At `d01130e`, all 49 Mac hidden suites passed, but the receipt correctly reports
`source_unchanged=false`: the Windows-only fixture correction was committed during
the run. Do not report it as a clean matrix. Production Mac source was identical
at `1591016`; the latter has a clean serial displayed 9/9 receipt in the dependent
worktree, `1591016182-displayed-20261022T195324Z`. Clean final-source matrices remain
required after further shared changes. No failed receipt was overwritten.

Next extraction reuses `NativePanelPosition`, `NativeGroupSlot`, `NativeRootLayout`,
`VacatedContents`, `GroupVisibility` and logical tree visibility. The Windows
adapter adds no Cocoa top overlap to Qt page coordinates; native layout margins
remain authoritative. The new common ownership fixture checks repeated exact
box/grid/split/stack recovery, constraints and a Timeline with its toolbar still
attached. The Windows retained-session path reuses the accepted vacancy guard,
rather than inventing another placeholder suppression mechanism. Full managed
assembly, tabs/groups/docking, primary bands, preset startup and the remaining
numbered flow are still incomplete. No Mac candidate has been built or installed.

The separate requested final identity/migration/revision-4 task is recorded in
`opengate-identity-migration-task.md`, including stargazer notification. Migration
execution must not interrupt this parity work; cutover requires concrete approval.

## Shared managed-session and coordinator checkpoint (2026-10-22)

The release target is `v0.0.2-prototype.4` in the existing repository. Standalone
repository migration is deferred; it must not delay parity implementation.

`676a631` passed Windows runner [39888491564](https://github.com/jordantetewsky/OpenGate/actions/runs/39888491564):
shared managed sessions ran both offscreen and with the Windows Qt platform;
shared caller, logging, tree, ownership and preset policy fixtures passed;
retained DLL-bootstrap callers, existing Windows windowing checks, installer
construction and non-mutating support export passed. Downloaded evidence is in
`build/windows/evidence/39888491564`. The manifest records clean source and still
truthfully identifies the production backend as `windows-floating-pane`, parity
false and live acceptance false. These are development artifacts, not candidates.
The earlier missing-include compile failure at `6750b42` is preserved separately.

The accepted `ManagedPanelSession`, floating host, panel group, native mounting,
placement and dock-scope modules now live in core with Mac compatibility includes.
Windows uses the existing `PaneFloatingWindow` mechanisms through a focused
window adapter: actual HWND ownership, native movement/resize, system-menu pin,
clipping and explicitly distinguished physical/native and Qt logical coordinates.
Native input age is unknown, never fabricated. No playback/performance claim.
`22fe282` passed Windows [39889519984](https://github.com/jordantetewsky/OpenGate/actions/runs/39889519984),
adding the existing Mac section-docking assertions without weakening them.

A frozen Mac hidden matrix at `22fe282` is being retained in the sessions
worktree. Its first-install fixture failed before assertions because it read the
Mac compatibility include instead of the moved core recipe (regex returned None).
`da040ad` corrects that single source path; all recipe, immutable seed and
no-overwrite assertions remain intact and the individual fixture passes. The
failed receipt is not replaced or reported as passed.

The next mechanical extraction (`07fb04c`) shares the existing native workspace
binding, restoration ordering, snapshot schema and arrangement controls. No new
layout engine, persistence format or controller is introduced. Legacy Mac session
names remain aliases; common implementations use ManagedPanelSession. The Mac
restoration fixture passes, including preflight, rollback boundaries and stale
callback cancellation. Equivalent Windows execution is pending the next runner.

Production Windows managed startup/discovery, permanent Panels/title integration,
complete bands/page/Native transitions and the assembled numbered flow remain
open. Shared components passing is not evidence those production paths are wired.
No new Mac package, installation, live acceptance, release publication or migration
cutover occurred. Revision 3's accepted Mac asset remains unchanged.

## Shared controller and assembled-flow audit (2026-10-22, continued)

The accepted PanelControls implementation and only its required dependency
closure are now shared. Mac compatibility headers preserve existing entry points;
there is no second managed layout/preset/panel controller. Native Resolve paths
and window operations remain platform adapters. Shared request/result routing
also reuses the existing WorkflowCommandBridge. The shared workflow generator
produces all 30 Mac scripts and its manifest byte-for-byte identically to the
previous generator; Windows changes only its directory and native wake primitive.
Both generated-script fixtures pass locally, including atomic requests, no-ctypes
fallback and selected-clip API completion without recursive requests.

Windows runs `39889984665` and `39890856228` failed the unchanged restoration
message/order assertion with MSVC's implicit source charset. Windows `/utf-8`
compilation fixed the actual Unicode conversion mismatch. Run
[39891163351](https://github.com/jordantetewsky/OpenGate/actions/runs/39891163351)
passes restoration, pinned header and complete navigation assertions, shared
session/tree/ownership/preset fixtures, existing windowing/DLL fixtures, packaging
and support export. Full downloaded receipts are retained in
`build/windows/evidence/39891163351`; failures remain under their own run IDs.

Run `39892392253` compiled most shared assembled fixtures but failed on fixture
portability: an implicit Mac-only activation helper, transitive visibility include
and old session alias. Test activation now has a Windows native adapter; required
includes are explicit and common session types replace aliases without changing
assertions. Run `39893148184` then compiled the full shared controller but failed
Native entry's cramped toolbar overflow assertion. This is an open production/
fixture investigation, not a parity pass. Full workflow and build logs are kept
in `build/windows/evidence/39893148184`. New runs retain every assembled fixture's
settings and event trail under `build/windows/fixtures`. Native stderr is retained
and executable exit codes remain mandatory; a dedicated Windows runner fixture
checks success, failure code 23, missing programs and paths containing spaces.
The next orchestration records every independent suite, then fails packaging if
any fixture failed. No assertion/threshold has been relaxed.

Frozen Mac `594dc80` hidden receipt `594dc803cc-hidden-20261023T024230Z` and frozen
`82017ba` receipt `82017ba934-hidden-20261023T025908Z` each pass all 51 suites with
clean/unchanged source. Resolve reopened, so the first displayed request was
correctly NOT RUN. Jordan closed it again. Displayed matrix
`82017ba934-displayed-20261023T031806Z` passes 7/9: panel sessions and hidden controls
failed drag/release conditions. The session log shows requested/cursor mismatch
and source deactivation; causation is not established. Both subsequently pass in
isolated logs beside the failed receipt. Those passes do not replace the failed
matrix. The fresh serial matrix at frozen `23caf36`, receipt
`23caf36877-displayed-20261023T032515Z`, passes all 9 suites with clean, unchanged
source. Resolve may reopen. The corresponding 51-suite hidden matrix is running.

Windows run `39894669562` stopped after the native-command runner's assertions
all passed: its deliberately failing child left LASTEXITCODE nonzero, which the
GitHub PowerShell wrapper propagated. The fixture now exits zero only after all
assertions pass; thrown assertions still fail. No production behavior or assertion
changed. This failed workflow remains recorded separately.

Windows permanent Panels access currently accepts a uniquely recognized visible
native project-title strip with its own actual box layout. Parked/native-toolbar
slots are rejected and logged; no guessed overlay or empty replacement row is
created. Actual Windows Resolve title-strip structure remains unverified and
requires tester WindowInventory evidence. The Windows startup service is prepared
but not yet selected by the packaged helper. The production backend remains the
older backend until shared assembled tests and a production DLL flow are wired
and verified. Do not report component/fixture coverage as production parity.

The existing read-only PagePanelInventory now also supports an explicit main
window scope for Windows discovery diagnostics; default Mac scope is unchanged.
Only names, classes, hierarchy, Qt geometry/constraints and method metadata are
read, never project/control text or custom property getters. Main-window capture
is bounded and separately stored. Windows support export allowlists these files;
new coverage preserves all previous export assertions and checks unrelated JSON
is excluded. The individual Mac privacy/ownership inventory fixture passes.

No candidate installation, Mac package replacement, live Windows acceptance,
publication, main merge or repository migration has occurred.

## Production startup verification (2026-10-22, continued)

`23caf36` completed the clean/unchanged 51-suite hidden matrix in
`23caf36877-hidden-20261023T033524Z` and the 9-suite serial displayed matrix above.
The preceding sandbox-limited hidden receipt `23caf36877-hidden-20261023T033255Z`
is preserved: window-service/pasteboard access failed and Qt reported no screens.
It is not a behavioral pass. Resolve is free to run during subsequent hidden work.

Windows `923b5f4` run `39895642757` completed all 40 fixture invocations: 39 passed;
only cramped toolbar overflow failed. Its retained event record reports a 32px
slot, 26px available and hidden controls. The Mac-specific 12px title gutter was
subtracting more than Windows' 6px native layout spacing. `4fb8bdd` adapts Windows
to actual native spacing; Mac remains exactly 12px and keeps its Option label.
The existing 28px minimum and 32px fixture assertion are unchanged. The focused
Mac native-entry regression passes. Windows snapping is labeled Alt.

`c832ee6` switches the packaged Windows DLL to the accepted shared PanelControls;
the Windows service contains only bootstrap/lifetime, diagnostics and command
integration. There is no second managed controller or a production legacy-mode
switch. The original controller remains in an unshipped test-only DLL to preserve
all historical Windows mechanism assertions. The same complete first-open
fixture also loads the actual shipped DLL through UnboundStart, checks repeated
bootstrap, Native return and fresh-process restart. Run `39896147572` passes this
production-DLL flow and 39 other invocations; only the same toolbar overflow
assertion fails. Evidence for both runs is retained under their run IDs.

The Windows adapter now measures an overlapping native project strip in Qt
logical coordinates. The OS caption remains excluded. Existing native-owner,
background hide/recovery, hidden-host and title-access checks pass on Windows.
An additional production flow models the older Windows outer Inspector frame
without renaming native widgets; canonical layout identities remain shared.
These synthetic structures do not verify the actual Windows Resolve hierarchy.

The existing single-use Mac manager-launch request policy was extracted in
`0cb69c4`, separately from Windows wiring. Windows command generation reuses the
accepted menu entry with only a directory adaptation. Generated Mac command
behavior remains unchanged. A new shared filesystem-wake fixture preserves stale
startup rejection, recent explicit launch, duplicate token rejection and reuse.
Its sandbox run missed watcher events; the same unchanged fixture passes with
filesystem notification access. The full Mac service regression is still pending
for this latest extraction. Latest full matrices must precede candidate readiness.

Windows installer integration now includes 30 command scripts plus OpenGate's
manager menu entry in Resolve's documented ProgramData workflow root. This needs
standard installer elevation; no installer has run on Jordan's machine. Settings,
native preferences and projects are excluded from installation/uninstallation.
Latest adapter/command/installer changes are implemented but not yet runner-tested
or packaged. Publication and live acceptance remain pending.

## 2026-10-23: Mac displayed focus evidence (before final packaging)

The d90358e hidden matrix passes 52/52. Two complete serial displayed runs each
passed 8/9; neither is a passing matrix. Preserve receipts
`d90358ebe7-displayed-20261023T043725Z` (panel-sessions failure) and
`d90358ebe7-displayed-20261023T044902Z` (Mixer failure), with all suite logs.
The original isolated panel-sessions also failed background-key preservation;
a diagnostic-only rerun passed unchanged assertions. Its cause is unestablished.
The first diagnostic build failed a local variable-name collision; its failed
compile log is retained along with the corrected run. No thresholds changed.

The second matrix's retained Mixer events explicitly show command sequence 716
rejected with `reason: no-active-window`, followed by sequence 717 accepted with
`active_window: UiMainWindow` and successful Mixer expansion. Its fixture used
Qt-only activation before processing queued Cocoa events. The focus setup now
uses the existing native test activation helper already used by the assembled
docking flow, and adds an active-workspace precondition plus requested/actual
pointer diagnostics. All original dispatch, maximize and exact ownership/return
assertions remain. This is a fixture setup correction, with no production change.
Retained failure directory: `opengate-opengate-first-open-tests-rHNgkH` under the
local temporary root. A corrected full serial matrix is still required.

## 2026-10-23: explicit startup parity and log branding corrections

Jordan requires installation followed by ordinary Resolve launch on Windows,
with no per-session OpenGate launch. The earlier manual-launch notes are a gap,
not accepted parity. Native IO plugin adaptation is in progress; no automatic
Windows launch is claimed yet. Installed SDK documents Win64 bundles under
ProgramData/Blackmagic Design/DaVinci Resolve/Support/IOPlugins and describes
Studio support; free-edition support remains unverified.

Mac log output and command transport move together to Library/Logs/OpenGate.
Older Unbound logs remain intact, support export reads both, settings paths stay
unchanged. Export privacy assertions and both platform workflow-wrapper tests
pass after this change. Prior signed Mac artifact 0348946 is preserved but is
not the final candidate for this request; fresh full verification/build needed.

Windows a59ddba run 39898066892 compiled, passed 45/46 fixture invocations, and
failed the added displayed stack-drop variant. Packaging/export stages were
skipped, not passed. All evidence retained under build/windows/evidence/39898066892.
The test queried nativeDockScopes from its EXE, while registration occurred in
the production DLL's separate inline-static registry. Correct the fixture to
measure actual rendered cells and route mouse events through the DLL event filter,
asserting its preview and actual released target. Retain the 16-pixel breadth
threshold, full-height preview, exact subtree/ownership assertions. This correction
requires a new runner result; it is not a pass.

Mac 0348946 full results: 52/52 hidden, final serial 9/9 displayed, clean unchanged
source. Earlier serial 7/9 failure remains preserved; cause unestablished. Signed,
notarized package SHA256 6ab744b2d6ab11a78f62d261c80d2a07fd4b4feca611ce61e0f1aefe495a9eba.
No candidate installed, live accepted, merged or published. Resolve can remain
open for editing while implementation/hidden checks run; notify Jordan before
any new displayed matrix.

## 2026-10-23: startup adapter implemented; new Mac hidden matrix passes

- Shared IO handshake extracted mechanically in 0770701 from the accepted Mac
  bootstrap; platform callbacks retain their own scheduling/loading mechanics.
- Mac log-directory change d90358e: 52/52 hidden suites pass with clean unchanged
  source, receipt `prototype4-mac-verification/build/macos/verification/
  d90358ebe7-hidden-20261023T042356Z/receipt.json`. Displayed checks pending Jordan
  closing Resolve. Package input/signing/transaction fixtures also pass.
- Windows 3bfb18f adds a Qt-independent IO bootstrap, installed at the SDK's
  documented Win64 bundle path. It reuses Inno's existing InstallLocation record,
  existing Windows readiness/version checks and UnboundStart GUI-thread handoff.
  No login service, background application or competing workspace controller.
  Startup diagnostics carry module OpenGate.dvcp in the existing event envelope.
- Minimal Windows IO ABI checked against the locally installed Resolve SDK: all
  used constants, packed context size/alignment/member offset and property names.
  The shared Mac wrapper compiles against that SDK. No SDK codecs are distributed.
- Run 39900270149 is pending. It adds the real IO plugin entry to the assembled
  displayed flow, including repeated initialization, retained ownership and stack
  docking. Its fixture creates/removes a disposable Inno record and refuses to
  overwrite an existing installation. Actual installer execution and Windows
  Resolve/Studio startup remain unverified. No fixture result implies live acceptance.
- Public Windows messages/resource product labels now say OpenGate. Compatibility
  DLL names and inherited Wolfgang Woehl copyright remain unchanged.
- Jordan has requested publication of v4 and an eventual uninstall/fresh-user
  tutorial setup. Do not alter the installed revision 3 or isolate settings while
  Jordan edits. First finish checks and publish the verified download page; notify
  him before uninstall. Ordinary uninstall preserves profiles; a tutorial profile
  isolation requires a verified reversible backup, with native Resolve preferences
  and projects left alone.
