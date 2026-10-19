# macOS integration ledger — October 18, 2026

Integration: `macos-panel-discovery-and-layout`. Preserved public baseline:
`fbe835c` / `macos-responsive-group-transfers`. No changes to master or upstream.
The new integration has NOT been installed. Resolve was closed for the native
fixture runs recorded below. Actual Resolve acceptance remains separate.

Latest combined candidate: 19 provenance entries, including scoped initial native
drop positions and the separate Inspector transfer fix. Source and tests match
`68b7f5f`; all 15 functional suites passed at that commit. Responsiveness remains
FAIL, and all feature acceptance entries remain pending. Detailed evidence:
`../unbound-features/native-arrangements/build/macos/verification/68b7f5f41f-20261019T034539Z/receipt.json`.
See `macos-native-panel-positions.md`, `macos-inspector-native-transfers.md` and
the newest workspace-checklist entry for current implementation and limits.
Older draft-status paragraphs below describe earlier checkpoints.

A feature tip in the build manifest means its source is included, not that its
acceptance requirements are complete. Worktrees live beside the main repository
under `../unbound-features/`. Merge feature branches into integration; do not move
master until reviewed acceptance evidence justifies it. Each branch starts from
8828868; downstream feature dependencies must be merged explicitly.

| Area | Feature branch | Implemented so far | Still required |
| --- | --- | --- | --- |
| Discovery | feature/macos-panel-discovery | Workspace UI Panel Manager entry; renamed window; native-header undock glyph beside recognized options; existing-header hidden-panel callers | Native fixtures/live verification; complete panel coverage; native live verification |
| Native button drag | feature/macos-native-panel-drag | Drag threshold, readiness-based host capture, release/click separation, header callback cleanup | Native drag/drop and lifecycle fixture execution; delayed readiness/live behavior |
| Placement semantics | feature/macos-panel-placement | Original-slot leasing separated from native-surface and floating-surface placement; headless state tests pass | Native-session fixture coverage; command and embedded-adapter wiring |
| Shared layout surface | feature/macos-layout-surfaces | Split/tab renderer used by floating groups and usable in an ordinary embedded host; invalid-tree checks before ownership changes | Native fixtures; actual original-window slot adapter and embedded arrangement persistence |
| Containment/width | feature/macos-panel-containment | Debounced boundary audit; owned group cells get native clipping NSViews | Reproduce actual Resolve leakage and prove fix; divider/resize/input and GPU/playback checks |
| Narrow Inspector | feature/macos-panel-overflow | Retained native content in a reversible scroll viewport; original minima/policy/background retained; grouped door/recovery/scroll and maximize-offset fixture checks PASS | Live Resolve validation; other panels; independent UI scale remains separate |
| Group signal lifetimes | feature/macos-group-signal-lifetimes | Group transfer disconnects only its own receivers; external callbacks and Qt accessibility destruction tracking survive; focused regression failed before and passes after the fix | Full integration and live Resolve validation; not a claim that all crashes share this cause |
| Nested doors | feature/macos-nested-panel-doors | Reveal every enclosing tab; close restores prior outer selection; covered-panel hiding leaves current view unchanged; transition logs | Live command/shortcut checks and larger nested-layout matrix |
| Native-first startup | feature/macos-native-first-startup | Manager initializes hidden; fresh defaults retain native background; stale launch requests consumed; explicit/legacy preferences retained; no key-window change in fixture | Fresh Resolve acceptance; original-window split/tab rearrangement is still open |
| Native workspace | feature/macos-native-workspace-space | Hidden restoration placeholders; transferred empty containers collapse; scoped native outer sizing/restoration | Native fixture/live verification of shrinking and restoration; drag/split/tab rearrangement inside native surface; native recovery and focus |
| Commands | feature/macos-command-registration | Read-only native keyboard-dialog interface inventory; native Keyboard Customization launch from headers/Workspace UI | Command registry; assignable maximize/reveal for every panel; collision/typing behavior; native keyboard registration and persistence; viewer command regression matrix |
| Clip colors | experiment/macos-custom-clip-colors | Known native palette action inventory, type metadata only | Storage investigation on disposable project; bright Red, picker/name/edit/remove; saved color identity; native menu and shortcut commands; reopening/undo/redo |
| Tracing/performance | feature/macos-interaction-tracing | Bounded pointer position/speed samples; receipt-to-next-turn records; existing action and latency logs retained | Runtime tests for new records; hook new command outcomes; repeated performance matrix with all features; actual playback/paint limitations documented |
| Empty Source/viewers | feature/macos-viewer-commands | Offline Source stack discovery without a loaded player; regression fixture | Live empty/loaded Source tests; Single Viewer and Cinema command matrix; maximize restoration |
| Independent scale | experiment/macos-panel-scaling | Feasibility/restoration specification; no scale implementation | Feasibility and hit/render/menu/GPU tests; +/-/100% controls; independent persistence; exact native restoration |

