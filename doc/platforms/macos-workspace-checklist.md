# Mac workspace checklist

Working branch: `macos-docking-compass-and-controls`.
Preserved prior iteration: `macos-docking-tabs-and-maximize` at `38d0616`,
installed executable `21466f2`. See [controls/docking follow-up](macos-controls-docking-followup.md)
for current evidence and acceptance checks. Installed follow-up: **cc2c233**.
Preserved previous branch: `macos-docking-layouts-v2` at `4a3f13f`.
Installed checkpoint: `f64c401`; archived binaries and dSYM in
`build/macos/checkpoints/f64c401/`. Do not overwrite that checkpoint.

## Next-pass acceptance contract

Track implementation, fixture result and live acceptance separately. In particular,
input accessibility is a regression gate, not docking polish.

- [ ] T1: No screen-wide docking suggestion. Only the eligible destination under
  the pointer offers compact, fixed-position targets after deliberate approach.
  Free movement over ordinary content or empty desktop shows no docking overlay.
  A marker does not chase the pointer along every edge. Covered windows and
  hidden sibling group cells cannot compete. Option clears all targets.
- [ ] T2: Distinct center/tab, inner edge/panel split and outer edge/group split
  targets; one stable preview predicts the exact committed result. Release away
  from actual targets floats freely. Preserve native window drag responsiveness.
- [ ] T3: Group-to-group and panel-to-group placement in either orientation,
  nested split plus tab stacks, header tear-off and outer-title group movement.
- [ ] T4: Backtick/tilde maximizes the hovered panel on its current display's
  usable area, regardless of docked/floating/split/tab placement. Second press
  restores that same panel's exact location, visibility, tab selection, split
  proportions and geometry even if pointer moved. Yield to typing and dialogs.
- [ ] T5: Resolve native fullscreen viewer command and assigned shortcut work
  for detached/grouped/tabbed Source and Program. This is image fullscreen, not
  Unbound panel maximize. Preserve native command semantics and restore layout.
- [ ] T6: Native visibility toggles/shortcuts (including user's Q Inspector
  mapping) share state with Unbound in standalone, split and tab placements.
  Hide collapses space but remembers slot/proportions; show restores; a covered
  tab is selected first. Apply to every recognized panel, not Inspector alone.
- [ ] T7: Optional visible pin control for the floating host/group; no keyboard
  focus theft, dialogs and menus remain reachable. No permanent global top tier.
- [ ] T8: Arrangements persist split/tab/hidden/pin relationships and survive
  restore/restart; old presets migrate without overwriting named originals.
- [ ] T9: Buttons (Stabilize, Dock All), Timeline audio/video divider, editing
  shortcuts, page changes and Workflow Integrations remain accessible after
  repeated moves/merges/tear-offs/maximize. New diagnostics identify targets,
  committed drops, visibility and maximize/restore outcomes.
- [ ] T10: GUI regression tests only while Resolve is closed and after warning
  about brief test windows; installed-build handoff lists concrete live checks.

Preserved first-pass branch: `macos-arrangements-and-docking` at 1c7d9a4;
installed executable build d3ed034. Do not overwrite that checkpoint.
Preserved reliability branch: `macos-workspace-reliability` at `c5cf2e4`.
Preserved baseline: `macos-panels-workspaces` at `9ba3fb7`.
Reliability changes are implemented and pass automated fixtures. Fresh-process
Resolve acceptance checks remain pending; installed helper is d3ed034.
Last confirmed Ripple Delete failure was in c5cf2e4; the d3ed034 page-scope
candidate still needs explicit live confirmation. First-pass groups move together,
but user reports stuck drag release, pair-only limits, background/Z issues and
multicam behavior differences.

## Completion rule

An item needs a reproduction or measurable requirement, appropriate regression
coverage, and recorded evidence. Resolve behavior also needs a live check in a
fresh process with the recorded helper build. Compilation and synthetic fixtures
alone do not establish that a reported runtime bug is fixed. Leave live checks
pending until performed. Preserve the user's editing session during testing.

For every item, record the commit/build, test command and result, live scenario
and result, and useful log timestamps. Record built and installed identities
separately. Do not commit project media or private diagnostic captures.

## First: lifecycle, focus and restoration

- [ ] R1: Fix Program going completely blank when Source is active. Compare both
  docked, floating with hierarchy off, and floating with hierarchy on. Both viewers
  retain their own correct image and controls during focus changes, scrubbing,
  pause and playback. Diagnose activation, visibility and rendering separately.
- [ ] R2: Clicking Timeline gives its editing widget keyboard focus. The user's
  B mapping executes Ripple Delete exactly once on the intended selection.
  Compare docked/floating and hierarchy off/on; check native key window, focused
  widget and shortcut receiver. Text fields, dialogs and other panels retain their
  own input. Do not assume the user's mapping matches Resolve defaults.
- [ ] R3: Cycle Edit to Color, Fairlight and other pages and back ten times.
  Timeline returns with saved geometry, usable content and shortcuts. Deliberately
  hidden panels stay hidden. No Dock All, controls relaunch or restart is required.
  Also test project closure/reopening and replacement widget instances.
