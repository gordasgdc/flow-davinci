# OpenGate

OpenGate by CineWaltz makes DaVinci Resolve’s Edit panels movable, groupable windows with saved workspaces.

**[Download CineWaltz OpenGate 0.0.1 Prototype](https://github.com/jordantetewsky/OpenGate/releases/tag/opengate-v0.0.1-prototype)**

This is an early macOS prototype. The release page contains the installer, requirements, release notes, recovery instructions, and known limitations.

## Install

1. Download the `.pkg` from the release’s Assets section.
2. Save your project and quit Resolve.
3. Run the installer, then reopen Resolve and use the **OpenGate UI** menu.

The published package requires **Apple Silicon, macOS 27 or later, and Resolve 21 in its standard application location**. It was tested with Resolve Studio 21.1.1. The package is unsigned and not notarized. These requirements describe the current binaries; broader macOS support is planned.

Existing arrangements are preserved. On a new profile, **Native Emulator** provides a modular Edit starting arrangement. Two additional saved arrangements are included. **Native Edit UI** remains available for returning to the native layout.

## Prototype features

- Move panels between independent windows and magnetic split or tab groups.
- Save and restore workspace arrangements.
- Separate Source and Program Monitors, with backtick maximize/restore for the hovered panel.
- Show or hide panels in their remembered locations.
- Float the Workspace Toolbar and adjust supported panel interface sizes.

**Custom hotkey execution is still in development.** An assignment appearing in Resolve’s Keyboard Customization does not guarantee execution. Layout, overflow, page transitions, and responsiveness also need broader testing. This repository’s default view represents the published prototype; experimental branches are not interchangeable releases.

## Roadmap

1. Reliable custom hotkeys that follow Resolve’s assignments.
2. Stable panel recovery and page transitions; clear, compact headers without overlapping controls.
3. More natural arranging and measured workflow responsiveness improvements.
4. Broader macOS compatibility, signing, and notarization.

See the [prototype release notes](https://github.com/jordantetewsky/OpenGate/releases/tag/opengate-v0.0.1-prototype) for the full status. Historical development documents may describe older experiments.

## Recovery

Choose **OpenGate UI → Native Edit UI** for layout recovery. To uninstall the resident helper, quit Resolve and follow the release’s uninstall instructions; removing the launcher alone does not remove it. Settings and arrangements are retained by the supplied recovery flow.

## Development and acknowledgments

OpenGate is developed by CineWaltz. Thanks to [Wolfgang Woehl’s Unbound](https://github.com/wolfgangw/unbound) for the foundation. Original copyright and MIT notices remain in [LICENSE](LICENSE). Internal Unbound identifiers remain for compatibility; they are not the product name.

The [historical upstream README](doc/upstream-readme.md) preserves the inherited Windows build and usage documentation. OpenGate’s macOS source, tests, and build scripts are under `src/platforms/macos`, `tests/platforms/macos`, and `infrastructure/macos`. Automated fixtures do not replace live Resolve acceptance testing. OpenGate is not affiliated with Blackmagic Design.
