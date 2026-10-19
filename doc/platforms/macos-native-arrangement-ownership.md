# Native arrangement ownership draft

Branch: `feature/macos-native-arrangement-surfaces`, based on integration
`1b27a6a`. This candidate now has controller/menu/save/drop bindings, but is not
installed or verified in actual Resolve.

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
- `NativeArrangement` coordinates multiple session leases, an explicit native
  remainder leaf, nested subtree insertion, individual detachment, Inspector door
  selection/return and root-first recovery. The remaining native controls keep
  their original layout and signal connections in the same main window.
- Sessions detached after rearrangement remember their logical original page,
  rather than the temporary remainder container. Returning one panel does not
  require tearing down the rest of the arrangement.
- Native drop registration accepts only real arrangement cells. Lookup respects
  native window order and never falls back to the native window's outer edges or
  unused margins. Registered native cells now dispatch floating-panel/group drops
  through the same controller route as native menu commands.
- Floating/native groups share `panelLayoutSnapshot`; covered-tab recovery no
  longer re-shows a session through an inactive tab. Transaction callbacks cannot
  reenter an unfinished coordinator operation.
- `NativeArrangementPlan` validates page-scoped saved trees and per-panel hidden
  states before adoption. Saved-plan initialization preserves selected tabs;
  divider snapshots retain usable proportions while a split panel is closed.
  Root recovery applies managed-control visibility immediately, so a subsequent
  reopen command does not depend on Qt's deferred show event.
- `NativeWorkspaceBinding` connects the coordinator to PanelControls without
  putting its ownership/persistence logic in the controls window. Native options
  menus expose **Dock in Native Workspace** for floating panels/groups. Existing
  native header Undock glyphs float embedded panels; Workspace panel callers and
  manager Show/Hide use the same native door state.
- Normal workspace saving/preset capture includes the native plan; startup waits
  until its session preparation finishes before adopting those sessions. Native
  and floating saved groups cannot claim the same panel. Full native recovery and
  helper shutdown restore the original root before docking sessions.
- Native tab drops select the new tab. Tab removal retains a valid selected panel
  as indexes shift. Empty tab/split branches collapse and can reopen from Workspace
  callers; expanding a covered tab restores prior tab selection and outer frame.
  Maximize completion now waits for readiness instead of a fixed 100 ms delay.

## Verification

`bash infrastructure/macos/test-native-arrangements.sh` exercises original layout
identity, constraints, focus, visibility, page destruction, foreign-layout
replacement/retry, the shared nested split/tab renderer and actual panel-session
transfer into/out of the native surface.
It also exercises multi-panel subtree addition, rejected targets without mutation,
native remainder preservation, logical parent restoration, project replacement,
door tab return, scoped drop bounds and operation diagnostics.
The saved-plan round trip now includes a closed split panel and immediate reuse of
the native slots. Headless layout-plan tests reject malformed geometry, unknown
panels, page mismatches and bad visibility without changing the last valid plan.
`bash infrastructure/macos/test-native-workspace-ui.sh` passes the actual controls
path: native options-menu docking, floating Inspector drop into Media Pool tabs,
Workspace Inspector door commands, temporary maximize, Hide All branch collapse,
normal workspace saving, helper reopen and native header Undock. This fixture
tracks moved native controls rather than their retained restoration shells.
It also transfers an entire floating tab group through that menu and asserts
that neither member's standalone host receives an intermediate Show event.
Transfers and Dock All defer member-host visibility until the destination owns
the content; explicit Undock still reveals the requested standalone window.

The full 13-suite run at `19bd6a8` was incomplete: Resolve opened after the five
headless suites passed. Eight native-window suites compiled but were NOT RUN by
their safety checks. Receipt:
`build/macos/verification/19bd6a8177-20261019T004732Z/receipt.json`.
The signed widget library and IO helper built at that commit; nothing was
installed. Subsequent saved-plan changes passed their individual native fixture
after Resolve closed; consolidated verification must be rerun on the new commit.

`bash infrastructure/macos/test-panel-sessions.sh` passed after the scoped session
adoption change. Fixture evidence does not establish real Resolve/GPU behavior.

## Required before exposing this in the product

1. Add initial native drop destinations before a custom arrangement exists.
   Preserve the user's intended location within the native layout;
   a generic split around the entire native remainder is not sufficient.
2. Verify remaining/unsupported Resolve controls and project/page ownership live.
   Fixtures establish synthetic ownership behavior only.
3. Complete shared group interactions for embedded surfaces, including tab tear-off,
   close/reveal callers and exact split/tab state restoration.
4. Integrate native-cell previews and drop dispatch, with no implicit conversion
   of a free-floating drop and complete rollback on failed transfer.
5. Persisted native placement and native/floating transfers; page/project changes,
   Source readiness, viewer commands and temporary maximize must preserve it.
6. Existing integration fixtures, responsiveness gates and fresh Resolve checks.

Native pin/chrome operations belong to floating hosts. An embedded arrangement
must never apply floating window chrome or pin priority to Resolve's main window.
