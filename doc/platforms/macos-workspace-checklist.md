# Mac workspace checklist

Working branch: `macos-docking-layouts-v2`.
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
