# Mixer panel repair — October 26

Branch: `fix/mixer-panel-repair`, based on accepted main checkpoint `6833b68`.
Native navigation fixes are merged in main. The ineffective floating Source blink
trial remains isolated; Jordan deferred that polish. This branch excludes it.

Read-only payload verification at 06:35 UTC matched all four installed bundle files
to the `0f092e3` blink-trial receipt. No Mixer installation, merge or push has occurred.
Current user layouts and preferences were not edited. Resolve was closed for the
displayed fixtures. Build and fixture output is under `build/macos/mixer-*`.

## Evidence and bounded changes

The current checklist and numbered native-entry contract govern behavior. The
older Downloads/behavior HTML supplies visual examples only. Its compressed
startup Timeline and the supplied maximized Mixer screenshot remain live evidence.

- Resolve Layout now gives Mixer an independent leaf beside Timeline. Saved user
  arrangements and the user's current unsaved arrangement are not migrated/reset.
- Managed Mixer uses the same header, placement control and title drag as other
  panels. Native options remain usable. A reversible native-header lease restores
  the previous explicit visibility when leaving a managed header.
- Expansion temporarily removes the native `frameEditAudioMixer` width cap,
  preserving track-control constraints, and restores that exact cap afterward.
  Earlier native evidence measured an 864-wide cap in a 1728-wide host.
- The displayed assembled fixture exposed a discovery lifetime bug: after content
  restoration, the empty replacement widget retained the native object name until
  deferred host deletion. A subsequent restore could select that retired Mixer
  instead of the live nested native panel. The session now clears the replacement
  name as soon as native layout ownership is restored. The formerly failing
  stack-drop -> Resolve Layout restore -> Mixer flow passes with this change.

## Crash distinction

Captured Resolve crash: session `5de8867e-2df3-495b-86fd-2a2b6f9d0b31`, build
`c7920d6-dirty`, October 26 05:40:55 UTC. Signal 11 in native
`BmdDavUI::UiGraphicsRenderWidget::event`, through Qt native-window creation and
`ManagedPanelSession::setHiddenByUser`. Last input was the native Mixer door after
Mixer expansion/restoration, Timeline expansion/restoration, floating Mixer close
and recall. Capture lives in the sibling native-loading-normal-launch worktree at
`build/macos/mixer-crash-20261026/`.

This identifies the failing sequence, not a proven crash cause. The retired-name
bug is independently reproduced in Qt fixtures; do not describe it as proof that
the native GPU crash is fixed. Native parked-layout geometry warnings are another
unresolved lead, not justification for speculative geometry controllers.

## Organization review

- `NativePanelHeaderLease` owns reversible suppression of redundant native header
  controls. `GroupPanelHeader` owns the lease; NativeArrangement and PanelGroup
  use it instead of their separate hide loops.
- `PanelExpansionConstraints` owns temporary native content constraints;
  `PanelExpansionController` wires acquire/release to its existing transaction.
  Native foreground priority remains in `PanelExpansion`.
- `ManagedPanelSession` already owns replacement-widget lifetime; retiring its
  discovery name belongs at the existing native-layout return boundary.
- Mixer behavior checks moved out of the growing first-open test into a focused
  helper using the same assembled fixture and production command entry points.
  No parallel visibility controller, state model or keep-showing timer was added.
- All production changes are shared core/recipe changes. Both platform adapters
  retain their interfaces. Windows compilation and live testing remain unverified.

## Contract and acceptance gates

The numbered entry flow was reread. First-open/restart, immutable recipe policy,
Native return, persistent Native gesture and re-entry, band allocation, standard
headers and maximize are exercised through the assembled first-open fixture.
Native-entry and panel-group regression suites pass. The profile check confirms
recipe consistency and no overwrite. A displayed assembled pass includes actual
pointer targeting of the Mixer header. These do not establish native GPU painting.

Final verification passed:

- `mixer-first-open-final-v3.log`: assembled hidden flow, including the complete
  last-panel detachment with independent Mixer, Native return and fresh restart.
