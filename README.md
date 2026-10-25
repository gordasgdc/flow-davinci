# OpenGate

OpenGate by CineWaltz gives DaVinci Resolve movable panels, split/tab groups and
saved workspaces while preserving Resolve's real native content.

**[Download 0.0.2 prototype.4 — macOS and Windows tester preview](https://github.com/jordantetewsky/OpenGate/releases/tag/v0.0.2-prototype.4)**

Prototype.4 brings the shared managed workspace to Windows and moves Mac logs to
OpenGate's own log folder. Windows live Resolve acceptance is pending testers.
The [accepted prototype.3 Mac installer](https://github.com/jordantetewsky/OpenGate/releases/tag/v0.0.2-prototype.3)
remains available unchanged for rollback.

## Install and first launch

1. Download your platform's installer from the release's **Assets** section.
2. Save your work and quit Resolve, then double-click the `.pkg` (Mac) or `.exe` (Windows).
3. Open Resolve normally. OpenGate should load automatically. A first OpenGate
   profile opens **Resolve Layout**; existing profiles retain their last working
   arrangement, including unsaved changes and user presets.

Mac: universal Apple Silicon/Intel, macOS 15+, Resolve 21 at its standard location;
Developer ID signed and Apple notarized. Windows: x64, Resolve Studio 21.1 with
Qt 5.15.2; installer unsigned. Windows free-edition startup is unverified. Automatic
startup is implemented and fixture-tested; actual Windows Resolve plugin discovery
still needs tester confirmation. The Start-menu OpenGate launcher is for recovery.
Please report failures to load automatically.

Drag panel titles to rearrange, float or group them. Use **Panels** in the permanent
main title strip to hide and recall panels. **Workspace Toolbar** and **Page
Navigation** hide independently. **Use Original Resolve UI** restores native
ownership while preserving the OpenGate arrangement. The built-in Resolve Layout
is immutable; use **Save as New** for a writable preset.

Assign commands in Resolve's **Keyboard Customization → Workspace → Workflow
Integrations**. The hovered-panel command is **OpenGate - Maximize-Restore Hovered
Panel**. Physical/remapped keys and GPU rendering require live testing on Windows.

## Send support logs

Reproduce the issue, note its time, then double-click the export entry:

| Platform | Export entry | Raw log folder |
| --- | --- | --- |
| Mac prototype.4 | Applications → OpenGate Tools → Export OpenGate Support | `~/Library/Logs/OpenGate` |
| Windows prototype.4 | Start → OpenGate → Export OpenGate Support Logs | `%LOCALAPPDATA%\OpenGate\Logs` |

Send Jordan the **OpenGate support ZIP created on your Desktop**, plus your local
time/time zone, Resolve version and edition, OS version, page/panel, reproduction
steps, expected result and actual result. Include a screenshot for painting or
geometry issues; on Windows also include display scaling and monitor arrangement.
Nothing uploads automatically. Review the ZIP before sharing it.

Events identify OpenGate, module, build, session and sequence, along with observed
input/signals and operation outcomes. A recorded click is not proof that Resolve
completed an operation. Mac export filters private paths and project titles from
its event summary; Windows includes bounded logs and native widget inventories.
Neither exporter copies projects, media or databases. Mac prototype.3 logs stay
in `~/Library/Logs/Unbound`; prototype.4 export reads both folders.

## Uninstall and recovery

Quit Resolve, then use **Applications → OpenGate Tools → Uninstall OpenGate** on
Mac or **Start → OpenGate → Uninstall OpenGate** on Windows. Presets, shortcuts and
arrangements remain. Ordinary uninstall/reinstall does not create a fresh profile.
Keep a verified backup before any separately planned first-user demonstration.
Removing only the launcher does not remove the resident plugin.

## Verification and development

Read the [release's testing status and checksums](https://github.com/jordantetewsky/OpenGate/releases/tag/v0.0.2-prototype.4),
[equivalent Mac/Windows live checklists](https://github.com/jordantetewsky/OpenGate/blob/v0.0.2-prototype.4/doc/platforms/prototype4-candidate-review.md)
and [evidence ledger](https://github.com/jordantetewsky/OpenGate/blob/v0.0.2-prototype.4/doc/platforms/windows-managed-workspace-progress.md).
Builds and fixtures do not establish installation or live acceptance. No
performance improvement is claimed. Main now includes the shared workspace core,
Source presentation checkpoint, retained Close repair and persistence extraction
reviewed on October 25. The locally installed Mac candidate is `093c9de`; Jordan
reported a positive live trial and requested this merge. Startup/reopen latency,
ordinary-launch Source experiment dependency and Windows live acceptance remain
open. Published installers are unchanged; prototype.4 source remains identified
by its release tag.
Repository identity migration is a separate, deferred task.

New cross-platform features require one behavioral specification, deliberate
shared-code/platform-adapter boundaries, equivalent coverage, Mac regressions,
and explicit platform gaps and live-test status. Two successful builds are not
cross-platform completion.

## Acknowledgments

OpenGate is developed by CineWaltz, based on
[Wolfgang Woehl's Unbound](https://github.com/wolfgangw/unbound). Inherited copyright
and MIT notices remain in [LICENSE](LICENSE). Internal Unbound identifiers remain
where needed for compatibility. See the [historical upstream README](doc/upstream-readme.md).
OpenGate is not affiliated with Blackmagic Design.