## Cross-feature acceptance (all open unless evidence is explicitly recorded)

- Original window can become smaller as panels leave; remaining native panels
  stay usable. No requirement to manually hide a backdrop for ordinary arranging.
- Same discoverable docking language inside native and floating arrangements:
  arbitrary groups, left/right/above/below/tab; target labels; reliable tear-off;
  no zones across the whole screen and no accidental dock on free release.
- Maximize/restore round trip preserves panel placement, tabs, proportions,
  visibility, display and temporary front priority. Native Cinema Viewer and
  Single Viewer commands retain their distinct meaning and mappings.
- Hide/reveal acts as a door; repeated cycles preserve outer group frame and
  restore intended internal proportions/tab selection. All-hidden groups remain
  recoverable through menus. Pins stay beside traffic lights without overlap.
- Source detaches without loading a clip. Timeline keeps its toolbar tethered;
  Mixer, footer and all supported panels have discoverable native controls.
  Footer sizing/orientation and scale remain explicitly tracked.
- Presets keep IDs, page isolation, order and restart persistence; Native UI is
  permanent. Unbound Panel Manager is optional, not the main arranging workflow.
- Stabilize, Dock All, native editing buttons, Timeline A/V divider, menus,
  workflow integrations, project/page changes and multi-display rescue work.
- Pointer/click/command/result/geometry evidence is bounded, redacted and linked
  to the exact build and feature commits. Missing paint must not count as fast.
- No automatic installation while Resolve is open. Before replacing a helper,
  preserve the installed bundle, symbols, settings and manifest; verify signatures
  and exact file hashes. Do not claim complete based on compilation alone.

## Current verification

- Native workspace-menu, group, instrumentation, hidden-controls and reliability
  fixtures PASS. Session fixtures exposed a splitter restoration bug: proportions
  were applied while the replacement pane still inherited the hidden vacancy.
  Applying sizes after revealing it fixes the failure; feature and integration
  session runs PASS.
- Two preview assertions failed twice before a later pass. The fixture had only
  drained Qt events, without waiting for WindowServer presentation. It now waits
  on both native window IDs appearing (500 ms deadline); the integration session
  fixture passes with that explicit precondition. Repeated/live acceptance remains
  required; a passing rerun alone does not close the reported drop-zone issues.
- Native clipping rejection compares original attributes against their baseline,
  since Qt may already promote a sibling to native. No actual Resolve clipping,
  custom-color storage, native-shortcut registration, independent scaling or
  native-root rearrangement acceptance is claimed.
- Responsiveness fixture functional checks PASS. At `29a431d`, p95 event-turn
  times in ms: free drag 1.882, dock approach 6.705, dock commit 28.641,
  divider 10.827, door 15.232, viewer command 12.953, button 0.795.
  Dock commit still FAILS the unchanged 16.667 ms target. These are simulated
  input on real Qt/native fixture windows, not playback/GPU presentation latency.
- Viewer commands now reveal Source without raising/activating its group;
  ordinary grouped doors avoid normalizing or moving an already-visible host.
  Native viewer discovery is cached with menu action/language invalidation and
  QPointer lifetime protection. Tests cover late menu creation/replacement and
  live shortcut identity. The new responsiveness branch is
  `feature/macos-responsive-command-discovery`.