- `mixer-mounted-final-v2.log`: displayed flow, actual header pointer maximize,
  Mixer and Timeline expansion followed by Mixer close/recall, docking and restart.
- `mixer-groups.log`, `mixer-native-entry.log`, `mixer-default-recipe.log`: shared
  group behavior, native entry and immutable recipe/no-overwrite checks.
- `mixer-helper-build-final.log`, `mixer-bootstrap-build-final.log`: production
  helper and bootstrap compilation, bootstrap handshake/lifecycle checks. Both
  binaries contain arm64 and x86_64 slices. `git diff --check` passes.

The first Mixer-only snapshot in `build/macos/mixer-panel-candidate/` is superseded
by the default-presentation work below; do not install that earlier snapshot.
Build identity is `6833b68-dirty` on `fix/mixer-panel-repair`; use the staged hash
to distinguish candidates. No commit, merge, push or publication.

Live gates, requiring candidate-specific installation authorization:

1. In the current retained layout, float Mixer, maximize and restore. Channel
   controls must fill the usable area and remain operational without clipping.
2. Maximize/restore Timeline, then close floating Mixer and recall through its
   native door. It must retain its placement and work without crashing.
3. Independent Mixer header/neighbor behavior in the updated immutable Resolve
   Layout requires a separately saved recoverable current view before selecting
   it; do not silently reset the user's layout to demonstrate the recipe.

Actual rendering, complete live grouping/tab
acceptance, Windows parity, and the rest of the checklist remain open. Ordinary
launch native navigation is inherited from accepted main; no launch flag is
required. Command submenu, Fixed Playhead, Stabilize, page state and remaining
contracts have not been reaccepted live by this Mixer work.

## Default presentation clarification and reproduced sizing defect

Jordan clarified that the default must display its complete parts properly and
look right as a preset, not merely contain the correct panel names. Timeline must
have usable height below the viewers; Mixer must be a properly fitted neighbor;
headers, native toolbars and tabs remain intact. Reopening still preserves the
user's last view. Do not apply the default over an existing saved/unsaved view.

The assembled fixture now checks the default center's 3:2 viewer/Timeline split.
That check failed before the sizing change (`default-proportions-before.log`) and
passed after it (`default-proportions-after.log`). Qt's local 5.15.2 source,
`qsplitter.cpp::setSizes_helper`, confirms why: `setSizes({3,2})` interprets the
values as pixel sizes and clamps each to its widget's smart minimum before
allocation. Unequal native minima therefore replace the preset's intended ratio.

`PanelLayoutProportions` now converts saved weights or pixel measurements into
requests for the splitter's actual available extent, applying parents first.
NativeArrangement and PanelGroup use this same function; their duplicate direct
`setSizes` loops are removed. There is no second state owner or settling timer.
Native content minima remain enforced by Qt. Exact same-extent pixel restoration,
resizing, unequal minima and minimum preservation pass the shared layout policy
fixture; floating group and persisted-control regressions also pass.

This demonstrates a cause of distorted startup proportions. It does not establish
that every live Timeline painting/clipping symptom is fixed.

Final default-presentation verification passed: `default-presentation-mounted.log`
(displayed assembled flow and restart), `default-proportions-after.log`,
`default-proportions-policy.log`, `default-proportions-groups.log`,
`default-presentation-build.log` and `default-presentation-bootstrap.log`.
The current reviewable candidate is `build/macos/mixer-default-presentation-candidate/`:
exact bundle, full `review.diff`, source/payload hashes in `candidate.json`.
It is built, locally verified, and not installed. Candidate-specific authorization
and live acceptance are the remaining gates, including the complete default's
native rendering and Mixer stability. Earlier Mixer-only staging remains preserved
but superseded. Windows runtime verification remains open.

## Authorized installation — October 26