- [ ] R4: Dock All restores every panel's original parent, layout slot, native
  visibility and usable content after the event loop settles. Cover mixed,
  hidden/minimized, page-switched and restored arrangements, native toggle changes
  and pending bulk commands. Repeated calls are safe; failures identify the panel
  and preserve a recoverable state. R3 must pass independently of this reset.
- [ ] R5: Investigate Resolve's native fullscreen command after viewer detachment.
  Compare Source and Program against the docked baseline. Record a verified fix
  or specific remaining limitation; our maximize toggle does not prove this fixed.

## Temporary maximize and compact controls

- [ ] M1: Backtick expands the panel under the pointer, falling back to the last
  active panel. A second press restores its exact floating geometry or docked
  slot/group relationship without disturbing the arrangement. Provide explicit
  Source/Program actions. Test page changes, display changes, window styles and
  closing while expanded.
- [ ] M2: Backtick yields to text entry and dialogs and fires once. Check keyboard
  layout and existing mappings; resolve conflicts before claiming shortcut support.
  Log target selection, focus and restore result.
- [ ] U1: Unbound can become narrow and short, with a scrolling panel list and
  essential preset/recovery actions kept accessible. Verify mouse/trackpad and
  keyboard access, saved control geometry, pinning and Workflow relaunch.

## Named arrangements

- [ ] P1: Named app-wide presets support Save As, Update, Restore, Rename and
  Delete. Moving a window never silently overwrites a preset. Distinguish current
  unsaved changes from the selected preset.
- [ ] P2: Preserve panel identity, floating/docked state, visibility, geometry,
  window style, options, group relationships and split ratios. Test missing displays,
  negative coordinates, invalid files, changed minimum sizes, restart and project
  reload. Preserve existing last-arrangement settings and report unavailable panels.

## Size and scale

- [ ] E1: Add Dock/Undock to positively identified native panel ellipsis menus,
  using the same operations and shared state as Unbound controls. No duplicate
  actions after reopening menus or switching pages. Timeline gets an accessible
  header control where there is no suitable ellipsis. Verify native menu actions
  and shortcuts remain intact; do not insert into unrelated menus.
- [ ] E2: Prototype independent interface scaling on an ordinary panel and a
  viewer. If viable, expose the same per-panel Interface Scale slider/reset in
  native menus and Unbound. Persist values in arrangements. Verify text sharpness,
  GPU surfaces, menus, input coordinates, Retina and playback; do not substitute
  window resizing or image zoom for UI scaling.

- [ ] S1: Identify Timeline's minimum-size constraints and relax only verified
  unnecessary limits. Narrow/short layouts retain usable tools, hit targets,
  scrolling and playback. Redocking restores original constraints.
- [ ] S2: Investigate per-panel UI scaling separately from desktop window size,
  timeline zoom and viewer image zoom/quality. Evaluate GPU rendering, Retina,
  input mapping and playback before promising a resolution slider. Agree on the
  slider's meaning and prove feasibility before implementation.

## Magnetic layout

- [ ] D1: Edge snapping previews display/panel alignment, with Option bypass.
  Verify thresholds, corners, multiple displays and minimum sizes without jitter.
- [ ] D2: Dock left/right or above/below. Source and Program support horizontal
  and vertical groups with adjustable split ratios. Preview attachment; move groups
  together and detach by dragging away. Keep snapping and group docking distinct.
- [ ] D3: Shared-edge resizing respects minimum sizes. Save and restore group
  relationships. Test hide/show, viewport recovery, display removal, page changes,
  temporary maximize, dissolution and Dock All. Detached-window hierarchy remains
  correct; group ordering must preserve focus and native dialogs.
- [ ] D4: Premiere-like group interaction: attached panels move as one unit with
  no visible trailing or delayed follower windows. Prefer one native host with
  split layouts, subject to actual Resolve rendering/focus validation. Preview
  attachment and detachment, preserve pointer-to-panel position when tearing off,
  and resize shared edges in one layout transaction. Verify interactive dragging
  during playback on Retina/mixed displays. Edge alignment alone does not pass.

## Cross-cutting verification

- [ ] N1: Left release completes every move, resize, attachment and tear-off.
  Cancellation, application deactivation, Escape and lost capture clear previews
  and grabs. Right-click must never be needed to release a drag. Capture physical
  event ordering in diagnostics; synthetic left-release alone missed this report.
- [ ] N2: Use a persistent split tree for arbitrary panels and group-to-group
  merging. Detach one leaf while preserving the other leaves, order and ratios;
  collapse empty/unary splitters. Restore existing two-panel saved arrangements.
- [ ] N3: Separate distant edge-alignment guides, contact snapping and intentional
  docking. Small local drop targets appear only near the current eligible target;
  show its exact proposed split. Pull away to leave guides; Option bypasses.
- [ ] N4: Workflow Integration/Relay activation cannot raise the empty original
  Resolve window over the detached arrangement. Preserve Relay text focus, native
  modal dialogs and intentional page navigation. Groups must not permanently win
  z-order just because they include a viewer. Check actual app switching.
- [ ] N5: Offer hiding/minimizing only the empty main background independently of
  floating panels, plus explicit recovery of that background. Verify native owned
  window behavior first; do not minimize the whole workspace inadvertently.
- [ ] N6: Center individual panels/groups on the current display, plus bulk rescue.
  Test disconnected displays, negative coordinates and minimized/hidden windows.
