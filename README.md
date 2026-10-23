# OpenGate

OpenGate by CineWaltz makes DaVinci Resolve’s Edit panels movable, groupable windows with saved workspaces.

**[Download the accepted Mac 0.0.2 prototype.3](https://github.com/jordantetewsky/OpenGate/releases/tag/v0.0.2-prototype.3)**

Revision 3 is the accepted Mac checkpoint on main. This feature branch develops
Windows parity for **0.0.2 prototype.4** using the same managed workspace core.
Prototype.4 is not yet published. Windows runner fixtures and packaging are
separate from live acceptance; a Windows Resolve tester is still required.

## Install the published Mac release

1. Download the `.pkg` from revision 3's Assets section.
2. Save your project and quit Resolve.
3. Double-click the installer, then reopen Resolve.

Revision 3 is universal for Apple Silicon and Intel, requires macOS 15+ and
Resolve 21 at its standard location, and is signed and notarized. New profiles
enter OpenGate's **Resolve Layout** automatically. Existing profiles retain their
last working arrangement, including unsaved changes. User presets are preserved;
the built-in Resolve Layout cannot be overwritten.

The main title strip retains **Panels**. Workspace Toolbar and Page Navigation
can be hidden independently. **Use Original Resolve UI** restores native ownership
while preserving the OpenGate arrangement for later return.

## Features and current development

- Independent panels, magnetic split/tab groups and saved arrangements.
- Separate Source and Program Monitors, panel visibility and shared maximize/restore commands.
- Independently hideable native Workspace Toolbar and Page Navigation; supported panel interface-size controls.
- Native-preset hotkeys on ordinary launch, remapping and alternate assignments. Stabilize and other hotkeys work in the maintainer’s current live trial.
- Stabilize selected timeline video clips through Resolve’s API, without requiring Inspector visibility.
- An OpenGate Commands submenu, wrapped keyboard-command labels, compact header clearance and scrollable narrow panels.
- Experimental timeline-tab scrolling, using discrete native arrow steps without changing the selected timeline.

Use Resolve’s Keyboard Customization under **Workspace → Workflow Integrations** to assign OpenGate commands. The backtick command is **OpenGate - Maximize-Restore Hovered Panel**. An explicitly unassigned native command stays unassigned.

Fixtures and a local trial do not establish every native layout/page combination. Fixed Playhead real scrolling in both states, header overlap in all native layouts, overflow and arrangement restoration still need broader live feedback. See the [candidate notes](doc/platforms/opengate-priority-live-candidate.md) and [task audit](doc/platforms/opengate-task-audit-20261020.md). Supported metric scaling does not scale every custom-painted timeline track or GPU surface.

## Development candidate and recovery

The Windows candidate uses the shared managed layout, panel commands and
persistence with focused native window/input/discovery adapters. See the
[parity evidence ledger](doc/platforms/windows-managed-workspace-progress.md) for
actual test results and remaining gaps. Windows fixtures cannot prove native GPU
painting, physical/remapped keys or real Resolve widget compatibility.

Mac's Applications/OpenGate Tools folder contains support export and uninstall.
Windows candidates provide Start-menu entries for **Export OpenGate Support Logs**
and uninstall. Export creates a ZIP to send with reproduction steps and timestamps;
it uploads nothing and leaves Resolve and settings alone.

Quit Resolve before installation or removal. Removing only the launcher does not
unload a resident helper. Uninstall preserves arrangements, presets and shortcuts.
The accepted revision 3 Mac package remains available unchanged for rollback.
Repository identity migration is a separate deferred task. Responsiveness work
remains on hold; no performance improvement is claimed.

## Development and acknowledgments

OpenGate is developed by CineWaltz. Thanks to [Wolfgang Woehl’s Unbound](https://github.com/wolfgangw/unbound) for the foundation. Original copyright and MIT notices remain in [LICENSE](LICENSE). Internal Unbound identifiers remain for compatibility.

The [historical upstream README](doc/upstream-readme.md) preserves inherited Windows build and usage documentation. macOS source, tests and infrastructure are under their respective `platforms/macos` directories. OpenGate is not affiliated with Blackmagic Design.