- An experimental repaint batch failed to improve docking latency and was
  reverted. No feature or diagnostic logging was disabled to improve measurements.
- Production rebuild and fresh Resolve acceptance are still required before
  claiming this pass installed or complete.

Source/native-toggle preparation now observes Show/ParentChange/LayoutRequest
readiness instead of waiting fixed 100/50 ms. The temporary event filter is
removed at completion/cancellation/deadline, with a one-second bounded fallback.
Feature and integration session fixtures PASS, including not-before-ready and
exactly-once completion. Actual empty Source behavior in Resolve remains a live
acceptance item; this does not prove lazy player creation in every Resolve state.

## Continued native-first and interaction pass

- Readiness observation also replaces the fixed 100 ms group-command delay.
  A fixture verifies ready panels join through event processing without sleeping.
- Native session, group, hidden-controls, reliability and instrumentation fixtures
  passed during this pass. Later changes still require combined verification and
  actual Resolve acceptance; none of the new candidates is installed.
- Narrow Inspector keeps real control constraints and supplies horizontal/vertical
  overflow, not UI scaling. The original rightmost button is the actual hit target
  after scrolling. Magnetic grouping, door cycles, native reclamation and native
  restoration pass. Temporary expansion can restore the prior overflow offset.
- Nested tabs now select the complete containing path when revealing a panel,
  then restore the preceding outer selection on close. Logs record level counts
  and index transitions without copying user project text.
- Startup no longer automatically opens Panel Manager. A fresh configuration
  keeps the native workspace; explicit saved floating preferences remain valid.
  Recent explicit launch requests still open the manager, stale ones are consumed.
- A native-ancestor view-promotion experiment reduced some work but worsened viewer
  timing in repeated measurements; reverted. This was internal rendering plumbing,
  not removal of native Resolve surfaces or the native-first product goal.
- Fixture timing now separates input dispatch and deferred event work, with bounded
  outer-event attribution excluding scenario preparation and nested double counts.
  The overall 16.667 ms p95 gate remains unchanged and unpassed. Do not infer smooth
  presentation merely from short dispatch time, or sum separate p95 percentiles.
- New feature branches are stacked on required integration dependencies. Inclusion
  in the manifest does not make a branch independently merge-ready for master.

Consolidated fixture receipt for `6b6cef81ab`:
`build/macos/verification/6b6cef81ab-20261018T191923Z/receipt.json`.
Eleven suites PASS; responsiveness FAILS, with zero functional failures and the
unchanged latency gate exceeded by docking commit (38.898 ms p95) and door toggle
(16.722 ms p95). Viewer command p95 is 12.832 ms in this run. The receipt records
clean source, exact commit, feature-lock hash, every exit code and complete logs.
Run `python3 -u infrastructure/macos/verify-integration.py` with Resolve closed;
it briefly creates native fixture windows. The script retains failures rather
than reporting a partial selection of passing checks as full acceptance.

## Continued implementation

The separate group-signal-lifetime correction (`03c8596`) is included. A full
native-arrangement draft run exposed a Qt accessibility crash during group recall.
The focused regression proved that broad Hide-button disconnection removed
external observers and left a deleted button's interface cached. The fix preserves
Qt's own receivers and removes only old/new group handlers. Native test builds
retain source symbols. See `macos-group-signal-lifetimes.md`; actual Resolve crash
acceptance is still open.

Inspector overflow recovery follow-up (`feature/macos-panel-overflow`, `d2a1334`):
the group fixture reproduced a 700-pixel content widget retaining a 936-pixel
control layout after native reparent/recovery. Its rightmost button therefore
remained outside the scrollable host despite scrolling to the maximum. Recovery
now invalidates/activates that content layout once at the ownership boundary.
The regression asserts layout/content geometry agreement and keeps detailed
pointer-hit diagnostics on failure. Group fixtures pass on the overflow branch
and together with the native-arrangement ownership draft. Actual Resolve input
and responsiveness acceptance remain open.