- [ ] N7: Multicam playback/angle selection/switching works docked, floating and
  grouped, with correct source/timeline focus and program output. Screenshots and
  user reports establish a regression; generic fixtures cannot close it.
- [ ] N8: Investigate detaching the native Stabilization Inspector section as a
  toggleable quick-access panel. Preserve clip selection, settings and reversible
  ownership. Identify native Stabilize dispatch and whether Resolve exposes it to
  Keyboard Customization; do not claim shortcut registration from reparenting.

- [ ] C1: Log launches, actions/results, window arrangements, focus/shortcut
  routing, page transitions, restoration phases, presets and docking operations
  with build/process/time identity. Verify diagnostics explain failures without
  introducing playback polling or excessive pointer-movement work.
- [ ] C2: Preserve regressions for all eleven panels, source-only detachment,
  ownership, visibility, title styles, hierarchy/key-window focus, viewport recovery,
  Workflow relaunch and Quick Export. Deliveries identify installed versus tested
  builds and distinguish live-verified behavior from pending or unsupported work.

## Evidence ledger

Update this ledger as work proceeds. Check an item only after its required
regressions and live acceptance checks pass.

| Item | Commit/build | Automated evidence | Live Resolve evidence | Status |
| --- | --- | --- | --- | --- |
| R1–R5 | Baseline 9ba3fb7 | Existing fixtures do not reproduce these reports | User reports: blank Program, failed B/Ripple Delete, lost Timeline after page changes, unreliable Dock All, unavailable fullscreen | Open |
| R1–R4 | Working reliability changes | `bash infrastructure/macos/test-reliability.sh`: PASS. Reproduced 17 failures before changes; now covers ten native page moves, deferred docking, replaced layouts, unrelated ownership rejection, viewer visibility and native shortcut scope/text entry. | Pending fresh-process checks, including actual B/Ripple Delete and both viewer images | Implemented; live verification pending |
| C1–C2 | Working reliability changes | `bash infrastructure/macos/test-panel-sessions.sh`: PASS for eleven panels, persistence, bulk cancellation, visibility, viewport recovery, styles, hierarchy/focus, Quick Export and relaunch. Added event-driven focus, shortcut, native action and page recovery diagnostics; text entry excludes key values. | Pending | Automated regression pass |
| R2 | Installed c5cf2e4 | Owned-window shortcut fixture passes, but does not reproduce actual Ripple Delete failure | User confirms Ripple Delete still fails with Timeline undocked | Open; requires native command/focus diagnosis |
| R2 diagnostic | c5cf2e4, pid 46314 | — | 2026-10-16T13:58:23.710Z: B reaches floating Timeline with m_TiTimelineView (UiTiSequenceView) focused; no corresponding shortcut/action observed. Ctrl+Z subsequently produces input.shortcut. Focus alone does not explain failure. | Native command routing investigation |
| U1 | Continuation branch | Scroll area added for options and eleven-panel list; status and bulk recovery buttons remain outside it. Regression suite passed before adding explicit compact-size assertions. | Pending | In progress |
| P1–P2 | Continuation branch | Named preset creation, duplicate-name rejection, rename, independent duplicate/update, deletion, reload and invalid-file preservation pass. Control restore fixture restores Source only with saved size; current changes do not update saved presets. Pair-group layout is included in the same workspace/preset data. | Pending; UI scale remains unimplemented | Implemented for current panel/options model |
| E1 | Working changes after f85a49b | Native menu fixture passes shared Dock/Undock state, original item preservation, repeated-open deduplication and unrelated menu exclusion. Only positively attached options menus are modified. | Pending real menu ownership verification; Timeline header control not implemented | Partial |
| M1–M2 | Working changes after f85a49b | Explicit Expand/Restore fixtures pass original docked slot, floating geometry, close-to-dock and exclusion of temporary geometry from persistence. | Pending; backtick already produces a native shortcut event in c5cf2e4 logs and needs conflict investigation before interception | Partial; keyboard mapping not implemented |
| D1 | Working changes | Frame alignment, negative display coordinates, threshold rejection, preview/release and Option bypass fixtures pass. Uses the Unbound grip, including a new grip beneath standard title bars. Snap preference persists in settings/presets. | Pending | Partial: native title-bar drag does not preview attachments |
| D2–D4 | Working single-host pair groups | `test-panel-groups.sh` passes both orientations for direct/content-layout panels, synchronous group motion, shared split restoration, page visibility, recall, pointer-anchored tear-off, group close and original-slot docking. Panel-controls suite passes union preview, drop-to-group, saved group reload and Release Groups. Earlier prototype emitted Cocoa KVO warnings; revised layout transfer passes without window recreation. | Pending GPU playback/rendering, keyboard scope, physical drag feel, mixed displays and refresh rates | Partial: two-member groups; no nested groups, add-third, continuous tear-off/reattach or group-aware maximize |
| R2 candidate | Working activation bridge | Reliability fixture passes existing Windows audition bridge pattern: application receives one page activation notification; synthetic event is consumed before Qt widget delivery. Native key window, Qt active host and text focus stay unchanged; inactive Edit is skipped. | Actual Ripple Delete still requires new build and live confirmation | Candidate only; not claimed fixed |
| E2/S2 | Investigation | Bundled Qt 5.15.2 source, qgraphicsproxywidget.cpp lines 582–584 excludes WA_PaintOnScreen/OpenGL widgets from embedding; its class documentation cautions against high-performance use. A transformed proxy is therefore not a proven universal panel-scale solution. | Need native/GPU surface inspection and actual-panel prototype | Open; no scale slider shipped |
| Candidate build | d3ed034 | test-panel-sessions.sh, test-panel-groups.sh, test-reliability.sh: PASS. build-widget-probe.sh and build-io-helper.sh: PASS, including native bootstrap fixture. Qt-helper SHA-256: 897ebca8b060d0dd75224c7f178e413f82366773594c12e531cfa4c3f99b24e5. | Not installed: Resolve pid 46314 still running. Installed helper SHA-256 remains 20d1592b8dce740b931e32263427e44cf909b587ec05b241e1f06a46c149bfc6 (c5cf2e4). | Built; waiting for closed Resolve and live checks |
| N1 | v2 working changes | Receiver-independent release, missing-button state, Escape and native Cocoa mouse-up monitor/removal fixtures pass. Tear-off reuses the same capture/release handler. | Right-click workaround reported in d3ed034; physical release must be retested | Candidate fix |
| N2 | v2 working changes | Merge pairs into four members; insert fifth beside a leaf; detach a transferred member via its header; dock one while preserving the other four; nested tree restore and old pair reload pass. Controls fixture covers group-to-group drag, off-screen group centering, saved reload and Dock All. Test caught a mutating JSON lookup creating a null leaf; corrected before delivery. | Pending | Implemented; live validation pending |
| N3 | v2 working changes | Local 28-pixel target, half-split preview, no center target, distant alignment guides and Option bypass fixtures pass. Native title-bar attachment remains absent. | Pending physical discoverability and target accuracy | Implemented for grip dragging |
| N4–N5 | v2 working changes | Native tool hide-on-deactivate disabled at normal level. Independent background hide/reveal keeps Qt ownership and native floating visibility. Workflow-window fixture retains Z above workspace, native key window and text entry. Mixed groups follow activation within ordinary group tier. | Actual Relay/external Electron switching pending | Candidate fix |
| N6 | v2 working changes | Per-panel Center recalls hidden off-screen panels; group Center retains the nested layout and fits the current viewport. | Pending mixed-display/disconnect checks | Implemented |
| N7–N8 | Investigation | Multicam needs actual media/native playback evidence. Startup inventory does not identify the lazy Inspector Stabilization section reliably. Added read-only visible-control ancestry/action diagnostics; no native Stabilize dispatch/shortcut invented. | Pending | Open |
| Saved user preset | Read-only inspection during v2 work | Arrangements.json contains “Jordan Arrangmement” with two groups. Workspace.json currently has zero groups. Tests use isolated temporary files and do not modify either user file. | Restart/preset restoration still needs live confirmation | Saved data present |

