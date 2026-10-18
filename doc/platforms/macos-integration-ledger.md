# macOS integration ledger — October 18, 2026

Integration: `macos-panel-discovery-and-layout`. Preserved public baseline:
`fbe835c` / `macos-responsive-group-transfers`. No changes to master or upstream.
The new integration has NOT been installed. Resolve is running; native-window
fixtures have intentionally not been executed during this session.

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
| Containment/width | feature/macos-panel-containment | Debounced boundary audit; owned group cells get native clipping NSViews | Reproduce actual Resolve leakage and prove fix; scrolling at narrow widths; divider/resize/input and GPU/playback checks |
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

- Discovery workspace menu and group fixtures compile with warnings-as-errors.
- Production helper compiles after discovery, pointer, capability and boundary
  diagnostic integration; native clip integration compiles; final rebuild and native runtime verification remain pending.
- No new native fixture pass or live acceptance claimed. No new custom-color,
  native-shortcut registration, scaling or native-root rearrangement claim.

## Continued implementation

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
