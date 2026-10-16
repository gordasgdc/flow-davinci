# Mac workspace checklist

Working branch: `macos-arrangements-and-docking`.
Preserved reliability branch: `macos-workspace-reliability` at `c5cf2e4`.
Preserved baseline: `macos-panels-workspaces` at `9ba3fb7`.
Reliability changes are implemented and pass automated fixtures. Fresh-process
Resolve acceptance checks remain pending; installed helper is c5cf2e4.
User reports some improvement, but Ripple Delete still fails in floating Timeline.

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

## Cross-cutting verification

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
| M1–C2 | — | — | — | Planned |
| R2 | Installed c5cf2e4 | Owned-window shortcut fixture passes, but does not reproduce actual Ripple Delete failure | User confirms Ripple Delete still fails with Timeline undocked | Open; requires native command/focus diagnosis |
| R2 diagnostic | c5cf2e4, pid 46314 | — | 2026-10-16T13:58:23.710Z: B reaches floating Timeline with m_TiTimelineView (UiTiSequenceView) focused; no corresponding shortcut/action observed. Ctrl+Z subsequently produces input.shortcut. Focus alone does not explain failure. | Native command routing investigation |
| U1 | Continuation branch | Scroll area added for options and eleven-panel list; status and bulk recovery buttons remain outside it. Regression suite passed before adding explicit compact-size assertions. | Pending | In progress |
| P1–P2 | Continuation branch | Named preset creation, duplicate-name rejection, rename, independent duplicate/update, deletion, reload and invalid-file preservation pass. Control restore fixture restores Source only with saved size; current changes do not update saved presets. | Pending; groups and UI scale remain unimplemented | Implemented for current panel/options model |

## Install handoff

Every install must report build identity, changes actually included, exact user
test steps, and known unfinished behavior. Never present fixtures as live proof.
Do not ask the user to test features absent from that build.

Latest install: c5cf2e4; matching bootstrap and Qt-helper SHA-256 verified.
Test: click Timeline and invoke the user's B/Ripple Delete mapping; compare docked
and floating. Switch Edit to Color/Fairlight and back; check Timeline returns.
Alternate Source and Program; check both image and controls. Try Dock All after
page changes. Magnetic docking, presets, maximize and independent UI scale are
not included. Ripple Delete is already reported failing and remains open.

## Current work order

1. Build and identify the reliability helper; install when Resolve is closed.
2. Validate R1–R5 in a fresh Resolve process and retain diagnostic evidence.
3. Add named presets (including Duplicate), explicit Update and a modified indicator,
   alongside compact scrolling controls. Named presets must remain separate from
   automatic last-arrangement saves.
4. Implement temporary maximize, investigate size/scale limits, then magnetic
   snapping and horizontal/vertical groups with persistence and regression checks.
