# Restore the last view on reopening

This is distinct from recalling a named preset: changes made after choosing a preset must remain on restart. First launch uses the familiar default; later launches restore the latest view. Original Resolve UI remains a separate recovery option.

Required live acceptance includes window geometry/display, nested groups/tabs, selected tabs, hidden doors, pinning, divider proportions, selected workspace identity, toolbar/footer attachments and visibility. No duplicate original shell should unexpectedly replace that view. Per-panel scaling settings must join this contract when scaling exists.

`test-workspace-reopen.sh` adds a **two-process** Qt fixture: the first creates a selected but empty named preset, modifies the real controller arrangement into a pinned split/tab group plus standalone panels and a hidden Media Pool door, then exits. A second process starts automatically from those settings and compares the restored group tree/frame/pin/dividers, panel geometry, hidden door and selected workspace. It deliberately does not invoke Restore Preset. Temporary files, generated widget contents and diagnostics are isolated.

The fixture compiles. It must run only while Resolve is closed because it displays native hosts. Passing it will not establish actual Resolve/GPU restart acceptance; that remains a fresh installed-helper check. Toolbar/footer attachment and independent scaling are not yet implemented and are not claimed covered by this fixture.
