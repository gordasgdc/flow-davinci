# Docking interaction research and implementation contract

Research date: 2026-10-17. Branch: macos-workspace-drag-architecture.
This document records a design and acceptance criteria, not a completed fix.

## Sources

- Adobe, Dock, group, or undock panels: https://helpx.adobe.com/premiere/desktop/get-started/tour-the-workspace/dock-group-undock-panels.html
  Edge destinations split beside panels/groups/windows; middle and tab destinations group panels. Floating windows also support arrangements.
- KDAB KDDockWidgets: https://github.com/KDAB/KDDockWidgets
  Supports nested floating groups and docking across windows.
- KDAB component boundaries: https://kdab.github.io/KDDockWidgets/custom_styling.html
  Separates floating windows, groups, tab bars, separators, drop indicators and placement previews.
- Qt Advanced Docking System guide: https://github.com/githubuser0xFFFF/Qt-Advanced-Docking-System/blob/master/doc/user-guide.md
  Separates drag preview configuration, containers, tabs, splitters, focus and visibility behavior.

Adobe documents behavior, not its private hit-testing algorithm or pixel thresholds. The following geometry and state rules are Unbound design decisions, to be verified with interaction tests.

## Confirmed current implementation problems

- floatinghost.h restarts an active drag in beginPointerDrag; runtime traces show restarts during a single gesture.
- Compass dwell restarts while pointer movement exceeds 24 logical pixels. Discovery depends on slowing/stopping.
- Group-cell content sits inside a dedicated leaf wrapper with a drag header. Source inspection and the group fixture establish that this wrapper is the correct full-leaf destination; using only content bounds excludes its header. The earlier parent-bounds suspicion was not confirmed.
- Tiny local targets, an outer target rule, and a delayed compass compete in one selection loop.
- Preview selection is cleared and rediscovered on every update; no explicit enter/retain/leave policy.
- Native window ordering and overlays are involved in destination discovery; preview overlays must never become destinations or obscure the destination search.

## Interaction contract

1. Dragging a panel tab/header moves that panel; dragging a group window title moves the group. Clicking controls never starts docking.
2. Exactly one drag session owns the gesture, regardless of native or custom event delivery. Release commits at most once; Escape cancels and restores.
3. Resolve the frontmost eligible container beneath the pointer, ignoring the dragged subtree and Unbound overlays. Never dock to empty desktop, display edges or hidden native placeholders.
4. Resolve the actual visible leaf/tab stack using its own global bounds. The leaf and whole container are distinct destinations.
5. Use one regional policy: bounded edge regions split; center/tab regions create tabs. Highlight only the selected operation, with a placement preview. Do not require aiming at an isolated tiny icon or waiting stationary for a compass.
6. Edge region widths must be bounded in logical pixels and proportional on small surfaces. Regions cannot overlap ambiguously; center remains usable. Final values require testing at small/large sizes and mixed display scales.
7. Selecting the outside border of a group explicitly splits the whole group; an internal panel edge splits that panel. Preview extent makes the scope clear.
8. Retain the current operation through small pointer jitter using an explicit exit margin. Leaving the destination clears it immediately enough to permit free floating; no stale drop selection after leaving.
9. Region highlights and previews are visual only. Layout changes occur once on release, using the exact selected destination and operation. Invalidated destinations cancel safely.
10. Option bypass leaves a floating window without docking. Alignment assistance is separate from layout docking and cannot create groups.
11. Joining groups and splitting/tabbing panels preserve native widget identity, focus, hidden state and playback ownership. No duplicate panel content.

## Module boundaries

- Drag session: gesture identity, event ownership, threshold, cancellation and one release.
- Destination resolver: window ordering, native coordinate conversion and leaf/container lookup.
- Drop policy: pure geometry and semantic operation selection, including retention margins.
- Drop overlay: renders the policy result; does not select targets or mutate layouts.
- Layout transaction: validates and applies one split/tab/move; preserves or restores state.
- Panel registry/commands: shared native/menu/shortcut/launcher visibility and remembered placement.
- Native workspace adapter: original slots, native backdrop, footer/page navigation and viewer lifecycle.
- Window chrome: traffic lights, reserved title space and pin layout.

Existing split/tab tree code should be validated and reused where correct. Merely moving the existing mixed logic into smaller files is not a redesign. Evaluate established docking engines for compatibility with Resolve Qt 5.15.2, widget ownership, native rendering, runtime loading and licensing before deciding whether to adopt one. Do not inject another Qt runtime.

## Verification gates

