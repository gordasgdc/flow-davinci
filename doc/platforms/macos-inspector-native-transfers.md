# Inspector native ownership transfer

Branch: `fix/macos-inspector-native-transfers`, based on `f930871`.

The extended native UI fixture emitted six Cocoa warnings about recreating an
NSWindow with active KVO observers. A test-only message handler retained the
warnings and captured stacks in `build/macos/native-warning-trace.log`. The
stack identified `PanelOverflow::releaseInto` -> Qt widget `setParent` -> Cocoa
native-window recreation during normal docking/native recovery.

Qt 5.15.2's `QScrollArea::takeWidget()` clears its widget pointer and calls
`setParent(nullptr)`. For the retained native Inspector this temporary orphan
became a top-level native window before the code reparented it to its destination.

The candidate releases Inspector directly to the destination parent, removes the
scroll area's event filter, and destroys the retired wrapper synchronously.
Docking commands are the sole caller and run outside scroll-area event dispatch;
the original control subtree survives the wrapper. Qt's scroll-area destructor
does not explicitly delete its tracked widget; QObject child ownership destroys
only children still parented to the retiring wrapper. No Qt private state is
modified, native flags disabled, or warning suppressed.

The native UI fixture always counts these warnings and fails if any recur.
`UNBOUND_TRACE_NATIVE_WARNINGS=1` additionally prints warning stacks. The previous
message handler still receives each message, or its full text is printed when
there was no previous handler. This handler is fixture-only.

Focused native UI and panel-group suites pass with zero recreation warnings,
including scoped and whole-page save/reopen, Inspector overflow/hit targets,
group transfer, maximize/restore and native recovery. Real Resolve native-control
ownership, playback and performance remain acceptance requirements. This branch
is not installed.
