# Next-pass acceptance record

## Follow-up branch acceptance (macos-tethered-timeline-and-viewers)

Preserved baseline: 7551bce, tagged macos-live-workspace-7551bce and published to the personal fork. The rows below are requirements, not completed implementation.

- [ ] Page Navigation means the bottom Media/Cut/Edit/Fusion/Color/Fairlight/Deliver strip. Float it compactly without the tall empty area shown in the 03:04 screenshot. Keep Float/Dock discoverable without adding another full-height row. Restore its native footer slot on Dock; do not confuse it with the editing toolbar.
- [ ] Magnetic groups must support smaller panel widths, including Timeline, without increasing the outer window to satisfy inherited native workspace minimums. Scope constraint overrides to the floating/grouped lifetime and restore original constraints on native docking.
- [ ] Distinguish explicit native minimum widths, layout-derived minimum hints, and Unbound header/button minimums before changing them. Do not merely remove the outer minimum while leaving inaccessible content behind.
- [ ] Verify narrow horizontal and nested splits: divider dragging redistributes existing space, the outer frame stays fixed, hide/show cycles restore intended proportions, and native docking restores original constraints. Verify actual Resolve controls remain reachable at the supported narrow widths.
- [ ] Keep editing toolbar policy separate from Page Navigation. Earlier preference was Timeline tethered to its tools; the latest footer clarification does not authorize replacing the footer with the editing toolbar.

Initial source inspection: MacPanelSession preserves native widget constraints and transfers its layout into group cells; PanelGroup wraps those cells in additional header layouts. Floating hosts also consult minimumSizeHint during edge resizing. FooterPanel currently restores a saved rectangle or defaults to 900x100; this alone does not account for native container padding or inherited minimum heights. Diagnose the actual container before choosing compact geometry.

Candidate implementation: panelwidth.h temporarily relaxes the container's horizontal policy and explicit minimum to 240 logical pixels for Timeline, Inspector and Media Pool, without changing child control constraints or vertical limits. Group transfers retain the allowance; native docking restores the captured minimum, policy and layout constraint. Group titles no longer impose their full text width. Fixture coverage includes 20 apply/restore cycles and a 620-pixel Timeline/Inspector group with minimum-boundary redistribution and exact outer-frame retention. Actual narrow Resolve content still requires acceptance; this is not independent UI scaling.

Page Navigation candidate: replace the extra button layout row with a reserved 24-pixel ellipsis action exposing Float/Dock; derive floating height from native size hints plus chrome and discard excess saved height. Native footer content remains retained. Fixture asserts no extra layout row, compact host, and restoration. Live screenshot comparison remains pending.

Independent scaling research remains open. Qt's documented high-DPI controls apply globally or per display, not as an arbitrary per-panel slider (https://lists.qt-project.org/pipermail/development/2019-September/037434.html). A graphics-proxy scaling approach also excludes paint-on-screen/OpenGL widgets and is not recommended for high-performance use (https://doc.qt.io/qt-6/qgraphicsproxywidget.html). These are constraints on candidate approaches, not proof that Resolve-specific independent scaling is impossible. Do not present font-only changes as full-panel scaling.

Validation on this follow-up: layout-plans, hidden controls, panel groups, reliability and session suites passed. One earlier session run failed the two docking-preview assertions; a fresh process passed without a code change. Treat native-order/preview timing as an unresolved intermittent result, not a fixed runtime bug. No actual Resolve acceptance or installation is implied by these fixtures.

## Native menu and viewer follow-up

On macos-tethered-timeline-and-viewers, following candidate fe6e347:

- Native menu ownership supports the custom parented menus observed in Resolve's logs. Nearest-panel resolution handles Mixer nested inside Timeline; Timeline display preferences target Timeline rather than its optional tools row. Shared menus remain rejected when distinct panels claim them. Prepopulation/aboutToShow cover Cocoa menus; recreated controls rebind existing actions once.
- Single Viewer Mode discovery scans detached menus without relying on object-name case, strips mnemonic/shortcut decorations, deduplicates actions and preserves the user mapping. Native enabled/checkable/checked state is logged on discovery. Non-checkable commands are reported rather than guessed at.
- Empty/offline Source can detach while hidden and the native dual-viewer command is disabled. Only Source opts into that path. Native docking restores its original hidden state.
- Cinema recovery yields while Resolve deliberately hides/borrows player content, then resumes afterward. The fixture reproduces this lifecycle; actual rendered Cinema verification remains separate.
- Toolbar float controls reserve 24 pixels within the existing row rather than adding another row. Detaching Timeline tools collapses the original placeholder. Native attachment restores the original slot.
- Show-event toggle scans coalesce into one queued scan. Geometry logging coalesces bursts at 100 ms, retaining final geometry and event counts. Button/action/input logs remain immediate.

