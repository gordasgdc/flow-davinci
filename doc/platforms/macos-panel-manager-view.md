# Panel manager presentation boundary

`PanelManagerView` constructs labels, options, arrangement controls, group selectors, action rows, footer buttons and the foundation credit. It owns no panel sessions, native transfers, file writes or restore sequencing. QWidget parentage owns the actual controls. Existing object names, action ordering, preferences, scroll preservation and callback wiring remain compatible.

`PanelControls` uses the constructed controls and connects them to transition owners. Command registration/editor lifetime, expansion, native toggles, snapshot serialization and workspace restoration already have separate owners. Group coordination and policy wiring remain in the coordinator; this extraction does not claim the architecture is finished.

Use existing manager-hidden, native/group, shortcut/menu and cross-process restoration suites to validate behavior rather than tests that merely count constructed buttons. Displayed combined validation is required before installation.
