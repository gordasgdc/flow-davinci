# Independent panel shortcuts

Workspace UI → Unbound Panel Shortcuts opens an Unbound-owned editor. Assign Show / Hide and Maximize / Restore independently for each recognized panel, including Source and Program. Defaults are empty; native bindings are not overwritten. Backtick's existing hovered-panel behavior remains reserved. Bindings persist separately in `~/Library/Application Support/Unbound/Shortcuts.json`, unaffected by workspace recall or reset.

Panel callers reuse the existing queued door path for managed panels, preserving nested tabs and group geometry. Calling a panel still in the native workspace requests its existing undock path. Native Single Viewer remains separate. Key dispatch yields to text editing, dialogs, popups and other page/application contexts. Repeats do not toggle repeatedly. Duplicate/native conflicts and multi-chord assignments are rejected; a native action change refreshes conflict detection outside the key hot path. Dispatch and panel transitions are logged separately.

Read-only native tracing found UiActionManager, ActionDefinitions, scoped connectors and UiActionsEditorController. It did not establish a callable, persistent native registration interface. These bindings do **not** appear in Resolve's native keyboard editor.

Hidden fixture passes independent Source/Program dispatch, persistence, rejection without overwriting existing assignments, native conflicts, typing, page scope, repeats and editor discovery. Combined native-window/group command acceptance remains pending.
