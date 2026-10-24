# Source presentation follow-up — October 24

Status: uninstalled candidate based on 435aa40. No live acceptance, no merge.
Worktree: source-retained-presentation; branch fix/source-retained-presentation.
Installed experiment remains 435aa40; Resolve session fa277ce7, PID 8239.

## Evidence and executed paths

Evidence is retained in ../native-stash-visibility/build/macos/live-loading-review/20261024T153914Z,
including rotated logs and screenshots. In session-latest.jsonl, double-click
1587 -> title notification 1597 and 1617 -> 1627 accompany Jordan's confirmation
of actual picture changes. This supports improved loading with the clipped,
effectively visible native stash. It does not establish complete acceptance.

Hidden docked Source: input 3303 -> title 3312; Source's group cell remains hidden.
Hidden floating Source: input 3899 -> title 3909; retained floating host stays hidden.
Those title events do not prove which clip picture is available behind the host.
The installed build has no explicit double-click destination-recall route.

Empty Source screenshot: native-transfer 5043 is followed by join-group 5060 and
group-ready 5070, both recording stackedEditSourceOfflinePlayers explicitly hidden
inside the correct managed cell. Match Frame 5162 and context 5163/5167 leave the
cell visible but its viewer stack hidden. The old presentation helper only repairs
the selected child if the outer stack is already visible. Recovery is suspended
during the transfer, so that boundary can leave an explicitly hidden stack behind.
This establishes a presentation defect, not whether Match Frame selected the right
source frame. Match Frame's actual loading remains a separate unresolved task.

Program hide/recovery pairs 3307/3314 and 3318/3319 execute during hidden-Source
input. Their existence does not identify the hide caller or prove the cause of
Jordan's visible blink. The precise missing observation is the originating native
or OpenGate call path for these hides, correlated with visible frame disruption.
No recovery loop is removed on speculation. No stale-version execution is shown.

## Bounded changes and falsification

1. At completed Source ownership-transfer and explicit door boundaries, clear the
outer stack's explicit hidden bit only when the session still owns it and the user
has not hidden it. Do not select a different native page or interfere with Cinema.
This predicts the existing cell will contain its selected viewer after transfer.
A stack that is visible but still paints blank falsifies sufficiency of this fix.
2. Observe an unconsumed left double-click on a valid item in the active managed
Media Pool clip viewport. Queue one in-place recall through the existing visibility
path only if Source is not presented. Revalidate page, workspace transaction and
session ownership. Retain floating hosts/groups; avoid window activation and
geometry correction. No native command replay, new controller, or title-triggered
background reveal. Blank-area/right clicks do nothing. This predicts the requested
clip will appear in the existing Source pane; stale/blank/wrong pictures, blinking,
or panel movement fail acceptance even if recall mechanics pass.

These changes supplement the installed loading experiment. They are not a repeat
of eb59502 offered as a loading fix: visible clip-picture loading now has Jordan's
positive evidence, while hidden presentation has separately documented failures.
No controller ownership experiment, wrapper removal, or architecture rewrite.
Native command behavior, grouping, embedding, tabs and persistence remain part of
the experience being tested, not merely a floating-monitor layer.

## Local verification and contract mapping

Before-change shared-session regression: two failures reproduce an outer Source
stack hidden during ParentChange, on grouping and returning to its retained float.
After-change shared-session suite: zero failures. Source-presentation and Native
entry-flow suites: zero failures. Assembled first-open suite with SOURCE_RECALL and
VISIBLE_NATIVE_STASH flags: passes hidden docked recall, hidden floating recall,
inactive Source tab selection, repeated-click layout stability, retained identity,
managed tree and floating geometry, plus its existing restart and Native-return flow.
These fixtures do not load video or establish absence of native GPU flashing.

Acceptance contract reread: startup/restart (1), Save/Update protection (2), native
return/workspace re-entry (3), maximize (4), direct native-origin drag (5,8), title
strip and independent bands (6,7), flat panel doors (9), page persistence (11) and
assembled flow (12) are covered by existing first-open/native-entry fixtures.
Contract 9's native Match Frame/Find navigation remains incomplete; actual target
clip/frame identity is not proven. Native painting (10), real gestures/remapped
hotkeys, playback and Windows execution remain live/platform gaps. Fixed Playhead,
Stabilize and submenu implementations are unchanged, not newly live accepted.
Startup screenshot restored the prior native-remainder comparison checkpoint;
it is not evidence of a clean first launch or failure restoring a saved full layout.

No candidate installation or new live test is claimed. The product is not ready
for main: actual A/B pictures with hidden/tabbed/floating Source, absence of blink
and layout movement, Match Frame/Find, and persistence still require acceptance.
