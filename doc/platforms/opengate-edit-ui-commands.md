# Edit UI command candidates

Two assignable commands are added to Workspace UI → OpenGate Panel Shortcuts:

- Edit Timeline / Toggle Fixed Playhead
- Edit Inspector / Stabilize

These are OpenGate bindings, not registration in Resolve's native keyboard editor.
No default key mappings are assigned. They dispatch the real native QAction or
QAbstractButton click signal, respecting enabled state and active Edit page.
Ambiguous targets are refused and every invocation/unavailability is diagnosed.

Fixed Playhead requires Resolve's Timeline View Options menu to have been created
(open it once if the command reports unavailable). Only a menu also containing
Display Stacked Timelines qualifies. Stabilize requires an enabled, visible
Stabilize button beneath widgetEditInspector; reveal its native section for the
selected video clip. It does not auto-select footage or synthesize analysis.

Fixtures verify signal dispatch, native toggle state, disabled/inactive handling,
wrong-menu rejection and ambiguity. Native control discovery still requires live
Resolve acceptance. The bounded investigation did not establish invocation while
these lazy native controls have never been constructed or remain hidden.
