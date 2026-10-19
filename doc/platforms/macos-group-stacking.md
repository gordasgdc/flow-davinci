# Group-aware stacking candidate

The live `00bef62` trace shows Inspector opening above the Timeline group, then `window.order` moving it below that group when Timeline gains focus. Group names retain their creation labels and do not reliably describe current membership.

Use actual Timeline membership for the workspace tier, ordinary viewer/groups above that, separate Inspector above viewers and workspace groups (other standalone viewer/sidebar ordering is retained), and explicit pinned hosts above ordinary hosts. Preserve native focus and Cinema/expansion overrides. Ordering diagnostics include native IDs and tiers to distinguish identically named hosts.

Hidden policy tests pass. Added repeated pinned-tab door/outer-frame assertions compile, but require the displayed group suite with Resolve closed. The reported pinned Source/Media Pool visual issue is not claimed fixed: the live log records visibility changes while pinned, and more precise native-order evidence is needed.
