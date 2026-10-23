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

Prototype.4 is intended to start automatically with Resolve on both platforms.
The Windows IO-plugin adapter is awaiting its runner result and a Studio tester;
free-edition startup is unverified. The Start-menu OpenGate entry remains a
recovery launcher.

### Send support logs

Reproduce the issue, then export immediately:

| Platform | Double-click export | Log folder |
| --- | --- | --- |
| Mac prototype.4 | Applications → OpenGate Tools → Export OpenGate Support | `~/Library/Logs/OpenGate` |
| Windows prototype.4 | Start → OpenGate → Export OpenGate Support Logs | `%LOCALAPPDATA%\OpenGate\Logs` |

Send Jordan the **OpenGate support ZIP on your Desktop**, with the approximate
local time/time zone, Resolve version/edition, page, panel, steps, expected result
and actual result. Include a screenshot for painting or geometry problems.
Nothing uploads automatically. Export leaves Resolve and settings alone.

Mac prototype.3 logs remain in `~/Library/Logs/Unbound`; prototype.4 export reads
both locations without moving or deleting older evidence. The product is OpenGate;
internal Unbound filenames and settings paths remain for compatibility.
Both runtimes record OpenGate events with build/session/sequence information,
observed input/signals and operation outcomes. A click does not prove the native
operation completed. Mac's export contains a filtered event summary; Windows also
includes bounded runtime logs and widget-structure inventories for first testers.
Review the ZIP before sharing it. Neither export copies project/media/database files.

The same Tools/Start-menu folders provide uninstall. See the
[candidate review and live checklists](doc/platforms/prototype4-candidate-review.md).

Quit Resolve before installation or removal. Removing only the launcher does not
unload a resident helper. Uninstall preserves arrangements, presets and shortcuts.
The accepted revision 3 Mac package remains available unchanged for rollback.
Repository identity migration is a separate deferred task. Responsiveness work
remains on hold; no performance improvement is claimed.

## Development and acknowledgments

OpenGate is developed by CineWaltz. Thanks to [Wolfgang Woehl’s Unbound](https://github.com/wolfgangw/unbound) for the foundation. Original copyright and MIT notices remain in [LICENSE](LICENSE). Internal Unbound identifiers remain for compatibility.

The [historical upstream README](doc/upstream-readme.md) preserves inherited Windows build and usage documentation. macOS source, tests and infrastructure are under their respective `platforms/macos` directories. OpenGate is not affiliated with Blackmagic Design.
