# OpenGate

OpenGate by CineWaltz makes DaVinci Resolve’s Edit panels movable, groupable windows with saved workspaces.

**[Download the published 0.0.1 Prototype](https://github.com/jordantetewsky/OpenGate/releases/tag/opengate-v0.0.1-prototype)**

`main` now includes the locally tested next-release candidate, installed as **a6fdcf6**. The published 0.0.1 installer does not contain these newer fixes. Version **0.0.2** is the next package target.

## Install

1. Download the `.pkg` from the release’s Assets section.
2. Save your project and quit Resolve.
3. Run the installer, then reopen Resolve and use **OpenGate UI**.

The published package requires **Apple Silicon, macOS 27 or later, and Resolve 21 in its standard application location**. Tested with Resolve Studio 21.1.1. It is unsigned and not notarized. Older macOS compatibility requires a separate build and testing; changing a version label does not make these binaries compatible.

Existing arrangements are preserved. New profiles start with **Native Emulator** and include two additional saved arrangements. **Native Edit UI** remains available for layout recovery.

## Features and current development

- Independent panels, magnetic split/tab groups and saved arrangements.
- Separate Source and Program Monitors, panel visibility and shared maximize/restore commands.
- Floating Workspace Toolbar and supported panel interface-size controls.
- Native-preset hotkeys on ordinary launch, remapping and alternate assignments. Stabilize and other hotkeys work in the maintainer’s current live trial.
- Stabilize selected timeline video clips through Resolve’s API, without requiring Inspector visibility.
- An OpenGate Commands submenu, wrapped keyboard-command labels, compact header clearance and scrollable narrow panels.
- Experimental timeline-tab scrolling, using discrete native arrow steps without changing the selected timeline.

Use Resolve’s Keyboard Customization under **Workspace → Workflow Integrations** to assign OpenGate commands. The backtick command is **OpenGate - Maximize-Restore Hovered Panel**. An explicitly unassigned native command stays unassigned.

Fixtures and a local trial do not establish every native layout/page combination. Fixed Playhead real scrolling in both states, header overlap in all native layouts, overflow and arrangement restoration still need broader live feedback. See the [candidate notes](doc/platforms/opengate-priority-live-candidate.md) and [task audit](doc/platforms/opengate-task-audit-20261020.md). Supported metric scaling does not scale every custom-painted timeline track or GPU surface.

## Next releases

Package the working command/UI candidate as 0.0.2 with the current truthful system requirements. Work on older macOS compatibility separately, followed by signing/notarization and broader testing. Responsiveness work is on hold.

## Recovery

Choose **OpenGate UI → Native Edit UI** for layout recovery. To uninstall the resident helper, quit Resolve and follow the release’s uninstall instructions; removing the launcher alone does not remove it. The supplied recovery flow retains settings and arrangements.

## Development and acknowledgments

OpenGate is developed by CineWaltz. Thanks to [Wolfgang Woehl’s Unbound](https://github.com/wolfgangw/unbound) for the foundation. Original copyright and MIT notices remain in [LICENSE](LICENSE). Internal Unbound identifiers remain for compatibility.

The [historical upstream README](doc/upstream-readme.md) preserves inherited Windows build and usage documentation. macOS source, tests and infrastructure are under their respective `platforms/macos` directories. OpenGate is not affiliated with Blackmagic Design.
