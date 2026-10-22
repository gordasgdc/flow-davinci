# Windows parity after the accepted Mac checkpoint

Jordan accepted the revision 3 Mac behavior on 2026-10-22 and requested its
merge to main as a major checkpoint. Preserve this baseline. Windows parity is
the next task, not a completed property of the current Windows installer.

Read AGENTS.md, opengate-native-entry-flow-acceptance.md, the latest entries in
opengate-task-audit-20261020.md and macos-workspace-checklist.md before changes.
The numbered entry contract is the behavioral specification for both platforms.

## Required outcome

Windows must provide the same OpenGate user sequence as Mac: automatic first-open
Resolve Layout; subsequent unsaved arrangement restoration; reversible Native UI
ownership return; permanent Panels access; independent toolbar/navigation bands;
synchronized header/menu/hotkey visibility; retained floating hosts and geometry;
inactive-tab selection without relocation; individual and whole-stack docking;
readable actual titles; title dragging and temporary maximize on every supported
panel, including nested Mixer and Keyframes; Timeline and its toolbar together.
Native Resolve controls/content and projects/media must remain intact.

Reuse working modules and extract coherent shared policy where appropriate. Keep
OS window, ownership and input mechanisms behind platform adapters. Do not build
a parallel UI or grow a central controller to duplicate Mac behavior. Inventory
existing Mac and Windows paths before choosing extraction boundaries.

## Regression and delivery gates

Perform Windows work on an isolated branch. Preserve the accepted Mac installation
and rollback artifacts. Changes to shared code must retain Mac behavior and run
the relevant Mac fixtures; before delivery, rerun the complete assembled Mac flow
and matrix. Current reference: 47 hidden and 9 displayed suites passed at
75e8217, production source identical to 88988d4. Document any new failures.

Build Windows on its runner and exercise the assembled user sequence with
platform fixtures, not compilation alone. Record implementation, build, fixture,
installation and live Resolve acceptance separately. No Windows live-testing
claim is possible without a Windows Resolve tester. Do not silently weaken the
contract because only Mac is locally available.

Packaging must retain reversible install/uninstall, presets across upgrades and
downgrades, version/build provenance and exportable failure diagnostics. The
current Windows installer at e3caabf uses the older floating-pane backend and is
not the parity deliverable. No release replacement is implied by this handoff.
