# Timeline undock control

Timeline has a dedicated compact undock/drag row in its own native layout, independent of the detachable toolbar or its options menu. Floating/grouped hosts retain their existing native title/group Detach surfaces rather than adding another row. The row is owned and removed by its controller.

The full Timeline subtree is transferred: when the native tools row is attached it travels with Timeline, and when it is already detached it stays in its own host. General user-defined toolbar-to-panel attachment and adaptive vertical/wrapping toolbar layout remain separate work; ordinary neighboring group leaves are not silently reclassified as attachments.

A single-content horizontal layout is temporarily leased vertically for the header and its original direction is restored on teardown. Unsupported multi-item layouts are logged rather than covered by an overlay.

Hidden tests pass control retention after toolbar separation, single dispatch, callback rebinding, attachment subtree and teardown. Native workspace fixture compiles. Actual Resolve header geometry/discoverability and displayed native transfers require verification before installation.