New hidden fixtures cover parented/nested/shared menus, prepopulation, controller recreation, detached Single Viewer commands and geometry logging. Displayed fixtures cover hidden empty Source, original visibility restoration, inline toolbar controls and Cinema recovery. Session preview assertions have intermittently failed; failure diagnostics now print native target visibility and pointer-hit ordering. The suite passed after the shared-menu correction; this does not establish that the intermittent native-order issue is resolved.

## Previous acceptance record

Branch at that point: macos-native-workspace-callers. Baseline at that point: 6bd0e36. Subsequent installed baseline 7551bce is preserved in the checkpoint archive and tag.
No row is complete merely because its source builds.

| Request | Implementation | Verification | Live Resolve |
| --- | --- | --- | --- |
| Drag tab to detach without button | TabTearOff module | Hidden gesture and displayed subtree transfer/remerge fixtures passed | Pending |
| Empty Source Undock without loading footage | Retain source/offline stack | Displayed empty/offline Source integration passed | Pending |
| Backtick expansion foreground priority and restoration | PanelExpansion/native priority candidate | Displayed expansion assertion passed; native priority corrected against actual pinned levels | Pending |
| Descriptive tab/split/group drop labels | DockFeedback | Geometry/wording fixture passed | Pending |
| Drag latency | DragTiming; linear native lookup; skip unused alignment collection | Lookup work reduced; per-drag timing requires fresh runtime | Pending |
| Stable open/close divider sizes | GroupVisibility per-visibility-state sizes | 20-cycle hidden fixture passed after refresh correction | Pending |
| Inspector door returns to prior Source tab | GroupVisibility tab history | Hidden fixture passed | Pending |
| Visible Hide control distinct from Detach | Group header Hide button | Displayed group fixture passed | Pending |
| Native submenu callers for all panels | Reuse existing actions; add missing callers; queue door transitions | Hidden prepopulation/identity/shortcut/no-duplicate fixture passed | Pending Cocoa verification |
| Footer discoverable native submenu | Prepopulate + aboutToShow | Hidden/earlier displayed fixture coverage | Native surface button + pre-display menu registration implemented; live Cocoa check pending |
| Detachable native toolbar | Both timeline and workspace toolbar candidates, native buttons and callers | Displayed float/dock/menu fixture passed | Pending |
| Native backdrop stays hidden, explicit recall | Explicit floating/native workspace preference; hide after successful detachment | Displayed hide/recall ownership fixture passed; workflow activation still needs live check | Open |
| Divider resizing in nested viewer layouts | Wider splitter handles and nested layout regression coverage | Displayed mouse-driven divider + 20 nested door cycles passed | User-reported issue open |
| Native dual-viewer shortcuts in custom groups | Native Single Viewer Mode action bridge; preserves assigned key | Displayed grouped Source/Program test with F9 mapping passed | Pending |
| Cinema Viewer | Priority derived from actual pinned levels; exit/focus/app-state restoration | Native-window level/key/restoration fixture passed; actual rendered viewer pending | Open |
| Arbitrary subgroup tear-off | Nested tab-section transfer with rollback and remerge | Displayed subtree detach/remerge passed | Open |

Displayed fixtures must run with Resolve closed. Do not interrupt the user's edit session or replace the loaded helper. Toolbar and empty-Source/native-viewer changes need actual native widget verification; do not label this a completed pass or install merely to close the checklist.


The main controls file now delegates preset UI/CRUD to arrangementcontrols.h. New behavior lives in focused modules (toolbars, viewer-mode command, group visibility, expansion, tab drag, feedback and timing). Historical checklist investigation items (including independent UI scaling and Stabilization section extraction) are not silently declared complete by this pass. Live rows remain open until observed in Resolve.


Lifecycle checks: both toolbar buttons and menu callers are rebound when controls are recreated; fixtures passed without duplicate buttons/actions. Cinema priority fixture measured pinned Qt controls at level 8, verifies level 9 for the viewer, verifies key ownership and original-level restoration. Focus fixtures require explicit native app/window activation after closing all fixture windows; all ripple-delete/text-entry/page-scope tests pass with that setup. This is a native-window fixture, not evidence of actual Cinema image rendering.

Preset UI and CRUD refactor passes the panel/preset regression suite. Main controller is 807 lines versus 883 before extraction. No source file absorbs the new toolbar, command, expansion, tab gesture, feedback, visibility or timing implementation wholesale.
