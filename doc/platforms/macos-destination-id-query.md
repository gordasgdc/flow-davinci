# Destination order query experiment

The drag resolver needs the current front-to-back order of known Qt windows under the pointer, including non-dockable occluders. Its previous query materializes a metadata dictionary for every on-screen system window, then filters by process.

This branch adds an ID-only native query using Apple's `CGWindowListCreate` and filters the returned current IDs against the resolver's existing candidates. No metadata, image capture, stale order cache, changed drop regions or skipped pointer updates are introduced. The reference metadata query remains available for diagnostics and test comparison. CoreGraphics returns raw window IDs in this array, not CFNumber objects.

Reference: https://developer.apple.com/documentation/coregraphics/cgwindowlistcreate and the installed SDK's CoreGraphics/CGWindow.h.

Panel-session and responsiveness fixtures compile. Added displayed assertions compare candidate order against the metadata reference after establishing native presentation. Execution, occlusion acceptance and timing comparison require Resolve closed. This experiment is not yet integrated or installed; no speed improvement is claimed.
