# Native workflow command isolation candidate

This candidate removes menu regrouping, shortcut host associations, and editor
restoration mutations from WorkflowCommandMenu. Discovery retains native actions
in their original menus and only records bindings and activation. No preset reader
or replacement shortcut interpreter is introduced. Backtick's existing path remains.

Three installed a785d69 incidents implicate wire()+2064 at the menu-mutation source
line during native menu opening. This identifies a candidate cause, not the precise
operation or a proven lifetime mechanism. A fixture pass cannot prove live stability.

## Next live acceptance (planned restart only)

1. Confirm candidate build identity. Open native Workspace menus repeatedly and
   Keyboard Customization; inspect crash reports and command.menu.observed records.
2. In Edit with a valid source-monitor context, click its original Workflow
   Integrations command. Verify command.native.action, command.workflow.request,
   command.invoke / dispatched (or an explicit rejection), and actual UI behavior.
3. With focus outside text fields, test the saved Option+Shift+W assignment in
   the main window, then a floating surface. Capture ingress and downstream logs.
4. Test Stabilize only with its native target available; test Fixed Playhead
   separately. An unavailable target does not establish shortcut ingress failure.
5. Check reassignment and preset switching only after a successful native route.

If clicks succeed and keys do not, native keyboard dispatch remains unresolved;
this alone does not prove Resolve cannot support it. Do not infer support from
empty QAction shortcuts or documentation silence. Do not add timing deduplication
that could suppress legitimate repeats. A preset-reader fallback requires separate
format, modifier/layout, alternate-binding, conflict and preset-switch validation.

The focused menu fixture verifies unchanged ordering, ownership, associations and
empty shortcut state during repeated opening/discovery, deletion/rediscovery,
editor entry and observer teardown. The bridge fixture explicitly triggers a menu
action and executes the generated Python wrapper; it makes no native-key claim.
