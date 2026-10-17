# macOS development status

The Mac branch has an experimental Apple Silicon helper, launcher and panel
controls. It is not a release backend: root CMake still rejects Mac application
configuration, and Windows release builds remain independent.

## Workspace presets in the current experiment

Resolve's Workspace menu exposes Custom Workspace Presets. Save the current
layout there; each saved name has Restore, Update, Rename, Duplicate and Delete
commands. These share the controls window's arrangement store. Manage > Reorder
in Unbound opens a grip list; dragging changes both the widget and native menu
order and saves it across launches.

Restore Native Workspace (Dock All) is a separate permanent Workspace action.
It restores native docking and reveals Resolve's main interface without deleting
saved presets. Page Navigation has its own compact Float/Dock control. Timeline
tools can attach back to their actual native slot; detachment leaves no empty
tools placeholder. The retained Source/offline stack can float without loading
footage. See macos-next-pass-status.md for candidate and live acceptance evidence.

## Initial experiment and loading investigation

Resolve 21.1.1 / Qt 5.15.2 on arm64 exposed its Qt application and panel widgets
through a native OpenFX discovery probe. Startup environment insertion did not
load the probe. The original `codesign --entitlements :-` command gave misleading
warnings; `--entitlements - --arch arm64` decodes the entitlements correctly.
The app declares disabled library validation, but not DYLD environment access.

OpenFX caches discovery, so the zero-effect diagnostic was not a reliable startup
hook. The next development installation uses the installed native IO plugin SDK
for startup, advertises no codecs or containers, and queues initialization on the
main thread. Its API handshake and lifecycle pass a host fixture. Repeated normal
Resolve launches still need live validation. Resolve itself is not modified or
re-signed.

The helper links to Resolve's existing Qt libraries using configured Qt 5.15.2
headers. It offers separate controls for Inspector, Timeline, Source Monitor and
Program Monitor. The first experiment detached the entire source/offline stack;
the revised target is only `widgetEditSourcePlayer`. It preserves stack position
and the other viewer pages. Show Source Monitor in dual-viewer mode before
undocking it.

Dock buttons are named and enabled per panel. Restoration checks the original
parent and logs a deferred verification. Native Resolve docking and rendering
still need live validation; synthetic tests cannot establish that they work.
The `macos-panels-workspaces` branch extends this checkpoint with Media Pool,
Index, Effects, Metadata, Mixer, Sound Library and Keyframes. These sidebars transfer their content layout,
keeping the native outer slot in place. Undock All initializes hidden sidebars
through their native toolbar toggles. Source still requires dual-viewer mode.

The preserved branch `macos-arrangements-and-docking` adds a named arrangement
selector and Restore button. Manage offers Save As, Update, Rename, Duplicate and
Delete. Selecting a preset does not move windows until Restore is pressed. Update
explicitly replaces that preset with the current workspace; window movements only
autosave the current workspace. Duplicate copies the saved preset. A Modified
indicator distinguishes current changes. Restore requires the Edit page and
reports unavailable panels. Named presets live in
`~/Library/Application Support/Unbound/Arrangements.json`; invalid/unsupported
files are preserved and writes refused. `UNBOUND_ARRANGEMENTS_FILE` overrides this
path for isolated tests. Presets include existing panel geometry, docking,
visibility, chrome, hierarchy, pinning, viewport options and pair-group layouts.
Independent UI scaling is not implemented. Options and panel controls scroll;
preset and bulk recovery controls remain accessible outside the scroll area.
These additions have fixture coverage but await live Resolve verification.

Work after that checkpoint adds Dock/Undock commands to positively identified
native options menus. They call the same sessions as the controls and reflect
their current state. Unsupported menu ownership is logged rather than guessed;
Timeline does not yet have its own header docking control. On the
`macos-docking-tabs-and-maximize` candidate branch, Expand / Restore Size and
backtick/tilde target the hovered panel, restoring the same panel on the second
press even if the pointer moved. Grouped panels borrow a temporary native host;
their group snapshot is retained for restoration and persistence. Text editors,
dialogs and popup menus retain their keys. Native Resolve fullscreen viewing is a
separate command and remains pending diagnosis; panel expansion does not satisfy it.

Edge snapping is an initial grip-based implementation: drag the small dotted
grip below a standard title bar, the compact header background, or the hidden
style's grip. A blue outline previews nearby panel/display edges; release to
align. Hold Option to bypass, or disable Snap window edges in Unbound. Native
macOS title-bar dragging moves freely without attachment previews.

