# Saved-preset shortcut adapter: acceptance candidate

Opt in with `OPENGATE_NATIVE_PRESET_SHORTCUTS=1` in the environment of a freshly
launched Resolve. This is intentionally not enabled by default or installed.
Resolve's keyboard editor remains the assignment surface; preferences are read
only. No preset parser can establish which preset Resolve currently holds in
memory if its preference file has not been flushed. The supported scripting API
confirmed the persisted name for this session; live switching/save timing remains
an acceptance gate. Built-in presets absent from the custom file have no mirrored
assignments. Malformed/unknown data never retain previous keys.

The adapter decodes observed version-1 Qt records, supports alternate single-key
combinations, and preserves conflicts with non-OpenGate commands. Multi-key chords
are not executed. The known FairlightTimeline owner is out of scope on Edit;
other collisions fail closed. In the current preset, Option+Shift+S is eligible,
Option+Shift+F is eligible on Edit, and Option+Shift+W conflicts with native
`viewReferenceWipeModeCycle` and remains blocked. Reassign W to an unused chord
for acceptance instead of silently stealing the native viewer binding.

The input filter claims ShortcutOverride/KeyPress, suppresses repeat, checks the
active window/page/text context and revalidates before dispatching the existing
registry. It does not call the Python wrapper, mutate native QActions or regroup
menus. Logs identify the route as `resolve-preset-input`. Legacy custom bindings
are ignored while the adapter is enabled. The default backtick remains only when
there is no native hovered-panel assignment. If a future Resolve version dispatches
outside Qt's event path, duplicate execution must be tested; no timing heuristic
is claimed to prevent that.

Verification completed: actual Qt serialization decode and alternatives; absent
preset; corrupt XML; conflicts; main/floating key dispatch exactly once; text/page
guards; atomic replacement; preset switch and parse-error recovery; existing command
suite; seven independent Python format tests. These fixtures do not establish live
Resolve behavior, keyboard layouts beyond the observed US setup, or save timing.

Next live acceptance: selected eligible video clip with native Stabilize button
available; Option+Shift+S; confirm one native analysis invocation and one registry
entry. Then exercise reassignment, preset switching, native menus and floating
focus. Do not enable this adapter in a release until those gates pass.