Pending, not passed:
- Trace complete pointer paths from outside a window into every split edge and tab area, and back out to free floating.
- Traverse adjacent leaves and internal dividers in nested layouts; selected bounds must identify the actual leaf.
- Drop whole groups on other groups; verify intended subtree and selected tabs.
- Jitter near boundaries, move quickly, release on first entry, cancel, bypass and deliver duplicate native/custom release events.
- Verify no overlays/capture remain after release; ordinary buttons and native dividers still work.
- Verify exact preview versus resulting layout, small windows and multi-display coordinate scales.
- Live Resolve: Source callable without native UI; native backdrop remains hidden through Workflow Integrations; footer callable from Workspace submenu; panel toggles restore location.
- Live Resolve: Cinema Viewer visible and interactive above floating/pinned groups; exit restores layout and focus. Backtick expansion remains a separate command.
- Verify pin/title never overlap across standard/compact chrome, controls window, groups and narrow widths.

Fixture passes do not close live Resolve bugs. Preserve the prior branch and binary before installation. No installation is authorized by this research document alone beyond the existing user build/install requests.

## Source inspection and first test results

Inspected ADS DockOverlay.cpp (dropAreaUnderCursor/showOverlay) and DockContainerWidget.cpp (dropFloatingWidget/dropIntoContainer): explicit panel-area versus container scope, tracked overlay target, separate selection and final insertion. These are architectural observations; no ADS source was copied.

- https://github.com/githubuser0xFFFF/Qt-Advanced-Docking-System/blob/master/src/DockOverlay.cpp
- https://github.com/githubuser0xFFFF/Qt-Advanced-Docking-System/blob/master/src/DockContainerWidget.cpp

Implemented droppolicy.h as a standalone, GUI-independent candidate policy. It uses bounded edge regions, a center tab region and a clipped retention margin. It is NOT yet connected to live drag handling; installed behavior remains unchanged.

Passed test-layout-plans.sh: six four-panel arrangements merged recursively with subtree preservation; nested insertion and exact structural restoration after tear-off; all 24 panels removed without invalid residual nodes. Passed regional coverage and preview containment checks across small/large windows, negative screen coordinates, boundary jitter and clearing on destination exit. These tests cover layout and geometry, not native pointer ownership, subtree tear-off UI, focus, playback or Cinema Viewer. Those remain pending.

N means recursive groups rather than a two-group restriction, within validated resource bounds (currently 32 panels and depth guards). Do not describe it as unlimited. A whole split subtree becoming one tab versus distributing its leaves across tabs requires explicit semantics; preserve its internal arrangement rather than silently flattening it.

## Implementation checkpoint (not installed)

- Connected regional policy to MacFloatingHost; removed live compass dwell and competing anchor selection.
- Extracted dockdestination.h for native window/leaf selection and dockoverlay.h for the legacy compass renderer used by its fixture. floatinghost.h reduced from 404 to 336 lines.
- Duplicate starts are ignored while a drag is active; duplicate completion is ignored after release.
- Leaf wrapper (including header) retained as the full target; exact content cell retained as semantic destination.
- Selected operation is labeled Tab/Left/Right/Above/Below and previewed; no desktop/display targets in ordinary docking.
- Native title is rendered in a reserved rectangle after the fourth traffic light, updated on controls resize/title change.
- Added cinemaviewer.h: normal hierarchy yields while native UiCinemaViewerDialog is visible; on show requests native viewer activation. Live behavior NOT verified, including pinned-controls level conflicts.
- Added viewerpreparation.h: hidden Source requests enabled/checked native Single Viewer Mode action to prepare dual viewers; disabled/missing actions are not forced. Logs exact discovery/visibility preconditions.

Passed: layout-plan tests; displayed panel-session tests (including duplicate start); displayed nested group tests; reliability tests; hidden controls/native chrome and mock source-preparation tests. The first group test failed when content-only bounds excluded headers; corrected to dedicated wrapper and reran successfully. Hidden test initially failed from sandbox WindowServer denial, then passed with permitted access.

Still pending: explicit subtree tear-off UI; full shared panel command registry; native submenu/footer discovery and integration; backdrop lifecycle; live Source preparation; live Cinema Viewer and pinned controls; repeat complete pointer-path tests for the newly extracted resolver. No install or claim of overall completion.

### Footer checkpoint

Widget captures identify native m_PageSwitcherContainer (UiPageSwitcherContainerImp) under bigBossWidget, and Workspace submenu workspaceShowPanel. Added footerpanel.h with retained-widget float/dock, geometry/visibility persistence, page-independent native owner and a shared submenu caller. Controls expose Float/Dock Page Navigation; Dock All docks the footer too. Footer is currently a global independent host, not yet mergeable into Edit-page groups. Fixture passed float, page hide, submenu hide/reopen, original-slot restoration and saved-state restoration. Live Resolve page navigation still needs verification.
