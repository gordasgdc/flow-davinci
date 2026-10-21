# Continuous timeline tabs — isolated experiment

Branch experiment/macos-timeline-tab-viewport. The installed 091268b scrollbar
invokes native arrow stops. Qt 5.15.2 QTabBar exposes no public pixel-offset setter;
its macOS wheelEvent ignores wheel input (local pinned Qt source qtabbar.cpp:2328).
No private Qt memory offsets are used by this experiment.

NativeTabViewport, in shared Qt core, places the original QTabBar in a public
QScrollArea. The original tab objects paint at native resolution, retain selection,
ordering and signals, and receive Qt-mapped input. The existing plus button remains
outside the viewport. An owned scrollbar with 3px top and 2px bottom spacing reserves
a separate layout row below the native row; it cannot overlay tab labels. No native
row height or native min/max constraint is changed by the controller.

Only recognized native slots with a vertical insertion row are accepted. The branch
tries this model on ordinary launch; OPENGATE_TIMELINE_TAB_SCROLLING=1 restores the
legacy arrows adapter and 0 disables added scrolling. Unsupported topology falls
back to that legacy adapter with a diagnostic. This branch must be live-tested
before merging: Resolve's custom UiTiTimelineTabBar may have parent assumptions
that ordinary QTabBar fixtures cannot prove safe.

Hidden checks pass: one-pixel movement, pixel trackpad delta, unchanged selection,
new tab extent, separate padded row, retained plus button, original parent/layout/
constraints/arrows on teardown, and unsupported slot left unchanged.
Actual Resolve interaction, tab reorder/close menus and page/session restoration
remain acceptance tests. Do not describe this as shipped smooth scrolling.
