# Native entry flow: acceptance contract

This is the user-visible flow, not a list of implementation modules. Passing a
component fixture does not establish this flow. Keep installed software separate.

1. Fresh launch uses normal Resolve UI unless restoring the last modified layout.
   No emulator selection, Panel Manager or background checkbox is required.
2. On Native Edit, the native header retains its controls and styling. Source and
   Program Monitor controls work there, including drag-to-place from the header.
   Native panel placement handles also work before loading any arrangement.
3. Added header controls fit their allocated space, preserve the project title,
   and have a reachable overflow route at narrow widths. Native buttons keep their
   original identity and signal wiring. No unbounded startup discovery in filters.
4. Both title-bar Panels and OpenGate UI > Panels directly list current-page
   panels plus Header and Footer. No Edit/Workspace/All Panels category layer.
   Visibility changes never undock a native panel or load a preset.
5. Header and Footer mean pinned or hidden. They reserve outer space in the
   workspace and never become a panel-group leaf. Hide reclaims that space; Show
   restores the same native controls. Panels remains accessible with both hidden.
6. All visibility entry points use one state. Footer follows Resolve's native page
   navigation visibility command and includes native Home and Settings. Header
   and footer are excluded from ordinary panel dragging/docking/maximize targets.
7. Rearrange directly from Native Edit, save an optional named arrangement, or
   leave it unsaved. Last modifications survive restart. Named arrangements remain
   selectable independently of the optional advanced Panel Manager.
8. Native Edit explicitly returns the original native layout from a modified,
   grouped or maximized state. Original layout slots and controls survive. An
   incomplete return reports failure and is not presented as success.
9. Page departure/return retains each page's independent current state. Unsupported
   Fusion arrangements are not claimed complete; no cross-page native ownership.
10. Demonstrate the sequence from (1) through (8) in one assembled fixture using
    public controls/menus and native drag callbacks, including hidden bands and a
    narrow header. Add restart coverage. Then live-test native rendering, actual
    pointer dragging, maximize response and Relay > Deliver before release claims.

Before stating ready: reread every numbered item, link it to its test evidence,
and mark anything not actually demonstrated pending. Do not substitute separate
menu tests or a generic accessory fixture for the assembled flow.
