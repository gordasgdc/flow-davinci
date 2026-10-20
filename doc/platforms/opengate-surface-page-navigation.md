# Surface Page Navigation

Branch: feature/macos-surface-page-navigation, based on installed ca505c1.
The working command/hotkey and scoped Source-docking foundation is retained.
The local footer-preflight experiment ad7780a is excluded.

## Behavior

Page Navigation is a surface footer, outside panel splits/tabs. The primary
custom Timeline host gets a strip by default; the visible native main surface
remains primary when Timeline is docked there. A lone floating Inspector does
not acquire the primary footer by accident. Other content hosts opt in through their
Panels > Page Navigation > Pin to This Surface setting. Standalone toolbar strips
do not gain another footer row. Switching a group's selected panel does not
reparent the strip or the native page buttons.

Show Page Navigation remains Resolve's native global command and hotkey. Its
checked state is authoritative; saved arrangements cannot override it. OpenGate
show/hide controls invoke that native QAction, not another keyboard binding.
All pinned surfaces hide/show together without losing their placement choices.
Early construction/fixture fallback uses the real footer visibility only until
the native action is available. Native action lookup still requires its English
label; localized discovery remains unverified.

Owned QToolButtons mirror the native page buttons' availability and invoke their
original clicked signals. Native parentage, layout positions, metrics, controls
and callbacks remain intact. The current page updates across every strip.
Native icons are used when exposed; independent vector glyphs are the fallback.
Only the native page-button region receives an adaptive presentation overlay,
leaving Home/Settings and adjacent footer controls untouched.

Wide strips show complete labels. Narrow strips use icons with complete hover
and accessibility names; if icons do not fit, a horizontal scrollbar keeps the
last page reachable. Buttons have explicit intrinsic widths and never elide.
The short native footer slot uses compact controls inside its existing bounds.

## Persistence and migration

A surface-v1 snapshot records per-surface choices using existing stable host
object names. Old floating/attached footer records are accepted without creating
a standalone footer window or acquiring a native-widget lease. The native footer
never moves; Dock All and arrangement switching have no accessory to detach.
Existing saved preset files are not rewritten by loading this model. The native
global preference remains separate from surface placement. Removing a custom
strip deletes only OpenGate widgets; native controls remain in place.

## Validation

Hidden fixtures cover primary/secondary surfaces, shared native visibility and
page-button dispatch, actual page changes, current-page synchronization, optional
pins retained across hide/show and model recreation, wide/narrow/font-scale
reachability, full tooltips, preserving Home beside the native overlay, duplicate
refresh prevention and unchanged native footer slot/parent/constraints. Legacy
lease tests remain isolated to the old general-purpose accessory class; footer
production code no longer uses that class.

Displayed group/restart fixtures were updated to the new surface contract and
compile. They must run with Resolve closed. Actual native page switching, native
visibility hotkeys, repeated saved-arrangement transitions, footer rendering and
restart behavior still need live acceptance. Fixtures do not prove the historical
first-click restore failure fully resolved. No performance work is included.

Build and installation are distinct. No installation or main merge is authorized
by this document. The prior installed ca505c1 remains available for rollback.

## Pinned-only refinement

Jordan chose a reserved footer band instead of a dockable panel. Float, Native
Dock, attachment and drag affordances are removed rather than hidden. The
manager provides a checked Pin to Main Surface control; each surface's Panels
menu provides its own pin setting. Unpinning returns the band to surface content.
The native main surface also supports an independent pin choice without
changing the native global Show Page Navigation assignment or checked state.
Global hide/show preserves those pin choices. Other independent windows retain
normal overlap behavior; no new window-placement exclusion rules are introduced.

The fixture checks content/footer non-overlap, content expansion after unpin,
group-content replacement preserving the bottom band, and native-main unpin
while other footers remain visible. Old footer-docking methods and callbacks
were removed; old serialized floating/attachment keys are accepted as migration
input only. Teardown restores the native footer's global visibility.

A backtick conflict investigation is separate. Jordan confirmed the installed
build works after resolving duplicate assignments. No shortcut conflict-policy
change or proposed Fusion exception is included in this footer branch.
