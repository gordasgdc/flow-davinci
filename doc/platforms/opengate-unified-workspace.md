# Unified native workspace candidate

`feature/macos-unified-native-workspace` keeps Resolve's real native contents as
the familiar starting layout, with the existing undock controls. It does not
maintain a second clone. Saved modified arrangements still restore on reopening.

Empty native branches created by detached slots collapse automatically. Only
known Qt layout containers on those vacancy paths qualify; custom-painted native
containers and surviving native controls are not assumed empty. Native toggles
can reopen a collapsed branch. Original hidden state and constraints restore.

When the last visible registered editing surface leaves the native page, the
empty main shell hides automatically, leaving the floating panels. Returning a
native panel, choosing Native Edit UI, or changing page restores the shell. A
custom arrangement mounted inside the native window is never treated as empty.
The existing manual hide-background override remains available, but is not
required for this automatic behavior. Unchanged presentation avoids native
window reordering. Diagnostics identify each branch and shell transition.

This is a candidate for live Resolve acceptance. Unknown custom native layout
wrappers may still retain space; the implementation deliberately preserves those
rather than hiding potentially useful controls. First-launch appearance,
project/page changes, native buttons and the original UI recovery must be tested
in Resolve. Fixture results do not establish that visual acceptance.
