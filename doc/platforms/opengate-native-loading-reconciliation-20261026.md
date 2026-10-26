# Native loading reconciliation — October 26

Candidate worktree: `native-loading-normal-launch`, branch
`fix/native-loading-normal-launch`, based on local main `00e79a7`.
This is an uninstalled candidate plus diagnostics, not completed native navigation.
No merge, push, publication, layout reset or Resolve termination performed.
Existing uncommitted recovery notes in main-release and reopen-title-preparation
are preserved.

## Runtime and launch evidence

Read-only recheck: all 54 installed payload files still match the ffc7c98 plan.
Against the recovery snapshot, 23 of 24 protected files match; Workspace.json
changed during subsequent use. It was not restored or overwritten. Reconciliation
receipt and rotated log capture are in sibling reopen-menu-preparation's
`build/macos/launch-reconciliation-20261026/`.

PID 26600 / session 64a99df9 ran ffc7c98. Filtered `ps eww` inspection found neither
OPENGATE_EXPERIMENT_VISIBLE_NATIVE_STASH nor OPENGATE_TRACE_NATIVE_LOADING.
Its full Edit root mounted and assembly completed without a native-remainder
leaf; there was no visibility-experiment event or loading-context event. Thus
its structural path was eligible, but its process did not have the experiment
flag. The earlier 093c9de PID 26245 session likewise completed full Edit assembly
without a remainder and without the experiment event. Its terminated process's
environment cannot now be inspected; source path and logs support a missing/not-1
flag, not a retained-remainder explanation. The relevant root/assembly source is
unchanged between 093c9de and ffc7c98.

Jordan quit Resolve. The existing launcher started unchanged installed ffc7c98
with both flags, PID 28049 / session 715f60a8-1ba4-4804-b615-db02dd4ace5e.
Event 751 records enabled visibility with stash_visible=true and a mask outside
the rectangle. Loading snapshots (including 1896, 1937, 1993, 2058) show
m_pConformPanel and its stash/main ancestry effectively visible.

Jordan's distinct results in this session:
- Double-click loading works.
- Hidden docked Source loads but does not reveal.
- Visible floating Source loads but blinks.
- Hidden floating Source loads but does not reveal.
- Find remains unresolved. Jordan's native-behavior explanation is not an
  OpenGate pass. Captured Show Media Pool actions are not Find.
- Preserve the prior working Match Frame report; action timelineMatchFrame at
  1161 is observed, but is not a new picture-acceptance result.

This worktree's `build/macos/live-20261026/` preserves rotated logs, session-only
records and the user reports. Earlier successful controls in the historical
visibility-experiment report remain evidence; no layout tour was repeated.

## Executed path, hypothesis and bounded candidate

The failing historical double-click reaches the native Media Pool viewport and
Source focus, without the successful controls' Source title changes and confirmed
picture changes. The full layout parks the native root under a hidden stash.
Working native/remainder controls and the now-working flagged session preserve
effective native-tree visibility. Media Pool contents transfer, wrappers and
embedding alone are not sufficient causes.

Falsifiable hypothesis: preserving the parked Edit tree's effective visibility
on ordinary launch restores loading in the same arrangement. The same-build
flagged result supports this; exact internal Resolve handler execution remains
unobserved. Tracing was also enabled, so this is not a proof of every internal
causal step. Normal-launch candidate acceptance remains necessary.

NativeRootLayout remains the sole native-root lease/visibility owner. Its existing
masked, mouse-transparent visibility behavior becomes automatic only for a full
Edit arrangement without a native remainder. NativeArrangement invokes the
renamed method. Page departure, Native return and remainder adoption release
or suspend it through existing lifecycle paths. No new layout/state schema,
controller, keep-showing timer or command replay. ViewerDiagnostics retains
bounded controller ancestry under loading tracing without requiring the obsolete
experiment flag; the field is now native_controllers.

