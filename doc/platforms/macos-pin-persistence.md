# Pin persistence

Pin-only edits now schedule the same bounded autosave path as other layout changes. Standalone sessions notify arrangement changes when `unboundPinned` changes; groups schedule their existing arrangement timer. This does not invoke window rescue, move a window or change pin/stacking semantics.

Page Navigation snapshots now include pin state and restore it when floating again. Old snapshots with no pin field continue to default to unpinned. The existing native group fixture now checks footer snapshot/restore pin state as well as the ordinary navigation visibility/layout behavior. It compiles; displayed execution is pending while Resolve is open.

This fixes a missing persistence field and save notification. It does not implement Page Navigation attachment to Timeline or establish live Resolve restart acceptance.