When an individual floating panel is dragged beside another, the outline covers
both panels to preview joining them. Release to create one native group window
with a horizontal or vertical splitter. Move the group by its title/grip and
resize the shared divider. Join two panels in the scrolling controls also offers
Side by Side and Stack, defaulting to Source/Program. Any two available panels
can form a pair in that checkpoint. The continuation branch
`macos-docking-layouts-v2` replaces the pair limit with nested split trees.
Each member has a draggable title and Detach button. A tear-off keeps the pointer
anchored to the panel, then follows it directly until release. Release Groups
restores standalone positions; member detachment places the released panels near
their group locations. Group placement, orientation and split sizes persist in
the last workspace and named presets. Group close docks both members; docking
one member first releases the pair and leaves the other floating. Groups follow
Edit-page visibility and the highest member's hierarchy tier. Individual Expand
is disabled while grouped; temporary group-aware maximization remains pending.
The v2 branch keeps tear-off in the same drag handler, including attachment and
Option bypass after detachment. Real viewer GPU rendering, multicam playback,
physical drag feel and native shortcuts still need live checks.

In v2, a single 28-pixel docking target appears near the hovered edge. Enter it to
preview the proposed half split; release to attach. Inside a group, the target
belongs to the individual panel under the pointer. A group's outer edge targets
the whole group. Dragging one group onto another merges their split trees into
one native host. Controls can also join the groups containing the selected panels.
Removing one member collapses its empty split while keeping the remaining group.
Saved trees preserve nested orientations and divider sizes; original pair presets
load without conversion of the named preset file. Only explicit Update rewrites a
named preset. Distant edge guides help align windows across gaps without joining
them. Option bypasses guides, snapping and attachment. Native title-bar motion
still moves a whole group without attachment targets; use the Unbound grip to dock.

Drag release is handled regardless of which Qt window receives mouse-up. A native
Cocoa mouse-up observer supplies a GUI-thread fallback; Escape, lost left-button
state and deactivation clear capture and previews. Diagnostics record drag.begin
and drag.end with the completion/cancellation reason. Native monitor tests post
an application-local mouse-up, rather than system mouse input.

Hide empty Resolve background orders out only the native main window, retaining
Qt page ownership and the detached panels. Uncheck it to recover the background;
Dock All also reveals it. The option persists in workspaces/presets. If controls
are hidden, relaunch them through Unbound.app. This is independent background
hiding, not macOS application minimization. Floating tool panels no longer hide
on application deactivation and retain normal native window level. Mixed groups
use normal group activation ordering rather than inheriting the viewer's highest
tier. Unmanaged active Workflow windows remain above workspace ordering, with
text focus preserved. Actual Relay switching needs live verification.

Center on each panel recalls it onto the display under the pointer. A grouped
panel centers the shared host. Recover Windows centers and recalls all active
panels, including previously hidden panels. Automatic display recovery still
keeps windows within available work areas without changing user-hidden choices.
Stabilization remains an investigation: inspector.stabilization.candidate logs
visible native button ancestry and matching actions without invoking them.

The last arrangement is saved atomically in
`~/Library/Application Support/Unbound/Workspace.json`. Floating state, user-hidden
state, geometry, title style and control pinning persist. Desired floating panels
restore when the Edit page and lazily created targets become available. Shutdown
saves desired state before restoring Resolve's original widget ownership.
Closing the controls hides them and leaves the floating workspace intact.

The optional floating hierarchy orders Timeline behind every panel, other panels
(including Media Pool) in the middle, and Source/Program above them. The most
recently active viewer wins within that tier. Native ordering preserves the key
window, excludes hidden/minimized hosts, respects modal dialogs and runs on window
events rather than polling. Pinned Unbound controls remain above the group.
Every reorder logs the bottom-to-top names and native key window before/after.
Quick Export opens Resolve's existing menu action without directly starting a
render; it remains a native dialog rather than a reparented panel.

Per-panel Show/Hide and bulk Show All/Hide All retain hosts. Bring All On Screen
and automatic display-work-area bounds recover off-screen windows; native minimum
sizes are preserved. "Keep controls on top" sets the controls' native Qt topmost
flag. Title styles are Standard, Compact traffic lights (a full-size native title
bar with integrated lights), and Hidden (a drag grip and custom resize edges).

These additions pass native fixtures, including workspace reload and title style
checks, but still need validation on the actual added Resolve sidebars. Full
shared-core platform integration remains unfinished.

## Installation and relaunch

Development artifacts are built by the scripts under `infrastructure/macos/`:

- `build-ofx-probe.sh`, then `build-widget-probe.sh`: shared bootstrap and Qt helper.
- `build-io-helper.sh`: native startup bundle and API tests.
- `build-launcher.sh`: Unbound.app plus launcher/Workflow request tests.
- `test-panel-sessions.sh`: native Qt window regression tests.