Source reveal is deliberately separate: the live session has navigation delivery
but no source.navigation.recall. Existing code silently rejects several states;
the logs cannot identify which check failed. The existing routine moves out of
PanelControls into SourceNavigationRecall, which recognizes input and queues the
existing door request. PanelControls supplies current ownership/transaction state
and its existing showPanel/save callback. The old routine is removed. New skipped
and queued diagnostics identify the exact rejection and phase; no speculative
eligibility relaxation is claimed to fix the failure. Deferred ownership now
also checks the retained source-session identity.

Floating blink remains open. In the sampled 2057–2120 interval, window ordering
and native focus changes occur, but no floating-host hide/show pair is recorded.
This does not identify the originating native/GPU behavior. Do not remove recovery
or force repeated shows based on that observation.

## Local verification and organization review

- Universal arm64/x86_64 production helper compilation and bootstrap checks pass.
- Hidden ownership suite passes ordinary launch, legacy flag, non-Edit exclusion,
  unchanged synthetic pixels/geometry/focus, explicit hidden children, page changes,
  remainder unmasking and exact Native restoration.
- Hidden assembled first-open suite with SOURCE_RECALL=1 passes hidden docked,
  hidden floating and tabbed Source, blank/right-click exclusions, retained hosts,
  geometry/tree stability, first-open flow and unsaved restart.
- Native entry, interaction diagnostics, native preset bindings and workflow
  command submenu suites pass. Logs are under build/macos in this worktree.
- The initial baseline ownership run compiled but failed inside the sandbox because
  no macOS screen was accessible; its permitted window-service rerun passed.
- Actual diff reviewed for state writers, boundaries, dependencies and growth:
  root lease stays focused; Source input recognition/deferral moved from coordinator
  to a 55-line focused module; coordinator shrinks by five lines. No parallel
  visibility owner or replaced routine remains. Shared callers compile on macOS;
  Windows uses shared PanelControls/NativeArrangement but has not been compiled
  or executed here. No Windows runtime acceptance claimed.

Numbered contract mapping: assembled first-open/restart (1), Save/Update protection
(2), Native return/re-entry (3), temporary maximize (4), Native gestures (5,8),
primary header/footer and independent bands (6,7), flat Panels and retained Source
placement (9), page changes (11) and assembled sequence (12) have relevant fixture
coverage. Correct real clip pictures, hidden Source recall, native Find destination,
blink, GPU painting/maximize (10), physical/remapped inputs, Relay/Deliver and
Windows remain live/platform gaps. Fixtures do not close these gaps. Fixed Playhead
and Stabilize behavior is unchanged; no new live acceptance is claimed.

## Next precise gate

Installation requires Jordan's explicit authorization for this candidate and
Resolve closed by Jordan. Do not install 376b153 or replace the live build silently.
After authorized installation, an ordinary launch without either flag and one
hidden-Source double-click can establish both automatic loading and the exact
recall rejection. A/B picture confirmation is still necessary. No repeated layout
tour. Find must be exercised as the real native command with correct clip selection
and retained Media Pool reveal, recorded independently; never substitute Show.

Remaining checklist scope is unchanged: presentation/visibility stability, startup
and Timeline sizing, container semantics, docking targets, readable controls,
assignable hold-to-bypass, persistence, native return, Windows foundation and later
platform/release work remain tracked in the repair checklist. Speed work stays paused.

## Authorized installation — October 26

Jordan authorized this candidate and Resolve was verified closed. Installed the
reviewed helper bundle as 00e79a7-dirty (fix/native-loading-normal-launch), with
source and binary hashes in build/macos/candidate-receipt.json. All four bundle
files match; all 24 protected settings/preferences remain unchanged. Installation
receipt: build/macos/authorized-install/receipt.json. Recoverable ffc7c98 checkpoint:
/Library/Application Support/CineWaltz/OpenGate/Transactions/58DFCD25-F975-4934-A1A8-3EA914F357BD.
No merge, push or publication. Ordinary-launch loading is expected to work without
the experiment flag; hidden-Source recall is diagnostic only, and blink/Find are
not claimed fixed. Next live test: ordinary launch, A/B pictures, then one hidden
Source double-click to capture the rejection reason. Earlier uninstalled status
in this document describes preparation and is superseded by this receipt.
