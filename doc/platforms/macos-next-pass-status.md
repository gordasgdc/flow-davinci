# Next-pass acceptance record

Branch: macos-native-workspace-callers. Installed baseline remains 6bd0e36.
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
