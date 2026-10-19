# Drag destination ordering

Keep current candidate windows and non-dockable occluders under the pointer. Query WindowServer IDs without collecting per-window dictionaries. A drag-scoped order snapshot is invalidated by Qt ordering, visibility and lifetime events, candidate changes, and Unbound native stacking operations. Release always revalidates current native order before committing. No timer-based stale ordering is used.

Panel-session fixtures pass, including actual ID order versus the full WindowServer list, snapshot invalidation after native and Qt ordering changes, previews and occlusion.

Two responsiveness runs measured docking-approach p95 at 1.972/2.039 ms versus 17.466 ms for the previous combined candidate. These are fixture measurements, not a comparison to the installed baseline or proof of live Resolve responsiveness. Dock commits remain over the unchanged 16.667 ms budget (43.015/27.844 ms); door/viewer results vary and remain unaccepted. One run also reported a one-pixel free-drag tracking discrepancy; the repeat passed tracking. Preserve this as unresolved evidence rather than hiding it with a tolerance.
