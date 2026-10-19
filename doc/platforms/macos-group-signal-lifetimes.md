# Group signal lifetime correction

Branch: `feature/macos-group-signal-lifetimes`.

A floating-group fixture crashed while Qt's native style queried a button
accessibility interface during group recall. The full run retained this failure;
it was not converted to a pass by retrying.

Inspection found that `PanelGroup::wireMember` disconnected every receiver of a
transferred Hide button. That removed external observers and Qt accessibility's
destruction tracking. The focused regression failed all three assertions before
the correction: external click preservation, destruction notification and removal
of the deleted button's accessibility interface from Qt's cache.

The correction disconnects only the owning group's handlers, including removal
of the old group's Hide handler during merge. Qt and third-party receivers remain
connected. The focused regression passes with this change. Accessibility remains
enabled; the product does not clear or disable its interface cache.

`bash infrastructure/macos/test-group-signal-lifetimes.sh` exercises that lifetime
contract directly. Native fixture builds also retain source debug symbols for
subsequent crash reports. Full fixture and real Resolve validation remain required;
this evidence does not establish that every reported crash has the same cause.
