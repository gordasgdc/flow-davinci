# Media Pool double-click: bounded visibility experiment

Status: experiment, not a demonstrated repair. Branch
`experiment/native-stash-visibility`, based on installed `2642b15`.
No installation, Resolve input/termination, main merge, push or publication.
The primary investigation worktree's existing edits are untouched.

## Executed paths and runtime identity

Read-only verification on October 24 found all 54 installed payload files match
`native-loading-routes/build/macos/install-plan.json`. No Resolve process was
running. The latest OpenGate session is `ad6840d9`, PID 2111, build `2642b15`,
with `clean_shutdown=true`. Current logs plus all five rotations contain that
session's 6,149 records and the earlier failing session's 1,135 records. There is
one matching loader/helper pair in the inspected system/user IOPlugins and OFX
roots. There is no live process whose loaded-image list can now be inspected;
this does not retrospectively prove there were no duplicate loads. Historical
builds and development sessions in rotations are not competing running helpers.
The latest session has exactly one helper.start, one feature-manifest and one
helper.stop. Installed-base startup also checks for the existing
unboundControlService before creating it. These observations support a single
controller instance; they are not a substitute for a live loaded-image inventory.

Failure session `513203ea`: native Media Pool viewport double-click observations
910/941/972/1001 precede native Source focus activity, but no Source title-change
notification occurs in the captured intervals. Jordan confirms picture failure.
Media Pool used `ManagedPanelSession::undockContents`: native layout transferred
to a replacement widget and wrapped. `NativeRootLayout::mount` at 723 parks the
remaining page layout under a hidden `unboundNativeLayoutStash`.

Working same-build controls: native double-click 1502 -> Source focus 1506 ->
title notification 1512; second clip 1531 -> 1541. Embedded Pool/Source group
double-click 4955 -> focus 4959 -> title notification 4965. Jordan confirms actual
picture changes. The embedded control mounts at 4841 and shows the native
remainder at 4842. Thus both cases use root-layout transfer; the successful one
also presents the retained native tree. Transferring/wrapping Media Pool and
embedding a group are not sufficient causes of failure.

Neither successful native double-click nor failure requires an observed
PlaceClipOnSourceViewer signal. Native handler execution inside Resolve remains
unobserved. Signal-observer ordering cannot identify native slot ordering.

Executing OpenGate paths: InteractionDiagnostics observes input without consuming
it. ManagedPanelSession records Program Hide at 914 and recovery at 921, with
similar later pairs; recovery calls content->show(), then respects covered tabs.
This is a real visibility interaction, also present in earlier working code,
not proof of the loading cause. VacatedContents suppresses empty-slot Show events;
no such suppression is logged inside failure interval 900–1018. NativePanelToggles
intercepts registered toolbar button input, not the clip viewport; checked changes
schedule signal-blocked presentation synchronization. No native-activation event
is recorded in that interval. Silent synchronization execution cannot be excluded
by the log. PanelTitleGesture acts on title drag thresholds, not double-clicks on
the native clip viewport. No replay/reveal callback from rejected eb59502 is in
this installed-base experiment. No evidence identifies stale branch code as active.

## Hypothesis and exact intervention

Hypothesis: hiding the parked native layout subtree prevents the native
double-click load route from completing even though Source can receive focus;
making that subtree effectively visible will restore loading in the full layout.
This is falsifiable and unproven. It is narrower than saying two modes are wrong.
No architectural constraint requiring different user experiences is established.

Only when `OPENGATE_EXPERIMENT_VISIBLE_NATIVE_STASH=1`, and only for a full Edit
arrangement without a native-remainder leaf, the existing root lease shows its
existing stash. A nonempty mask outside its rectangle clips all its pixels;
mouse transparency prevents it intercepting pointer input. Explicitly hidden
native children stay hidden. The lease follows page departure/return and removes
the experimental mask/attributes on Native return or remainder adoption.
No explicit stash resize, managed-panel geometry change, new layout leaf,
controller, command replay, tab selection, settings key or user-facing mode.

This changes effective visibility of the retained subtree, unlike 2b6c5c8 which
retained the top-level Edit controller but still parked its presentation children.
Unlike eb59502 it adds no navigation callback. Unlike 9eb69c5 it changes no viewer
wrapper. Media Pool transfer, vacancy suppression and toggle synchronization stay
as in the installed baseline. The current source is an experimental build base;
it is not a proposal to ship an invisible native tree as a final design.

One bounded addition to the existing loading snapshot records already-observed
UiConformerPanelImp visibility and ancestry when the experiment flag is enabled
(maximum eight controllers, ten ancestors; no global scan, clip payload, private
slot invocation or added per-input timer). This checks that the experimental
condition actually held at the double-click. It does not observe native execution.

## Prediction, falsification and acceptance limits

Prediction: with the full managed arrangement held fixed, double-click video A
and then B produces the correct distinct Source pictures, with no panel placement
change or repeated flashing. A title notification alone is insufficient.
If the controller ancestry is effectively visible at the click but the correct
pictures still do not load, visibility alone is insufficient: reject this proposed
repair and do not stack another reveal patch on it. If the intended ancestry
remains hidden, the manipulation did not establish the test condition; report
that exact hidden node. If pictures work but focus/blink/geometry regresses, the
experiment supplies causal evidence but fails product acceptance.

Hidden/covered Source reveal remains part of the first task. This experiment
deliberately does not add another reveal mechanism before establishing loading.
Correct pictures with Source already selected would not close acceptance 3.
Existing panes must ultimately reveal/select in place, including nested tabs.
Find and Match Frame remain separate, unresolved tasks.

Mask behavior on actual native/GPU surfaces and native focus reactions cannot be
proved by QWidget fixtures. No live test, installation or final repair acceptance
is claimed. The next external gate is authorization for an explicitly reviewed
candidate installation and a focused picture test; no repeated layout tour.

## Verification and contract mapping

Results and exact source hashes are retained in `build/macos/stash-evidence/`.
The initial sandbox ownership run could not access macOS screens; it is retained
as an environment failure. Hidden window-service runs test containment and the
existing assembled flow, not Resolve clip loading.

Numbered contract entry points: first-open PanelControls startup and child-process
restart (1); workspace Save/Update actions (2); unboundRestoreNativeWorkspace and
workspace selection (3); native/managed expansion controls and preset provider
(4); native title gesture (5,8); primary header/footer controllers and flat Panels
(6,7,9); page-stack departure/return (11); assembled first-open sequence (12).
The existing first-open/native-entry fixtures exercise these paths with the flag.
Pixel comparison of the masked root is synthetic containment evidence only;
native painting (10), physical inputs, complete clip-picture acceptance, blink,
and Windows execution remain live/platform gaps. Ordinary-launch/remapped hotkeys
and the command submenu have separate existing regression checks. Fixed Playhead
and selected-clip Stabilize implementations are unchanged, not newly live accepted.
Latest primary-worktree contract clarifications are preserved in this worktree.
Product direction is one consistent OpenGate UX with a protected Default Resolve
View; current UI labels are not renamed by this experiment.

Local result: six focused hidden suites passed (ownership/pixels, assembled
first-open with restart, Native entry, native presets, command submenu, input
diagnostics). The full helper built with arm64 and x86_64 slices. This is an
uncommitted experimental diff with file/binary hashes in verification.json, not
a clean release matrix. No installer was produced, no candidate installed, and
no Windows compilation or actual clip-picture check was performed.
