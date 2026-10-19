# Native keyboard-editor command targets

This candidate generates thirty individual Workflow Integration scripts: Show/Hide
and Maximize/Restore for thirteen Edit panels, hovered-panel Maximize/Restore,
Page Navigation, Fixed Playhead and Stabilize. Each script posts a single-use
command-ID request; the resident helper validates and dispatches it on the Qt GUI
thread using the existing command registry. No arbitrary code or media edits are
accepted through the request bridge. Darwin notifications wake the GUI queue;
filesystem notifications provide a fallback. Request age and outcomes are logged.

The installed c9d6b1e version exposes only the OpenGate manager launcher in Resolve's
native Keyboard Customization. Its successful enumeration is the evidence for
this route. The thirty command scripts still require fresh-start verification:
search OpenGate in native Keyboard Customization, under Workspace → Workflow
Integrations, assign a key, save/reopen the native keyboard preset and invoke it.
This does not claim a new native OpenGate category or direct internal action
registration. The user-facing panel callers stay under OpenGate UI → Show Panel
in Workspace → Edit; a second keyboard center is not the preferred UI.

Hovered-panel expansion is now a real assignable OpenGate command. Bare backtick
is its editable default, rather than an interception permanently reserved in the
manager. Clear that OpenGate binding if replacing it entirely with a native
Workflow Integration key mapping. Per-panel expansion commands use the same
controller. Stabilize and Fixed Playhead retain their native-target availability
requirements described in opengate-edit-ui-commands.md.

Verified in isolated fixtures: thirty exact atomic requests; native notification
wake-up; one-shot consumption; malformed/oversized/stale/future rejection;
registry whitelist; native-workflow dispatch through the expansion command;
default binding reassignability. Native keyboard-editor enumeration, persistence,
actual key invocation and relay latency remain live acceptance after installation.