Jordan authorized installation of the Mixer/default-presentation candidate.
Installed `6833b68-dirty` from `fix/mixer-panel-repair`, helper SHA-256
`314fd16443c594bc1b296215b407bd33ca21f0d9de6bcb7918e2745ce10dd8f8`. All four installed bundle files
match the staged candidate; Current metadata matches; all 24 protected settings
and preference files are unchanged. Resolve was confirmed closed before install
and checked again by the installer. Rollback checkpoint:
`/Library/Application Support/CineWaltz/OpenGate/Transactions/7FB5D8FC-A564-4E7E-B3E0-1C0B03E4F486`.
Receipt: `build/macos/mixer-default-presentation-candidate/receipt.json`.
This supersedes earlier uninstalled statements. No merge, push or publication.
Native live acceptance remains pending. Launch normally; preserve the current
view with Save as New before selecting Resolve Layout to inspect the updated
default. Check complete panel presentation, then Mixer maximize/restore and
close/recall after Timeline expansion. No launch flags are required.

## Live feedback and nested-section follow-up — October 26

Jordan reports Mixer mostly working and the default's initial appearance mostly
correct. Remaining problems begin during drags/rearrangement. Preserve the new
default proportions; do not redesign the preset to avoid the failing structure.
Evidence: `build/macos/live-mixer-source-20261026/`, including screenshots and all
rotated logs captured at investigation start. Session
`67755275-5c94-4e3d-be38-83f06721e8f8`, PID 34577, installed `6833b68-dirty`.

Source below Media Pool becomes a split section inside the default's Media
Pool/Index tab group. NativeArrangement looked for ANY ancestor tab and suppressed
every descendant leaf header, competing for the section's corner controls. The
other preset's ordinary neighbor structure does not enter that branch. This is
a shared presentation inconsistency, not evidence of a Source loading failure.
The new assembled regression fails all three header/corner assertions before the
repair and passes afterward (`section-headers-before.log`, `section-headers-after.log`).

GroupPanelHeader now determines direct tab-page membership for both embedded and
floating callers. Descendants of a split section retain individual headers; only
a direct tab leaf uses the tab title. Duplicate membership logic was removed from
both coordinators. The shared layout builder specifies right-side text elision
and scrolling for long tab titles. Native styling and real content are retained.

Existing drag logs show active targets alternating with no target, but omit pointer
and destination bounds. They do not establish a Mixer-specific target failure.
Only one Mixer drag appears in the captured session and completes successfully;
most recorded repeated target searches are Source drags. The existing
`dock.target.selection` event now includes pointer and candidate bounds to separate
missing destinations from neutral drop zones. No speculative target-selection
behavior change, new controller, or timer was added.

Jordan later could not readily reproduce after restart. Do not ask for repeated
layout tours: the screenshot structure and deterministic local failure are already
preserved. Exact restart-state differences remain unproven. The follow-up remains
uninstalled; the mostly-working live installation is preserved. Windows runtime
verification remains open.

Follow-up verification: assembled before/after regression passes after repair;
universal helper and bootstrap pass; floating-group fixture compilation passes
(runtime not rerun while Resolve is open). Exact uninstalled bundle and a bounded
diff relative to installed source: `build/macos/section-header-candidate/`.

## Authorized header-repair installation — October 26

Installed approved nested-section header repair at 06:58:58 UTC with Resolve closed.
Helper SHA-256 `e260d08b8992b844b3957f29c752f3bed42fac8e64340d0e4621505d7b408d99`; all four installed payload files and
Current metadata match. Rollback checkpoint: `/Library/Application Support/CineWaltz/OpenGate/Transactions/2EC009F8-CF62-4E57-8329-C39B34CB6333`.
Receipt: `build/macos/section-header-candidate/receipt.json`.

Resolve reopened at 06:59:06 UTC (session `ab819ac1-c778-46b4-a4dc-bac64c15332d`)
during verification. Twenty-three protected files remain identical; Workspace.json
changed with recorded successful runtime saves beginning 06:59:18 UTC, after
installation completed. Its pre-install copy is preserved; the live workspace
was not overwritten to force a checksum match. Earlier uninstalled statements
are superseded. Live header/tab acceptance and docking-target investigation
remain open. No merge, push or publication.
