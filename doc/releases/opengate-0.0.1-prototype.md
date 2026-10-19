# CineWaltz OpenGate 0.0.1 Prototype

OpenGate makes Resolve's real Edit panels movable, groupable windows, with saved arrangements, panel doors and independent interface-size controls. Built by CineWaltz on **Unbound by Wolfgang Woehl**, distributed under the MIT license. OpenGate is an independent fork, not Wolfgang's upstream Unbound release and not affiliated with Blackmagic Design.

This prerelease captures the combined build Jordan has been trying. Arranging still has rough edges and some awkward flow that we intend to correct. Earlier development branches contain other useful experiments; they are not interchangeable installers. Responsiveness work continues; this release makes no measured speed guarantee.

## Install

1. Download **CineWaltz-OpenGate-0.0.1-Prototype-macOS-arm64.pkg** under Assets.
2. Save your project and **quit Resolve**.
3. Double-click the package and follow macOS Installer. Administrator authentication is required. This prototype installer is unsigned and not notarized; macOS may require you to explicitly allow it.
4. Open Resolve and an Edit project. Use **OpenGate UI** in the menu bar. The optional **OpenGate** app in Applications opens the panel manager.

**Requirements:** Apple Silicon, **macOS 27 or later**, and Resolve 21 in its standard `/Applications/DaVinci Resolve/DaVinci Resolve.app` location. Tested locally with Resolve Studio **21.1.1 / Qt 5.15.2** on macOS 27. Intel, older macOS, other Resolve versions and nonstandard installation locations are not supported by this package. Resolve supplies Qt; Python, Xcode and source builds are not required for installation. Custom hotkey execution is on the roadmap, not a working feature of this release.

On an untouched user profile, the installer seeds **Native Emulator**, using real modular panels in a familiar Edit arrangement. It also includes **Jordan Arrangmement** and **Jordan Source Monitor below Media Pool**. Native Emulator is a starter, not a claim of pixel-for-pixel native equivalence. Existing profiles and saved arrangements are preserved, not reset. **Native Edit UI** remains available in the menu. Subsequent launches restore the last saved arrangement rather than forcing the starter each time.

The installer backs up previous OpenGate/Unbound helper files and user settings under `~/Library/Application Support/Unbound/InstallBackups/`. Internal Unbound bundle/settings names remain for compatibility. The helper replaces the same Unbound helper slot; do not install upstream Unbound alongside it. Resolve's application bundle and native keyboard preset are not modified.

## Included

- Floating panels, magnetic split/tab groups and per-panel hide/reveal doors.
- Pin light alongside the native traffic lights; backtick maximize/restore for the hovered panel.
- OpenGate UI menu, page-aware saved Edit arrangements, panel callers and manager access.
- Independent Source and Program Monitor surfaces, including Source detachment without loading a clip first.
- Movable Workspace Toolbar: attached or floating, compact strip, independent sizing and horizontal overflow. Callers for Source Monitor, Program Monitor, Timeline and Page Navigation supplement the native buttons.
- Default pinning for a newly floated Workspace Toolbar, with remembered pin preference.
- Project name in outer window headers, with stable short labels inside groups.
- Page Navigation/footer controls, including attachment beneath Timeline.
- Experimental independent native widget metrics; smaller Inspector panels can scroll.
- Diagnostics for controls, commands, pointer movement, drag stages and Qt dispatch latency. Logs remain local under `~/Library/Logs/Unbound/`; review before sharing because they can contain project names and interaction details.

## Known issues and validation limits

- **Custom hotkey execution is on the roadmap.** Commands may appear in Keyboard Customization, but assigning a key does not establish that they execute. Backtick maximize/restore works in Jordan's trial. Do not rely on the other custom panel, Stabilize or Fixed Playhead hotkeys in this release. Existing Resolve shortcuts are retained.
- Arranging, native-background interaction, panel focus/stacking, occasional viewer readiness and drag/drop behavior still need broader testing. Pinning or native recovery may be needed in some flows.
- Scaling adjusts supported native fonts/control/layout metrics, not every custom-painted timeline track or GPU surface. Toolbar appearance and overflow need feedback on other displays.
- Custom layouts are currently focused on Edit. Footer placement remains more restricted than ordinary group panels.
- Thirty-five functional fixture suites passed for runtime commit `78ae2ac43ff5723df18c6d659ecb714e5c5e2662`; Jordan accepted this installed build for the prototype release. Fixtures do not prove all live Resolve behavior. Earlier intermittent drag fixture failures remain recorded. The responsiveness performance gate remains open and was not rerun for this feature pass.
- Unsigned, unnotarized prototype; broader macOS compatibility is not provided by these exact binaries.

## Next targets

1. **Implement reliable custom hotkey execution**, with discoverable assignments, use of existing native shortcuts where appropriate, and verified execution without duplicate assignment stores.
2. Faster, more natural arranging and panel calling, fewer refresh flashes, and measured responsiveness improvements.
3. Further unify the native-background experience; eliminate panel lockouts and improve placement/overflow across displays.
4. Broader macOS support, signing/notarization and a more polished first-install experience.
5. Continue custom clip-color research. Additional hues, including bright red, are **not shipped**. Any future approach must retain valid native fallback colors, preserve clips and editing information without OpenGate, and investigate safe project-sharing metadata.

## Uninstall / recovery

Quit Resolve first. In Finder, use Go → Go to Folder and enter `/Library/Application Support/CineWaltz/OpenGate/`. Open **Uninstall OpenGate.command** and follow its administrator prompt. It moves installed OpenGate files into a backup rather than deleting settings. Your arrangements, native keyboard assignments, projects, media and timelines are retained. Removing the launcher app alone does not remove the resident Resolve helper.

For layout recovery before uninstalling, choose **OpenGate UI → Native Edit UI**. Keep the install backup if you want to restore a previous prototype manually.

## Credits and source

Foundation: [Unbound by Wolfgang Woehl](https://github.com/wolfgangw/unbound), MIT. See the included LICENSE. OpenGate additions and release packaging: CineWaltz. Runtime source and feature provenance are included through the release branch and build manifest. This prototype is released from a separate branch; it is not merged to master/main.
