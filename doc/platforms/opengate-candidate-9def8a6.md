# Installed OpenGate candidate 9def8a6

Installed 2026-10-19 after all **28 functional suites passed** on clean unchanged
source. Helper/bootstrap signatures and byte hashes verified; all thirty workflow
command scripts match generated artifacts. Resolve left closed for live testing.

- OpenGate UI → Show Panel in Workspace → Edit lists registered Edit panels,
  toolbars, Mixer, Source, Program and Page Navigation. Show is not a hide toggle.
- Edit page lifecycle checkpoints the latest modified view and resumes retained
  panels on return; other pages use the native UI. No selected preset reload.
- Source readiness repair reveals the current native child page if its visible
  outer stack retained an explicitly hidden selected child. It logs outer/inner
  state. Actual blank-source acceptance remains pending.
- Thirty native Workflow Integration command targets are installed. Search
  **OpenGate** in Resolve Keyboard Customization under Workspace → Workflow
  Integrations. They include independent panel doors/expansion, hovered-panel
  Maximize/Restore, Stabilize and Fixed Playhead. Enumeration, assignment,
  persistence and actual native shortcut invocation need fresh-launch acceptance.
- Bare backtick is now the editable default of the hovered-panel expansion
  command. Clear it in OpenGate Panel Shortcuts if using only a replacement native
  binding. No separate dockable keyboard UI was built.

Stabilize still requires its enabled native Inspector button to be visible.
Fixed Playhead may require opening Timeline View Options once. Timeline custom
track/header/ruler/clip-lane scaling remains unfinished; the native-metrics
scaling candidate does not establish coverage of custom painting. The performance
gate remains open and was explicitly excluded from this feature pass.

Functional evidence:
`build/macos/verification/9def8a6d1d-functional-20261019T130547Z/receipt.json`.
Installation evidence: `build/macos/install-receipt.json`.
Previous c9d6b1e helper, launcher and latest workspace/arrangement/shortcut settings:
`build/macos/checkpoints/pre-9def8a6d-20261019T131150Z`.

Feature branches remain separate and published on the personal fork. Main/master
is unchanged. Settings and stable binary identifiers retain their legacy paths;
user-facing product identity is OpenGate by CineWaltz, with Wolfgang Woehl's
foundation credit and MIT license preserved.
