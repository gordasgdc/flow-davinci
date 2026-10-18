# Native arrangement ownership draft

Branch: `feature/macos-native-arrangement-surfaces`, based on integration
`1b27a6a`. This draft is not installed or connected to production drop targets.

The goal is one native workspace whose panels can be rearranged, rather than a
second floating workspace requiring the user to hide the original background.

## Implemented foundation

- `NativePanelSlot` replaces the painted vacancy label with a zero-footprint
  restoration position. Resolve content remains owned by its panel session.
- `NativeRootLayout` parks the original page layout intact while a custom
  split/tab surface occupies the same native page and main window. Restoration
  returns the original layout object, constraints and unmanaged-child visibility.
- Embedded session adoption is scoped to descendants of the mounted arrangement
  root. Unrelated native children cannot adopt sessions. Floating group adoption
  retains its existing path.
- Root transitions and rejected restoration attempts emit diagnostics. Unexpected
  layout replacement retains both control trees for a retry; page destruction
  cleans up the parked native tree.

## Verification

`bash infrastructure/macos/test-native-arrangements.sh` exercises original layout
identity, constraints, focus, visibility, page destruction, foreign-layout
replacement/retry, the shared nested split/tab renderer and actual panel-session
transfer into/out of the native surface.

`bash infrastructure/macos/test-panel-sessions.sh` passed after the scoped session
adoption change. Fixture evidence does not establish real Resolve/GPU behavior.

## Required before exposing this in the product

1. A page arrangement coordinator with explicit ownership of every participating
   panel and preservation of unsupported or remaining native controls. Parking
   a page while only adopting some panels would hide the others; do not ship that.
2. Transactional recovery: restore the original root before returning sessions
   to their native layout slots. A pane's own vacancy must not own other panes.
3. Shared group interactions for embedded surfaces, including doors, tab tear-off,
   close/reveal callers and exact split/tab state restoration.
4. Native-cell drop registration and preview coordinates, with no whole-screen
   docking catchment or implicit conversion of a free-floating drop.
5. Persisted native placement and native/floating transfers; page/project changes,
   Source readiness, viewer commands and temporary maximize must preserve it.
6. Existing integration fixtures, responsiveness gates and fresh Resolve checks.

Native pin/chrome operations belong to floating hosts. An embedded arrangement
must never apply floating window chrome or pin priority to Resolve's main window.
