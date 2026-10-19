# Shortcut editor and backtick recovery

Installed de27b1c live evidence exposed two regressions that fixtures did not establish:

- The backtick command was blocked by an existing disabled native QShortcut on UiMainMenuBar. Conflict inventories now exclude disabled shortcuts and disabled/invisible actions. A pending refresh is evaluated before deciding rather than treated as a conflict by itself. Active conflicts remain protected and log the key and reason.
- Removing Workflow actions from their original menus removed the rows from native Keyboard Customization, although their QObject identities and key assignments remained. Original actions now stay under one OpenGate Commands submenu in Workflow Integrations during normal use. Before our native-editor launch, those actions return to the exact original native menu. Their identity, labels, assignments and ownership are retained. After the native editor hides, grouping resumes. No extra Commands submenu is added to OpenGate UI.

Native menu-click/keyboard activation hooks also prepare the original menu before dispatch where Qt exposes those events. Native macOS menu activation and new submenu discovery still require live verification; they are not established by fixtures. OpenGate Panel Shortcuts remains the single OpenGate UI entry and uses the explicit preparation route.

Logs: command.binding.conflict includes key/reason; keyboard.commands.restored records the number restored before enumeration; keyboard.launch.request and keyboard.native.search record native launch/filtering; keyboard.native.command-list reads only OpenGate command names from native displayed models with bounded traversal; command.menu.grouped records regrouping; existing input, native-action, workflow relay and command dispatch logs remain intact. No preset writes or separate editor are added.

Fixtures cover disabled backtick dispatch, enabled conflict protection, original menu restoration before the native launch callback, grouping suppression while editing, regrouping on editor close, action identity, ownership, assignments, original signal delivery and idempotent grouping. Live acceptance remains pending.
