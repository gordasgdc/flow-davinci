# Panel manager responsibility boundaries

`PanelControls` is still a large coordinator, not a completed architecture cleanup. This pass extracts four owners rather than moving its methods into headers that require access to the whole manager:

- `PanelCommandCatalog`: stable command identities, command context and shortcut-editor lifetime. Receives panel transition callbacks.
- `NativePanelToggles`: native button observation, connection lifetime and signal-blocked checked-state synchronization. Door coalescing remains with the panel transition coordinator.
- `WorkspaceSnapshot`: persistence schema and active/inactive panel serialization. No widget mutation or file I/O.
- `PanelExpansionController`: one reversible maximize transaction, readiness versioning, original panel/group/native snapshots, overflow position and temporary foreground priority. Receives explicit transition/report callbacks; cannot access manager internals.

The manager retains UI construction, group coordination, native transfers and startup/preset orchestration. These remain candidates for subsequent extraction. Do not claim the line count alone establishes good boundaries.

Hidden regression tests cover command availability/identity, shutdown suppression, native signal blocking/rebinding, saved state preservation and cancelling expansion before readiness. Group and two-process reopen fixtures compile; their displayed execution and real Resolve acceptance require Resolve closed. No smoothness claim follows from these checks.
