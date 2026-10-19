# Scoped native panel positions

Branch: `feature/macos-native-panel-positions`, based on `f9f4129`.
The candidate connects scoped positions to initial native docking targets. It is
not installed or accepted in actual Resolve.

`NativePanelPosition` leases exactly one supported native widget position. Its
marker belongs to the position lease independently of panel sessions. This
separation is required because docking a session currently deletes that session's
placeholder; an arrangement containing other panels cannot live in it safely.

Supported positions are Qt box/grid layouts, splitters and stacked widgets.
The lease preserves ordering, box stretch/alignment, grid spans, splitter sizes
and stack selection. Native content stays under the page's ownership while
parked. A `NativeGroupSlot` can mount an arrangement at the marker without moving
the other native panels or replacing the whole page layout.

Restoration requires the embedded group to release the marker layout and return
the original content to its parking owner first. Unexpected native layout changes
reject restoration and retain both trees for retry. Page destruction destroys
both the leased marker and parked original content. Unsupported/custom/unmanaged
positions are rejected before changing ownership.

`bash infrastructure/macos/test-native-panel-positions.sh` passes repeated cycles
for all four layout types, real button signals inside an embedded two-panel
surface, occupied-slot rejection, foreign-layout replacement/retry and page
destruction. This establishes synthetic Qt ownership behavior only.

`MacPanelSession::adoptOriginalPosition` now accepts a released position after
its embedded group has unmounted. It moves the session's restoration marker into
the real native slot, replaces its original-parent lifetime observer before the
temporary parking owner is deleted, and adopts the original geometry/visibility
and splitter/stack state. The content may remain floating and later dock through
that native position. Occupied positions reject handoff without changing session
ownership. Tests cover every supported layout type and native-parent destruction
after handoff.

The connected candidate now:

- Connects the position lease to session content adoption and original docking.
  Returning or removing the anchor panel must not destroy the other members.
- Persists anchor identity and nested placement; prepares that position before
  startup detaches its content. Reject unavailable anchors without moving panels.
- Registers only supported, visible native panel bounds. Dispatches the first drop
  to the same scoped position represented by the preview.

The extended native UI fixture passes initial destination lookup, unused-margin
rejection, first split/tab drops, doors, maximize/restore, save/reopen, independent
anchor detachment, saving while the anchor floats, and original-slot recovery.
The existing whole-page arrangement path passes in the same fixture. An optional
JSON lookup initially inserted `anchor: null` into older plans; reads now use
`value()` and preserve absent optional fields. Reopening a one-leaf scoped plan
also exposed deletion of its retained root cell; rebuild now retires generated
containers only. The failure was reproduced as a fixture crash before the fix.

Still required: real Resolve controls/page/project/viewer acceptance, complete
failure-injection and latency checks, and multiple independently anchored native
surfaces. Currently one native coordinator owns one scoped position; after the
first drop it accepts additions inside that arrangement. Other native slots are
not advertised until it is released. Returning the floating anchor to its original
native slot releases the remaining arrangement members as retained floating
panels. Dock All returns every member to its original native slot.

Qt emits native-window KVO warnings during Inspector transfers in the extended
fixture. The fixture completes, but these warnings remain an open lifecycle risk;
do not suppress them or treat this pass as live Resolve acceptance. Unsupported
native layouts remain excluded, and failed scoped placement never falls back to
a generic split around the whole native page.
