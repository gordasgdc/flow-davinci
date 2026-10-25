# Retained floating window close repair

Branch: `fix/retained-window-close`, based on GitHub checkpoint `192741c`.
Runtime changes are shared Mac/Windows core code. No installation, upstream push,
main merge, release packaging or live Resolve acceptance occurred in this repair.

## Behavior and scope

Ordinary Close now calls the existing session hide operation or the existing whole
group hide operation. It does not dock, destroy the host, or dissolve membership.
Existing panel callers recall the retained host and select the destination tab.
Close on temporary expansion restores the earlier placement. Explicit dock,
native return and shutdown continue to use direct ownership-release operations.
No new controller, visibility timer, model, persisted field or platform workaround
is introduced. `PanelControls` is unchanged. The production diff is confined to
`panelsession.h` and `panelgroup.h`; source-loading changes remain inherited.

The assembled close checks are in `tests/core/workspace-callers/retainedclosechecks.h`
and reuse the first-open fixture's setup. This avoids another copy of the native
fixture topology and keeps additional assertions out of the large coordinator.
Old displayed session/group tests now assert retained hiding; explicit cleanup
uses docking directly. The Mac verification matrix registers the new scenario;
Windows registers it through the production DLL fixture, including forwarding
the DLL argument to the restart subprocess.

## Verification evidence

Local evidence: `build/macos/close-verification/` (ignored build artifacts).

- `before.log`: before the production change, the new assembled fixture reports
  six failures, beginning with standalone Close and retained-host visibility.
- `receipt.json`: seven of eight targeted suites pass on the first matrix run:
  retained-window-close, shared-sessions, native-entry-flow,
  workspace-restoration, panel-caller-flow, opengate-first-open, panel-groups.
- The first-open run includes SOURCE_RECALL and the visible-native-stash
  experiment. It verifies presentation and ownership, not real video pictures.
- The displayed panel-session suite initially fails the actual synthetic drag
  release assertion; three subsequent group-persistence assertions also fail.
  `panel-sessions-checkpoint-baseline.log` passes on unchanged checkpoint 192741c.
  `panel-sessions-recheck.log` passes on the changed code without further changes.
  This is an unresolved intermittent result, not an uninterrupted clean matrix.
- The new focused scenario verifies standalone and nested-group close/recall,
  retained identity/frame/pinning, hidden sibling state, page return, temporary
  standalone/grouped expansion, named snapshot restore, fresh-process unsaved
  restart, native return, explicit docking after native return, and shutdown.
- `build-ofx.log` and `build-runtime.log`: local production loader/helper build
  succeeds; `lipo -archs` reports x86_64 and arm64 for the widget runtime.

The nested-tree assertion initially retained a QJsonValueRef from a temporary
snapshot. The test now copies the value before comparison. No production sizing
change was made to satisfy that assertion.

`dock-baseline.log` records a separate fixture reproduction: after full-layout
startup, detach Inspector and choose Dock Panel, with no Close action. It reports
"The original panel slot changed." This path is unchanged by the repair. A
fixture option `--dock-baseline` retains that reproduction separately from the
close acceptance scenario. Do not claim all docking paths are repaired.

## Contract and remaining gates

Native-entry contract item 8 is the immediate target: X hides and recall retains
placement. Items 1–3 and 11 are checked by startup, named/unsaved persistence,
native return and page-return fixtures; item 4 by temporary expansion/restore.
The existing first-open suite still covers bars, flat panel access and the broader
assembled flow. These remain fixture results, not native GPU or pointer acceptance.

Windows uses the same implementation and has the scenario registered, but no
Windows compile/run is claimed. No physical Mac traffic-light Close or live
Match Frame/double-click picture acceptance is claimed. Playback disappearance,
blinking, clipped labels, docking scopes, startup proportions and flag-dependent
loading remain separate. The 1,334-line coordinator has not yet been extracted;
the next organization task and panel/container definitions are in the checklist.

Before any candidate installation, obtain Jordan's explicit authorization and
preserve settings/checkpoints. Then test only: close a standalone window and
recall it; close a group and recall a destination inside it; verify unchanged
frame/pinning/membership and the actual requested picture for native navigation.
Record live results separately. Do not treat this as permission to install.
