# macOS development status

The Mac branch has an experimental Apple Silicon helper, launcher and panel
controls. It is not a release backend: root CMake still rejects Mac application
configuration, and Windows release builds remain independent.

## Current experiment

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
Index, Effects, Metadata and Mixer. These sidebars transfer their content layout,
keeping the native outer slot in place. Undock All initializes hidden sidebars
through their native toolbar toggles. Source still requires dual-viewer mode.

The last arrangement is saved atomically in
`~/Library/Application Support/Unbound/Workspace.json`. Floating state, user-hidden
state, geometry, title style and control pinning persist. Desired floating panels
restore when the Edit page and lazily created targets become available. Shutdown
saves desired state before restoring Resolve's original widget ownership.
Closing the controls hides them and leaves the floating workspace intact.

Per-panel Show/Hide and bulk Show All/Hide All retain hosts. Bring All On Screen
and automatic display-work-area bounds recover off-screen windows; native minimum
sizes are preserved. "Keep controls on top" sets the controls' native Qt topmost
flag. Title styles are Standard, Compact traffic lights (a full-size native title
bar with integrated lights), and Hidden (a drag grip and custom resize edges).

These additions pass native fixtures, including workspace reload and title style
checks, but still need validation on the actual added Resolve sidebars. Magnetic
snapping, drag-to-dock and full shared-core platform integration remain unfinished.

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
control requests without duplicate windows. Extended fixtures cover nine-panel
bulk operations and cancellation, initialization through native toggles, exact
multi-part layout restoration, saved workspace reload, hidden-state restoration,
pin persistence, display recovery with negative coordinates, native title styles
and mouse-driven resizing with hidden title bars. Tests previously caught a splitter
placeholder insertion bug. Live playback, GPU viewer rendering, native menu
changes, project closure and repeated launches remain required checks.

## Remaining work

Validate the startup route and native redocking first. Then implement the shared
window integration and placement contracts, snapping, drag-to-dock, responsive
resizing, Retina and Spaces/fullscreen behavior, persistence and playback checks.
Define release packaging, dependency auditing, signing and notarization after
runtime behavior is established. See [architecture](../architecture.md).
