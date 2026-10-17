# Next-pass acceptance record

Branch: macos-native-workspace-callers. Installed baseline remains 6bd0e36.
No row is complete merely because its source builds.

| Request | Implementation | Verification | Live Resolve |
| --- | --- | --- | --- |
| Drag tab to detach without button | TabTearOff module | Hidden gesture fixture passed; integration fixture pending | Pending |
| Empty Source Undock without loading footage | Retain source/offline stack | Integration fixture written and compiled | Pending |
| Backtick expansion foreground priority and restoration | PanelExpansion/native priority candidate | Integration assertion compiled | Pending |
| Descriptive tab/split/group drop labels | DockFeedback | Geometry/wording fixture passed | Pending |
| Drag latency | DragTiming; linear native lookup; skip unused alignment collection | Compiles; timings require new runtime | Pending |
| Stable open/close divider sizes | GroupVisibility per-visibility-state sizes | 20-cycle hidden fixture passed after refresh correction | Pending |
| Inspector door returns to prior Source tab | GroupVisibility tab history | Hidden fixture passed | Pending |
| Visible Hide control distinct from Detach | Group header Hide button | Group fixture compiles | Pending |
| Native submenu callers for all panels | Reuse existing actions; add missing callers; queue door transitions | Hidden prepopulation/identity/shortcut/no-duplicate fixture passed | Pending Cocoa verification |
| Footer discoverable native submenu | Prepopulate + aboutToShow | Hidden/earlier displayed fixture coverage | User reports installed entry absent; not closed |
| Detachable native toolbar | Not implemented; row clarification requested | None | Pending |
| Native backdrop stays hidden, explicit recall | Existing hide mechanism and new caller only | Earlier fixture insufficient for workflow behavior | Open |
| Divider resizing in nested viewer layouts | Not yet established | Full interaction fixture pending | User-reported issue open |
| Native dual-viewer shortcuts in custom groups | Not implemented/verified | None | Open |
| Cinema Viewer | Previous foreground candidate only | Native command creation confirmed, not successful presentation | Open |
| Arbitrary subgroup tear-off | Not implemented | Recursive tree merges covered, subgroup UI not covered | Open |

Displayed fixtures must run with Resolve closed. Do not interrupt the user's edit session or replace the loaded helper. Toolbar and empty-Source/native-viewer changes need actual native widget verification; do not label this a completed pass or install merely to close the checklist.