## Install handoff

Every install must report build identity, changes actually included, exact user
test steps, and known unfinished behavior. Never present fixtures as live proof.
Do not ask the user to test features absent from that build.

Latest install: d3ed034; installed after confirming Resolve was closed. Bootstrap
and Qt-helper SHA-256 match the built artifacts. Fresh-process live checks remain
pending. The following paragraph describes the earlier c5cf2e4 test scope; use
the Next candidate checks below for d3ed034.
Test: click Timeline and invoke the user's B/Ripple Delete mapping; compare docked
and floating. Switch Edit to Color/Fairlight and back; check Timeline returns.
Alternate Source and Program; check both image and controls. Try Dock All after
page changes. Magnetic docking, presets, maximize and independent UI scale are
not included. Ripple Delete is already reported failing and remains open.

## Current work order

1. Build the pair-group/activation candidate and install when Resolve is closed.
2. Validate R1–R5 and D1–D4 in a fresh process; keep test results separate from
   actual editing/playback evidence. No checklist box is closed by fixtures alone.
3. Validate named presets, compact controls, shared menus, temporary expansion and
   group reload with real Resolve widget/GPU content.
4. Continue group-aware maximize/backtick conflict resolution, native title-bar
   attachment, Timeline header controls, size/scale feasibility and nested groups.

## V2 candidate handoff

Built candidate: **1f22db1**, branch macos-docking-layouts-v2. Panel-session,
panel-group and reliability suites pass, including posted native mouse-up,
background visibility and Workflow focus checks. Widget/helper builds and native
bootstrap tests pass. Qt-helper SHA-256:
dc9035152f2d390410180344857eda97cc1f2b83709f1d1ee3d8ccd1172a2a43.
Native bootstrap SHA-256:
64188f8b617a46aaae851cfd0a08645e843dba0c3a2e85a072e580009418ca6a.
Not installed at this receipt: Resolve pid 51419 is still running d3ed034.

Preserve the first-pass Git branch and executable checkpoint. A separate ignored
copy of Workspace.json and Arrangements.json is saved before any new installation.
The named “Jordan Arrangmement” preset remains unchanged by development tests.

Implemented: nested group trees; group-to-group merge and leaf insertion/removal;
old pair restoration; local 28-pixel targets and separate distant edge guides;
receiver-independent/native fallback drag completion; independent native background
hide/reveal; ordinary group Z tier; unmanaged Workflow focus protection; Center
and bulk recovery. Regression suites for panels, groups and reliability pass.

