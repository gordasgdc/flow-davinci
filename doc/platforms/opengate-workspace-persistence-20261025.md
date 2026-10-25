# Workspace persistence extraction — October 25

This candidate combines the retained Close repair at `2ecb46e` with a bounded
capture/save extraction. No native-loading or Mixer-sizing repair is claimed.

## Responsibility review

`PanelControls` keeps UI actions and UI refreshes. `WorkspacePersistence` captures
existing live owners and saves through existing `WorkspaceSnapshot`,
`WorkspaceSettings` and `PageWorkspaceState`. Sources are borrowed synchronously;
there is no second layout model, timer or widget controller. The old capture/save
implementation is removed from the coordinator, including its expansion caller.
PanelControls falls from 1,334 to 1,291 lines; the focused owner is 126 lines.
This is one extracted responsibility, not a claim that the coordinator is fully
organized. Shared core is used by both platforms; Windows execution is pending.

The capture/persist timing span is now named `workspace.capture-persist` and excludes
UI refresh work. Existing phase logs remain. This is a measurement-boundary change,
not a speed improvement. Settings schemas and native-return checkpoint rules stay
unchanged.

## Local evidence

Evidence directory: `build/macos/persistence-verification/`.

- First-open assembled flow passes before and after extraction.
- Retained Close, workspace restoration, page workspace state, native entry flow
  and panel caller flow all pass on the extracted implementation.
- Complete settings and named arrangements were captured at first-open, before
  and after native return, before restart and in the fresh restart process.
- Unseeded before/after comparisons differ in divider sizes. A second unchanged
  baseline also differs. This is not evidence of exact equivalence or a proven
  refactor regression; the baseline variability remains an open investigation.
- With `QT_HASH_SEED=0` for both parent and child, all five complete snapshots match
  before/after extraction. Only generated preset UUIDs are replaced with their
  preset names for comparison; no dimensions, visibility, selection or other
  state fields are excluded. See `seeded-comparison.json` and the source JSONs.
- Universal OFX and helper compilation passed. Final clean build and displayed
  results are recorded separately below. No installer was run.

## Numbered entry contract mapping

1–3: first-open fixture exercises initial assembly, protected default, named save,
unsaved restart and Native return/re-entry through production callers.
4–5: native-entry and first-open fixtures exercise temporary expansion and persistent
Native gestures without loading a default.
6–9: first-open and panel-caller fixtures exercise primary band ownership, band
visibility, flat panel access, retained close/recall and native restoration.
10: widget geometry checks do not establish native GPU painting or Mixer contents
filling a maximized window. Real Resolve acceptance remains open.
11: page-workspace-state and first-open exercise independent state and page return;
Windows runtime parity and all supported real pages remain unverified here.
12: assembled fixture passes; real pointer gestures, remapped hotkeys, command
submenu behavior, Relay/Deliver and correct clip pictures still need live review.
Displayed fixture checks are separate from Resolve live acceptance.

## Remaining gates

Displayed window/drag checks require Resolve closed. Installation requires Jordan's
explicit authorization after candidate review. No merge, push or publication.
The Source experiment flag dependency, blinking, floating disappearance, native
painting and prior full-layout Dock rejection are not closed by this refactor.
