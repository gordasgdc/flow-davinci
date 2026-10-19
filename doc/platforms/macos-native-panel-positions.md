# Scoped native panel positions

Branch: `feature/macos-native-panel-positions`, based on `f9f4129`.
This is an ownership component, not exposed initial docking targets.

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
after handoff. This is session plumbing, not connected first-drop UI yet.

Required integration before exposing initial drop targets:

- Connect the position lease to session content adoption and original docking.
  Returning or removing the anchor panel must not destroy the other members.
- Persist anchor identity and nested placement; prepare that position before
  startup detaches its content. Reject unavailable anchors without moving panels.
- Register only supported, visible native panel bounds. Dispatch the first drop
  to the same scoped position represented by the preview.
- Verify native toggles, doors, tear-off, native recovery, helper/project lifetime,
  and real Resolve custom layouts. Never replace a failed scoped placement with a
  generic split around the whole native page.