The separate `feature/macos-native-arrangement-surfaces` draft has tested native
root layout ownership, shared nested split/tab rendering and scoped panel-session
adoption, including restore, destruction, door visibility and temporary expansion.
It is not integrated or installed: complete remaining-panel preservation,
arrangement coordination, drop adapters and preset wiring are still required.

Native outer sizing captures/restores its own minimum and layout constraint,
respects surviving content hints, avoids maximize/fullscreen changes and does
not apply during temporary Unbound expansion. User movement after an automatic
resize prevents automatic geometry restoration. Transferred native content slots
are hidden reversibly while their contents float. These are implemented candidates,
not live acceptance claims.

The shared layout renderer is now used by floating groups. It can construct the
same split/tab tree in an ordinary QWidget host; connecting an original native
slot requires ownership/restoration fixture verification before production wiring.
The original-window drop adapter and preset representation are still open.

Native sizing now captures constraints before detachment can change them, and
vacated content slots restore their exact policy/minimum/visibility. Native
undock-button drags cancel pending capture on Escape/application switch and retain
the release position if host readiness arrives after release. The dependency
branches also own their respective header-control lifetime fixes, so those fixes
are not available only as a side effect of enabling drag support.

## Test candidate: recovery and destination ordering (2026-10-19)

The combined candidate now contains 21 feature/research entries. Native drop recovery and drag destination ordering remain separate branches, integrated for installation testing. Their focused fixtures pass. Dock preview latency improves in the synthetic fixture, but the responsiveness gate still fails; docking commit, toggle variability and a one-pixel tracking failure remain recorded in `macos-destination-order.md`. No installed-baseline benchmark or live Resolve acceptance is claimed. Main remains unchanged.

The `7c31c99` combined run passed 15 functional suites but its responsiveness stress fixture also failed repeated-drop geometry and one-pixel tracking checks, beyond the latency gate. Therefore the destination-order optimization was reverted from the installation candidate and its manifest entry removed; the separate published branch retains the experiment. The candidate contains 20 entries including the recovery guard. This is not accepted as a smoothness fix.

After excluding that optimization, the `fecf7fa` responsiveness run passed free-drag tracking, repeated-drop frame prediction, linked-viewer grouping and divider bounds. Its only failed assertion was the unchanged 16.667 ms p95 gate: docking commit 37.033 ms; approach 8.271 ms, divider 10.741 ms, door 15.869 ms, viewer 14.475 ms, button 1.211 ms. Report retained under `build/macos/verification/fecf7fa-responsiveness/`. The previous combined run's 15 functional suites passed; excluding the optimization returns production/tests to that verified baseline plus the separately passing recovery guard. This is an installation test candidate, not complete checklist acceptance.

## Timeline, panel commands and stacking candidate (2026-10-19)

Installed baseline remains `00bef62`; candidate is not installed. Separate branches add Timeline-owned click/drag control, persistent independent Show/Hide and Maximize/Restore bindings through Workspace UI → Unbound Panel Shortcuts, group-membership-aware stacking with native-ID diagnostics, and guards against unchanged native presentation writes. Group door implementation is retained. Three combined hidden suites pass. Signed helper and bootstrap build pass. Displayed native/group suites remain pending while Resolve is open; these are required before installation. Native keyboard-editor registration is not delivered: the bindings use an explicitly Unbound-owned editor.

Red probe connects through the scripting API but a later read-only request stalled and was stopped. The bounded disposable-project probe is prepared and its open-project mutation guard passes. No Red palette item or storage success is claimed. Independent scaling, adaptive toolbar wrapping/vertical layout, general toolbar-to-panel attachment, and the first-launch movable default layout remain open. The pinned-door visual report also remains live-pending.

Follow-up hidden verification: native workspace-menu regression passes with the Unbound shortcut editor callback and repeated wire calls. Native keymap changes now block only the colliding Unbound binding; unrelated commands keep working and removing the collision restores the stored binding. Native action inventory is collected once per validation rather than once per command. Timeline header tests cover a single-content horizontal layout and exact direction restoration. A live guarded Red request returned `not-run` because an existing project is open; no project switch/save/color write occurred. Displayed regressions remain pending with Resolve open.
