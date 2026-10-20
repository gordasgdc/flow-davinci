# Keyboard command readability candidate

User preference: wrap full command names rather than truncate them or require
horizontal scrolling. Keep native registered labels and saved key assignments.

`KeyboardCommandReadability` attaches only when the native commands pane has one
unambiguous multi-column QTreeView. It disables text elision, enables word wrap
and non-uniform row heights, and provides a command-column size-hint adapter.
Painting, editors, model data and event behavior delegate to the existing native
delegate; the keystroke column, native model, command names and column widths are
not replaced. Full command text is available on hover. Model and width changes
coalesce into row layout; stable geometry does not repeatedly relayout.

The fixture renders a long name across multiple lines, verifies greater row
height, non-overlapping adjacent rows, narrower-column reflow, identical native
names/bindings/selection, unchanged native delegate for other columns, one adapter
on repeated wiring, policy restoration on explicit removal and safe view teardown.
A first teardown test caught access to an already destroyed viewport; the fix
clears retained view access when its viewport/header begins destruction. The
regression now passes. The rendered fixture was visually inspected.

This is not installed. Resolve's actual custom command delegate, conflict controls
and native editor lifecycle still need live acceptance before integration.
