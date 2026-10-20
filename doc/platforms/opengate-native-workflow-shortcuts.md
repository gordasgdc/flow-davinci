# Native keyboard-editor command targets

Current acceptance status is in [the task audit](opengate-task-audit-20261020.md).
Installed command candidate: **541cd25**. It is not a live-accepted release.

Thirty native Workflow Integration scripts provide panel show/hide, panel
maximize/restore, hovered-panel maximize/restore, Page Navigation, Fixed Playhead
and Stabilize. Twenty-nine publish single-use command requests to the resident Qt
command registry. Stabilize instead calls Resolve’s internal selected-timeline
API directly and publishes a completion result. Both its menu and custom hotkey
enter that same script; it does not post itself back into the registry.

Native actions remain in their original Resolve menu locations. The old native
menu-regrouping implementation was removed after the crash regression. A safe
OpenGate command submenu is still outstanding; this document does not claim it
has been delivered.

The OpenGate shortcut entry opens Resolve’s native Keyboard Customization. The
candidate preset reader is read-only and follows saved assignments and preset
switches. It currently requires `OPENGATE_NATIVE_PRESET_SHORTCUTS=1` at launch;
ordinary launch does not enable it. This is a product gap, not a completed normal
install experience. The current test session explicitly enables the adapter.

The combined fixture verifies that Stabilize’s registered command reaches its
native workflow action with no Inspector widget, alternate assignment, removed
old key, remapping, preset switching and parse recovery. Python tests execute the
actual generated wrapper against modeled selected clips. Workflow fixtures cover
popup-closing deferral, single consumption and API completion reporting.

These checks do not establish real keyboard dispatch through Resolve’s native
workflow engine, actual stabilization or actual Fixed Playhead scrolling. Live
checks must verify the final operation, not just accepted ingress or a changed
Qt checkmark. Bare backtick/legacy fallback semantics, normal-startup policy,
platform keyboard layouts and Source/Program independence remain in the audit.