Live checks after installation:
1. Restore the existing named preset before changing it. Merge its left and right
   groups. Add a fifth panel, detach one, and confirm the remainder stays grouped.
2. Finish each drag with left release only. Try a short movement, Escape, Option,
   crossing another window and tearing out then docking elsewhere in the same drag.
   Small docking targets should appear only near eligible edges. Far guides align
   windows across a gap without attaching them.
3. Hide empty Resolve background. Use Relay and Terminal, then return to Resolve.
   Keep floating panels visible, Relay text focus intact, and background behind.
   Uncheck the option or Dock All to reveal the original window.
4. Center a hidden/off-screen panel and a whole group. Save As a new arrangement,
   restart, restore, and inspect nested orientations, dividers and visibility.
5. Compare Source multicam angle selection/switching and playback while native,
   floating and grouped. Check Program image, B/Ripple Delete and page switching.
6. With Inspector Stabilization visible, interact with it normally. The new log
   records native ancestry/action candidates without invoking Stabilize itself.

Not delivered: an independent Stabilization panel/shortcut, verified multicam fix,
independent UI scale, group-aware Expand/backtick, or native title-bar drop targets.
These remain open even when the new automated suites pass.

## Next candidate: exact live checks

1. Click a floating Timeline and use the user's B/Ripple Delete mapping once on
   an intended selection. Compare docked versus floating. Verify text entry still
   types normally. Record input.page-scope.restore and input.command-context logs.
2. Undock Source and Program in dual-viewer mode. Drag a dotted Unbound grip beside
   the other viewer until a blue outline covers both; release. Move the shared
   host by its title/grip, resize the divider, and inspect playback and both images.
   Option bypasses attachment. Test Side by Side and Stack in Join two panels too.
3. Drag a member title out of its group. Check initial pointer position and motion
   through release; no jump, trailing or delayed follower is acceptable. Detach and
   Release Groups are alternative explicit operations. Reattachment currently needs
   a fresh Unbound-grip drag. Groups currently contain two members only.
4. Save As a named arrangement; move/resize panels; check Modified. Update, Rename,
   Duplicate and Restore. Restart and verify groups, visibility, split and geometry.
5. Switch Edit to Color/Fairlight and back; then test Dock All. Check all content,
   original slots and B again. Close a group and verify both panels dock safely.
6. Shrink controls and scroll; test recognized ellipsis Dock/Undock against the
   control buttons. Expand/Restore works on independent panels; individual Expand
   is disabled while grouped. Backtick, native fullscreen fixes, UI scaling,
   Timeline header buttons and nested groups are not delivered by this candidate.

### Diagnostic identity and activation coverage (2026-10-16)

Every Qt diagnostic event identifies Unbound, its helper module, build and PID.
Controls and detached panes observe button pressed/clicked signals (including
keyboard activation); menu actions without shortcuts are observed too. Existing
mouse-press evidence records input before action dispatch. Native AppKit chrome
continues to report resulting window state rather than Qt button signals.
An atomic active-session marker survives abnormal termination; the next launch
reports an unclean previous session. This is evidence of missing clean shutdown,
not proof that Unbound caused a crash. No process-wide signal/exception handlers
replace Resolve's crash reporting. Helper builds retain a matching local dSYM for
symbolicating the named UnboundWidgetProbe.dylib frames in macOS crash reports.
Logs: ~/Library/Logs/Unbound/events.jsonl (and rotated .1).

Installed 2026-10-16: build **126798d**, after confirming Resolve was closed.
Both installed binaries match their built SHA-256 hashes:
- Bootstrap: 212e391b9642e07893abcd9956b0f4fb4712b3b8c36b24ed224cb61053491f00
- Qt helper: 755f51fd33f244d05064c7489e0ffc2e2d4a47023422a555514e913314c21af3
- Matching helper/dSYM UUID: C8F9184E-BDE8-3046-857F-09076D7523A0.
Archived binary and symbols under build/macos/checkpoints/126798d/.
Session suite and diagnostic reliability suite pass; reliability's first GUI run
had transient focus failures, two subsequent runs passed. Bootstrap fixture passes.
Live Resolve docking/Relay/multicam/Ripple Delete verification remains pending.

### Input regression follow-up (2026-10-16, after 126798d install)

User reports blocked Stabilize, Timeline audio/video divider and sometimes Dock
All after arrangement changes. Live log at 17:18:27Z shows Source Monitor drag.end
(deactivated) before drag.begin, followed by repeated clicks received by that
floating host and no Stabilize activation. This supports reentrant native mouse
capture escaping drag cancellation; exact Resolve reproduction remains pending.
Candidate installs release monitoring before acquiring capture, checks for
cancellation during acquisition, and releases capture with no active drag on
input. Orphaned-capture Escape and content-click fixtures pass. Docking markers
are narrowed from 28 to 18 logical pixels, approach range from 40 to 18; actual
hit targets take precedence over nearby inactive group candidates.
The combined session run failed an old assertion expecting a 28-pixel marker;
that assertion is updated, rerun pending. GUI tests can visibly activate test
windows: do not run them during the user's editing session. User reports screen
flashing during tests. No new helper is installed yet.

