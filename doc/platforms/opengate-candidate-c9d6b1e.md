# OpenGate test candidate c9d6b1e

Installed 2026-10-19; Resolve left closed. Integration branch
`macos-panel-discovery-and-layout`; no merge to main/master.

The previous installed helper, launcher, workflow entry and latest workspace,
arrangement and shortcut settings are archived under
`build/macos/checkpoints/pre-c9d6b1ef-20261019T121831Z`. Stable plugin filenames and
Unbound settings paths remain for compatibility. Product labels are OpenGate by
CineWaltz; Wolfgang Woehl's foundation credit and MIT license remain.

## Included feature branches

- `feature/macos-independent-panel-metrics` (7081d1d): experimental 70–150% native
  font/control/icon/layout metrics; independent saved per-panel values; − / UI / +
  in group headers and manager rows, percentages in panel options. Native docking
  restores original values. Custom-painted controls are not claimed fully scaled.
- `feature/macos-unified-native-workspace` (e7031d2): collapse known empty native
  layout branches; automatically retire an empty Edit shell and restore it when
  native contents return or the page changes. Remaining native content stays in
  the real original window. No separate duplicate UI clone is maintained.
- `feature/macos-edit-ui-commands` (bf82958): assignable Fixed Playhead and Stabilize
  routes in OpenGate Panel Shortcuts. Fixed Playhead may require opening Timeline
  View Options once. Stabilize requires its enabled native Inspector button to be
  visible. Missing or ambiguous targets log a reason and do not dispatch.

## User acceptance sequence

1. Open Resolve and confirm the latest arrangement returns. Grouping, doors,
   dividers, pinning and Source/Program viewer modes should retain prior behavior.
2. Use a panel's − / UI / + controls, then a different panel's. Check text, icons,
   sliders, menus and actual pointer hits; check GPU viewers/playback separately.
   Dock back into Native Edit UI and confirm original native metrics return.
3. Pull native panels out. Remaining native content should retain a usable real
   surface; the empty shell should not require the manual background checkbox.
   Reopen a native panel, change pages, and use Native Edit UI recovery.
4. Bind the two Edit commands through Workspace UI → OpenGate Panel Shortcuts.
   They are not registered inside Resolve's own keyboard editor. Test Fixed
   Playhead on/off and Stabilize with selected video footage.
5. Quit/reopen Resolve and check the modified arrangement and independent panel
   metric preferences, including tabbed/hidden panels.

## Verification and open limits

All **24 functional suites passed** on clean source c9d6b1e, including two-process
restart, native recovery, independent scale restoration across real widget and
content-layout session transfers, group ownership and both native UI signal
routes. Fixture receipt:
`build/macos/verification/c9d6b1efde-functional-20261019T121433Z/receipt.json`.

Helper, bootstrap handshake and launcher/workflow tests passed. Installed helper
and bootstrap signatures verified; SHA-256 values match the built artifacts.
Local installation receipt: `build/macos/install-receipt.json`.

This is ready for live testing, not a claim that all requested work is finished.
Actual Resolve custom painting, scaling sharpness/GPU playback, native layout
wrapper coverage, Cinema Viewer and startup visuals remain user acceptance.
The responsiveness gate previously failed and was explicitly excluded from this
functional pass; no new smoothness claim is made. Custom clip colors remain
deferred. Native keyboard-editor registration remains unverified.
