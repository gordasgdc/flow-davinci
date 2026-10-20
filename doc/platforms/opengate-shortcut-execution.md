# Native shortcut execution candidate

Branch: `fix/macos-shortcut-execution`, based directly on the accepted prototype
master. This does not depend on the header, footer or overflow changes.

The prior native menu fixture called `QAction::trigger()` directly. It verified
the handler but bypassed Qt's shortcut context. A new assigned-key event failed
before the fix and passes after retaining the original native QAction on the
main window and managed floating windows. Native action identity, QObject owner,
key sequences, shortcut context and original handler remain unchanged. Returning
to the keyboard editor restores the original menu associations; teardown removes
only associations OpenGate added. No second binding store or key listener is added.

The generated scripts previously imported `ctypes` before creating a request.
That would stop execution if Resolve's Python lacked the optional extension.
The import is now inside the optional native-notification block, after publishing
the atomic request. The filesystem watcher remains available without ctypes.
This failure mode is tested; it has not been established as the cause on this Mac.

Focused hidden fixtures pass:

- Actual assigned key through Qt's shortcut map, main and newly created floating
  windows; native reassignment takes effect and the old key stops firing.
- Unrelated-window exclusion, editor-close restoration, native ownership and
  pre-existing associations retained on teardown.
- Assigned key → native action → actual generated Python wrapper → resident
  request bridge → whitelisted command registry, exactly once. Text editing is
  guarded and duplicate notifications cannot replay a request.
- Thirty generated targets and optional-ctypes fallback.
- Existing registry/backtick regression suite.

Diagnostics distinguish `command.native.binding`, `command.native.shortcut`,
`command.native.action`, `command.workflow.request`, and `command.invoke`.
Invocation rejection now identifies no active window, a dialog/menu, text editing,
the wrong workspace, or unavailable command state. No typed text is logged.

Real Resolve native editor assignment/save/preset switching and physical key
delivery remain required after a fresh helper load. The fixture does not prove
that Resolve's internal action manager publishes every assignment as a QAction
shortcut, nor does it establish native Cocoa key-equivalent behavior. Use the
stage logs to identify that boundary if a real assignment still fails. Native
keyboard preset files were inspected read-only; none was modified.