Installed input-fix candidate **f64c401** on 2026-10-16 after the user quit Resolve.
Session, nested-group and reliability GUI suites all pass, including the updated
18-pixel target assertion. Native bootstrap fixture passed at build time.
Installed bootstrap SHA-256: 6c95cf80c0e44f8f5b0b3c0e3c1a82c18de4ab552bd494ff7b8d27502dc86b25.
Installed Qt helper SHA-256: 87bbf73583910f5feb15618eceb86017180940d6a2ce44c76f9460227adbcf87.
Both match built files. Archived matching binary/dSYM under
build/macos/checkpoints/f64c401/; UUID 2DAA8FAC-AC79-39C9-B8E0-841FE4014F31.
Live acceptance remains OPEN: after floating/grouping/moving panels, repeatedly
use Stabilize, Timeline audio/video divider and Dock All. Inspect drag.begin/end
and drag.capture.recovered events alongside actual button activations. A fixture
pass is not confirmation that the reported Resolve input regression is fixed.

## Tab/maximize branch evidence (candidate, not installed)

Rollback remains `macos-docking-layouts-v2` at 4a3f13f, installed f64c401.
The retained live log records the successful screenshot layout at
2026-10-16T21:50:19.906Z: vertical root, horizontal Media Pool/Inspector top row,
Source below. Earlier failed-drop events were no longer retained. Do not invent
an exact failed-drop diagnosis from that successful record.

| Item | Implementation | Fixtures | Resolve acceptance |
| --- | --- | --- | --- |
| T1/T2 | Fixed isolated anchors, separate outer-group anchors, frontmost target, free release without automatic edge snapping; Shift alignment, Option bypass | No-GUI layout-plan suite PASS; GUI compilation PASS | Pending |
| T3 | Native Qt tab stacks mixed with splits; explicit Tabs button and center-drop path; existing group merge/tear-off | Tree plans PASS; tab/group/door GUI fixtures compile | Pending |
| T4 | Backtick/tilde routing with typing/dialog guards; temporary group layout loan and retained full group snapshot | Hover/second-press and grouped restore fixtures compile | Pending |
| T5 | Native fullscreen action metadata expanded; panel maximize remains separate | No fullscreen fix claimed | Exact command/shortcut and runtime trace pending |
| T6 | Native toggle clicked/check-change signals coalesced; split wrapper collapse, hidden tabs and tab reveal | Native button/shortcut-style check/tab fixtures compile | Actual Q mapping and other panel commands pending |
| T7/T8 | Per-host diamond pin and persisted pin/tab/split fields; old missing pin fields normalize to false | Group/pin and save/restore fixtures compile | Pending |
| T9 | Existing capture recovery retained; native mouse-up position captured before deferred dispatch; stale deferred drops rejected when a new drag starts | Prior capture suite passed on f64c401; updated suites compile | Pending |
| Viewport | Reachable grip instead of full-frame clamping; explicit recovery centers panels; display changes do not reveal hidden panels | No-GUI geometry checks PASS | Pending |

No-GUI check: `bash infrastructure/macos/test-layout-plans.sh`.
Compile-only checks: `UNBOUND_TEST_COMPILE_ONLY=1 bash infrastructure/macos/test-panel-sessions.sh`,
`test-panel-groups.sh`, and `test-reliability.sh`. These compile without opening
windows. Do not label a compile-only invocation a GUI test pass.

Candidate diagnostics include selected target, target kind, committed edge and
receiver, maximize/restore, visibility and pin state. Native release captures the
position immediately instead of reading the cursor after queue latency. Event
rotation retains five previous 8 MiB files so reproductions survive longer.

### Live checks before accepting this candidate

1. Move standalone windows through desktop and ordinary content: no rectangle or
   forced screen-edge docking. Release near an edge away from its fixed marker:
   preserve free position. Shift aligns; Option suppresses all target behavior.
2. Reproduce both screenshots: Media Pool beside Inspector, Source below both;
   create repeatedly by intentional panel-edge drops. Preview must match commit.
   Then put Inspector and Media Pool in tabs above Source. Repeat group-to-group.
3. Backtick over each panel in native, floating, split and tab placements; move
   pointer away; press again. Verify original slot, ratios, selected tab and frame.
4. Q/native Inspector toggle: covered tab reveals, next press hides, next restores.
   Repeat native toggles for Media Pool/Effects/Index in split and standalone hosts.
5. Verify native fullscreen command independently once the exact command is known.
6. Repeat Stabilize, Dock All, audio/video divider and Timeline shortcuts after
   every arrangement change. No stuck mouse grab or unreachable control allowed.
7. Partially park windows offscreen; retain position and size with a reachable
   grip. Explicit Center/Recover recalls. Test screen removal and hidden panels.
8. Save a new named arrangement; restore and restart. Original named presets stay
   unchanged unless Update is explicitly chosen. Verify pin behavior with menus,
   dialogs, Relay and other apps without focus theft.

Trace follow-up: destination selection now stops at a covering non-dockable Qt
window instead of looking through it for an eligible group. A covering-window
regression fixture is added (GUI execution pending). This is separate from native
macOS tiling and does not change any global macOS or third-party app setting.