Prepare matching headers using `tests/platforms/macos/README.md`. With Resolve
closed, install `build/macos/io-helper/Unbound.dvcp.bundle` into
`/Library/Application Support/Blackmagic Design/DaVinci Resolve/IOPlugins/`.
Place `src/platforms/macos/Unbound.py` in the adjacent `Workflow Integration Plugins`
directory and Unbound.app in `~/Applications/`. Retire the old
`/Library/OFX/Plugins/UnboundLoadingProbe.ofx.bundle` before testing this route.

Open Resolve normally. Workspace > Workflow Integrations > Unbound requests the
resident controls. Unbound.app sends the same request and starts Resolve if it
is closed. Repeated requests reuse the control window and existing panel sessions.
The shortcut cannot attach a helper to a Resolve process that never loaded it.

## Diagnostics and tests

Local logs are in `~/Library/Logs/Unbound/`:

- `events.jsonl`: helper start/stop, control requests, button actions/results,
  docking checks and window moves, resizes, visibility, activation and state.
  Includes UTC time, process ID and helper build. Rotates at 8 MiB, retaining one
  previous file.
- `launches.jsonl`: app and Workflow Integration launch requests and app actions.
- `ofx-loading-probe.log`: native bootstrap phase, process identity and Qt presence
  (the filename is retained from the original discovery experiment).
- `widgets-1.json` through `widgets-3.json`: bounded startup inventories.

Tests cover 30 docking cycles, box stretch/alignment, grid spans, splitter sizes
and order, source-only stack detachment, native stack selection changes,
independent panels, panel-specific button state, close-to-dock, page visibility,
unsupported/hidden target rejection, lifetime, diagnostic records and repeated
control requests without duplicate windows. Extended fixtures cover eleven-panel
bulk operations and cancellation, initialization through native toggles, exact
multi-part layout restoration, saved workspace reload, hidden-state restoration,
pin persistence, display recovery with negative coordinates, native title styles
and mouse-driven resizing with hidden title bars. Native window-order tests check
both active viewers, activation of a background Timeline, real key-window focus
preservation, hidden hosts, pinned controls, disabled hierarchy and modal dialogs.
Quick Export tests check action dispatch and disabled-action rejection.
`bash infrastructure/macos/test-panel-groups.sh` covers horizontal/vertical
single-host pairs, synchronous movement, shared splitter persistence, direct and
layout-transfer panels, page lifecycle, hide/show, pointer-anchored tear-off,
closing a group and reversible native docking. The panel-controls suite covers
drag attachment, ambiguous native menus and persisted group reload.
Tests previously caught a splitter
placeholder insertion bug. Live playback, GPU viewer rendering, native menu
changes, project closure and repeated launches remain required checks.

## Remaining work

Validate the current candidate in a fresh Resolve process: viewer rendering,
page scope/shortcuts, nested groups, Relay ordering and background/window recovery.
Remaining work includes group-aware maximize, native title-bar attachment previews, Timeline
header controls, interface scaling feasibility, Retina/Spaces/fullscreen behavior,
and full shared platform contracts.
Define release packaging, dependency auditing, signing and notarization after
runtime behavior is established. See [architecture](../architecture.md).


### Docking/tab candidate after f64c401 (not installed)

The continuation branch preserves `macos-docking-layouts-v2` and the installed
f64c401 checkpoint. Normal free dragging no longer snaps to display/panel edges
on release; hold Shift for alignment, Option to bypass. A destination offers fixed
small center/tab or edge/split anchors only when approached deliberately. Nested
groups offer separate anchors outside their frame to split the whole group.
Only the frontmost eligible destination participates. No rectangle previews free
movement; the highlighted drop result appears only over an active target.

The layout model now supports tab stacks beside/above/below split groups. Native
panel-toggle buttons feed shared visibility for floating panels, with collapsed
split wrappers and hidden/restored tabs. Shortcut routes that bypass those buttons
still require live verification. Per-host diamond pin controls keep a window or
whole group above the workspace through the normal native ordering, preserving
menu/dialog and keyboard focus rules. Pins are saved in arrangements.

The automatic viewport guard keeps a 96 by 24 logical-pixel grip reachable rather
than forcing the full frame into a screen. Partial off-screen parking is allowed
and large windows retain their size. Explicit Recover Windows and Center recall
panels; display changes preserve hidden choices while rescuing reachable grips.

These are candidate implementations. GUI regression results and live acceptance
are tracked in `macos-workspace-checklist.md`; none of these additions should be
reported as installed or runtime-verified until that ledger records evidence.
