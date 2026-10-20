# Native toolbar title clearance

The OpenGate accessory strip previously contributed all button-label widths to
the native row's minimum/preferred width. The strip now has zero horizontal size
hints and no layout-imposed minimum, and constrains its controls to the actual
slot and title budget. Native controls, title text, parentage and constraints
remain unchanged. Move/hide events refresh the budget; titles in another row do
not reserve unrelated space. Existing global/header panel menus retain callers
when the accessory cannot fit.

The hidden workspace-header fixture passes title-clearance, labels/icons/overflow,
width growth/shrink and native constraint checks. This is a source fix ready for
a combined test build, not proof that every native Resolve header is fixed.
Live-test Native Edit UI with toolbar attached, detached and hidden, and compare
Native Emulator.