Built candidate **256c848** (not installed). Helper and bootstrap build PASS;
bootstrap SDK handshake fixture PASS. All three updated GUI test executables
compile in compile-only mode; they have NOT been executed for this branch because
Resolve is open. No-GUI layout plans PASS. Archived candidate plus matching dSYM:
`build/macos/candidates/256c848/`, UUID
4F01ABD5-F231-3F7A-90D9-B320D30FC21D. Working source is committed; installed
f64c401 and its preserved branch/archive remain unchanged. Fullscreen viewer is
explicitly unfinished, awaiting the exact native command/shortcut and live trace.

Cinema clarification: the user means Resolve's **Cinema Viewer**, not Enhanced
Viewer, Full Page Viewer or Full Screen Window. Read-only accessibility inspection
of the running Workspace > Viewer Mode menu confirms Cinema Viewer enabled and
Enhanced/Full Page disabled. Shortcut attributes are absent from the accessibility
menu, so absence there does not mean the user has no native binding.
Added passive Cinema Viewer diagnostics: QAction dispatch and settled-window
snapshots, plus runtime-confirmed non-cloned cinema/active/focus player signals.
Private native methods are never invoked. Mac runtime metadata decides whether a
signal is connected; old Windows binary names are not assumed to exist on Mac.
This instrumentation is not a Cinema Viewer fix. Live command testing remains
pending after the user closes Resolve for GUI checks and a new helper install.

### GUI validation before installation (2026-10-16)

The first session fixture exposed a real target-picker limitation: AppKit's
NSApp.orderedWindows omitted owned Qt player tool windows. Target selection now
uses the actual CoreGraphics on-screen stack filtered to this process, including
child windows, without capturing images or reading other apps' window contents.
The fixture additionally guards missing markers, keeps Unbound controls away from
its intended target, and tests blocking with an explicitly foreground test window.
WindowServer commits ordering asynchronously; ordering assertions allow up to
250 ms to observe the requested stack rather than sampling immediately.

After those corrections, all three actual GUI runs PASS:
- `bash infrastructure/macos/test-panel-sessions.sh`: including grouped backtick
  expand/restore, no forced free-drop alignment, covering-window exclusion, input
  capture recovery, viewport/persistence and native order/focus scenarios.
- `bash infrastructure/macos/test-panel-groups.sh`: including split/tab mixtures,
  native Inspector click/check-change coalescing, covered-tab reveal, hide/restore,
  group merge/tear-off and temporary grouped layout loan/return.
- `bash infrastructure/macos/test-reliability.sh`: focus, shortcut scope, text input
  protection and distinct Cinema Viewer diagnostic dispatch.

These passes do not close Resolve runtime acceptance. Cinema Viewer has passive
trace coverage, not a verified fix. The next live check is its actual command in
a detached/grouped arrangement, compared with native docking.

### Installed 21466f2

Installed after confirming Resolve closed, 2026-10-16. Build and native bootstrap
fixture PASS, three GUI suites PASS, no-GUI layout-plan suite PASS. Installed and
built hashes match:
- Bootstrap: cc7ee209d0bd46ef4943043e014fe100dfcb339883ffeeb2d73dd69dad0bd43d
- Qt helper: 815c08effce455109f89f2696eea25ea91ffeb0617943628f858bc355c5ae256
Matching crash-symbol UUID: 28D2F0A3-2299-3EF1-B1B3-89D66E8A3B62.
Archive: build/macos/checkpoints/21466f2/. Previous f64c401 branch/binaries retained.

Live user handoff: drag grips/member headers; center marker creates tabs, inner
edge markers split one panel, outside-group markers split the group. Release
away from markers floats freely; Shift requests edge alignment; Option bypasses.
Test native Q Inspector toggle in split/tab arrangements, backtick/tilde expand
and second-press restoration, partial offscreen placement, and Stabilize/dividers/
Dock All after moves. Exercise Cinema Viewer separately and collect its trace;
this installation adds Cinema Viewer diagnostics, not a confirmed cinema fix.


### 2026-10-17 — workspace drag architecture branch

See [implementation checkpoint and pending acceptance gates](macos-docking-interaction-research.md).
Regional drop policy is connected in source, with separated destination resolution and candidate native viewer lifecycle. Layout/session/group/reliability/hidden-control fixtures passed. This is an uninstalled candidate, not live Resolve verification. Footer/submenu integration, subtree tear-off UI and shared command state remain open. Preserve macos-docking-compass-and-controls as rollback.


### User-confirmed baseline — 6bd0e36

Local annotated tag: `macos-solid-docking-2026-10-17`. Installed helper and symbols archived under ignored `build/macos/checkpoints/6bd0e36/`, with installed hashes in receipt.json. User reports mostly progressions; preserve this exact executable as rollback. This does not close outstanding acceptance items.

Next-pass requirements from live feedback:
- Backtick expansion must temporarily bring the expanded panel above other Unbound panels/groups, then restore its placement, group and prior stacking policy on the second press. Keep Cinema Viewer separate.
- Profile drag latency before optimizing; preserve improved split placement.
- Drag a tab out in one continuous gesture, then redock or leave floating.
- Reliable horizontal/vertical split dividers; reproduce Inspector above Source in the left column with Media Pool spanning the right.
- Clear tab placement previews and discoverable grouping.
- Native Workspace submenu callers for panels and footer; hide the native backdrop by intent with explicit Show Native Workspace recall.
- Source callable without manually detaching through native UI.
- Footer controls/submenu need live discovery and usability verification; user cannot find a dependable path.


