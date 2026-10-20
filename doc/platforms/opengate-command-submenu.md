# OpenGate Commands submenu

Branch `feature/macos-command-submenu` adds an OpenGate-owned Commands submenu
to OpenGate UI: Stabilize Selected Clips, Toggle Fixed Playhead, per-panel
Maximize / Restore and Show / Hide, plus the existing native shortcut-editor
entry. All invoke shared registry command IDs. Native Workflow Integration
actions remain in their original menu, unchanged. Dispatch waits for menu
tracking to end and rechecks registry availability.

The command-menu fixture and workspace-menu integration fixture pass. The full
helper compiles. This branch is not installed, merged or published. Actual
Stabilize and Fixed Playhead backend acceptance remains tracked separately;
presenting a menu does not prove those operations complete.
