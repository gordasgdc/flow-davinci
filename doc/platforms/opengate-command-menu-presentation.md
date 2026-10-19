# Command menu presentation candidate

Resolve registers the generated Workflow Integration scripts in its native keyboard editor, but also exposes each script in its Workflow Integrations menu. This candidate relocates only original actions with native `workspaceWIPlugins_` IDs and `OpenGate - ` labels to OpenGate UI → Commands. It retains the original QAction, QObject owner, text, shortcut, enabled state and signal connections. Other integrations and the manager launcher remain untouched. It does not delete scripts or alter the keyboard preset.

Hidden workflow-command-menu tests pass for identity, ownership, assignment retention, signal delivery, repeated wiring and native reinsertion. Resolve keyboard editor discovery and actual assigned-key dispatch after relocation remain live acceptance requirements. This is not installed.