### Next branch: macos-docking-interaction-polish

User requirement: the native toolbar must also be independently undockable and placeable, with a discoverable native undock control/menu action, a Show Panel in Workspace caller, and persisted placement/visibility. Identify the exact native toolbar container before implementation.

Global discoverability rule: every detachable element (including toolbar, footer/page navigation, Source, Program and Timeline) must have a discoverable undock action plus a shared-state caller in Workspace > Show Panel in Workspace. Hiding a panel must not remove its caller. Native macOS menu verification is mandatory; a QWidget Show-event fixture alone is insufficient.

Current source work (not installed): separate TabTearOff, PanelExpansion, DockFeedback and DragTiming modules; candidate source/offline-stack detachment; regression fixtures for empty Source, tab drag and temporary expansion priority. Compile/fixture results must be recorded separately from live verification.

Verification at polish checkpoint: layout-plan tests PASS; hidden controls/tab-tear-off tests PASS and foreground preserved; group fixtures compile-only PASS; widget helper builds with warnings as errors. Displayed fixtures and live Resolve checks pending because Resolve is open. Toolbar/menu/door sizing work remains unimplemented at this checkpoint.


### Native workspace callers checkpoint

Published fork: https://github.com/jordantetewsky/unbound. Baseline branch and solid-build tag pushed; unfinished interaction candidates preserved on macos-docking-interaction-polish. Continuation: macos-native-workspace-callers.

Implemented in source: workspacecallers.h prepopulates Source, Program, Timeline and Show Native Workspace actions; footer registration also prepopulates rather than relying solely on QWidget Show. Uses aboutToShow for refresh. Helper builds; hidden tests PASS for pre-display discovery, shared state and no duplicate menu entries. Cocoa menu appearance and actual Resolve callers are NOT live verified. Native existing panel actions/shortcuts and stable door sizing still require completion. Toolbar is required but not implemented; identify its container before claiming detachment. No new install.


### Door behavior and explicit acceptance audit

See [next-pass acceptance record](macos-next-pass-status.md) for implemented/tested/live/open distinctions. GroupVisibility now tracks divider sizes by open/closed combination and previous selected tab; grouped headers gain Hide. Hidden 20-cycle sizing and tab return tests passed after explicitly refreshing splitters. Native existing actions are reused and route through the same queued door transition; hidden tests verify action identity, shortcuts and single dispatch. Destination lookup is linear rather than repeated native ID lookup inside nested loops; normal drags skip alignment collection. These changes are not installed or live verified.


### Implementation and regression pass — toolbars, viewers, layout

- Both native toolbar rows now have panel definitions, persisted placement, native-layout Undock buttons and workspace callers. Detachment retains the workspace single/dual-mode stack. Displayed toolbar fixtures pass.
- Footer native-layout Undock control added; submenu prepopulation retained. Displayed footer controls and original-slot recovery pass.
- Empty Source stack detachment and native Single Viewer Mode routing pass displayed integration tests. The test deliberately uses F9 to prove command identity rather than Shift+W hardcoding.
- Nested tab containing a split section can detach into its own host, preserving its tree, then merge back. Rollback is explicit on initialization failure. Displayed fixture passes.
- Reported Inspector-above-Source/Media-Pool-right layout has a mouse-driven divider regression and 20-cycle exact sizing checks; passes. Splitter handles are 6 logical pixels.
- Cinema/native expansion priority no longer assumes NSFloatingWindowLevel is above pinned Qt controls: the fixture measured controls level 8 and now verifies viewer level 9. Original levels restore; app deactivation yields priority. Actual Resolve rendering remains unverified.
- Preset controls extracted into arrangementcontrols.h, reducing the main controller rather than adding another large feature block there. Preset/panel regression suite passes.
- Floating workspace is the explicit preference, with native recall/Dock All restoring visibility. Existing native ownership remains intact. Workflow/Relay activation still requires live validation.

Full automated suites have separate results in the acceptance record; live checkboxes remain open. Independent interface scaling and section-level Stabilization extraction remain separate investigations, not completed features.


Lifecycle follow-up: native toolbar buttons, footer controls and workspace actions survive destruction/recreation of the controls owner. Per-controller connection tracking replaces persistent wiring flags; displayed toolbar rebind and hidden action rebind fixtures pass. The fixture for native Cinema window/key/level restoration now passes alongside timeline shortcut/text focus and page-scope tests. Preset refactor keeps the controller at 807 lines (down from 883 before extraction). Live Resolve rendering/menu/workflow checks remain required.

2026-10-17 continued pass on macos-tethered-timeline-and-viewers: fe6e347 adds reversible narrower Timeline/Inspector/Media Pool containers and compact Page Navigation. Follow-up addresses custom parented Mixer/Timeline menus, pre-display population, detached Single Viewer command discovery, hidden empty Source detachment, Cinema recovery suspension, inline toolbar controls and collapsed tools placeholders. Geometry logging and Show-event scans coalesce during interaction. See macos-next-pass-status.md for exact fixture evidence and remaining live checks. Independent full-panel scaling remains unimplemented; intermittent preview/order misses remain open rather than being dismissed after a passing rerun.
