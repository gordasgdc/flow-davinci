# Native navigation reveal repair — October 26

Installed checkpoint remains 00e79a7-dirty with the ordinary-launch visibility
repair. Its exact source is now checkpointed locally at 5ad307b; that commit also
includes installation documentation, and is not a claim of a different installed
binary. Current candidate is 5ad307b-dirty on fix/native-loading-normal-launch.
No new candidate installation, merge, push or publication.

## Decisive live evidence

Ordinary-launch session 9d0065ee-587e-48da-92fd-ab780de21152 / PID 28898:
workspace.native-root.visibility-preserved at 681 confirms automatic activation.
Eleven source.navigation.skipped records (788 through 1895) say
not-abstract-item-view, class UiMp::UiMediaPoolClipView. The old standard-list
fixture could not represent this mismatch. Session evidence is preserved in
build/macos/ordinary-launch-live. Completing the requested actions is not itself
a report of correct pictures; no new picture acceptance is inferred.

Earlier flagged session 715f60a8 provides the native custom-view route: hidden
Source double-click 1339 -> mouse release 1342 -> btnSourceClipMode pressed/clicked
1344–1347 -> title notification 1348. Same-clip double-click 1367 -> release 1370
-> native Source Clip activation 1372–1375 without a new title notification.
Hidden floating Source repeats this at 1992 -> release 1995 -> activation
1997–2000 -> title notification 2001. Thus neither a title-change-only trigger nor
expiry at the double-click's first queued event turn fits the observed behavior.

Jordan believes Find locates the clip but leaves Media Pool hidden. This is kept
separate from a confirmed full Find pass. Match Frame was previously reported
working and is preserved; hidden/tabbed destination presentation remains required.

## Bounded implementation

NativePanelNavigation replaces SourceNavigationRecall. Its sole responsibility
is translating explicit native navigation into existing panel-door requests.
It owns no panel placement/visibility model and never invokes a loading command.
PanelControls supplies current page/transaction/session ownership and its existing
showPanel/save callback. The old SourceNavigationRecall implementation is removed.

- Standard item views retain the verified index-at-pointer route.
- A custom native scroll viewport arms observation of Source's native
  btnSourceClipMode activation during a left double-click. It observes, never
  clicks, the real control. A native activation queues the existing Source door.
- The gesture survives the double-click's first event-loop turn and native mouse
  release handling. It ends on the release's queued turn, another press, keyboard
  input or app deactivation. Repeated native activation queues once. Background
  updates without that explicit gesture cannot recall Source.
- Real QAction triggers named Find in Media Pool and Match Frame request Media
  Pool and Source respectively after native dispatch. Semantic native menu-label
  matching reuses nativeEditActionLabel; OpenGate-owned menus are excluded.
  Show Media Pool is not Find. No replay, synthetic shortcut or second controller.
- Before deferred reveal, current page, transaction generation and destination
  session identity must still match. Already presented panels are left alone.

Native Qt custom-viewport base compatibility and Source-control routing remain
live acceptance gates. If the guard reports unsupported-clip-viewport or missing
Source control, the manipulation did not establish its intended condition; do not
claim a successful repair. QAction observation likewise must be confirmed for the
real Find route; synthetic command execution proves dispatch mechanics only.

## Local verification and organization gate

The hidden assembled custom-view fixture passes hidden docked/floating/tabbed
Source recall, retained floating geometry and tree, repeated input, blank/right
input rejection and background activation exclusion. Its custom scroll viewport
loads on release after an earlier nested event turn, matching the captured order.
Native Find/Match Frame each execute once and reveal/select the retained target;
Find preserves the placement tree while changing the selected destination tab.
Show Media Pool is excluded. The suite also covers first-open, native return,
primary bands, protected built-in layout and unsaved restart.

Initial new fixture failures are retained: a dangling QJsonValueRef to a temporary
saved object caused a fixture crash; corrected to an owned QJsonValue. A comparison
then incorrectly disallowed the required destination-tab selection; it now excludes
only selected-tab fields while retaining structure/sizing checks. These were test
errors, not changes to the product requirement.

Navigation checks were moved into a focused helper using the existing assembled
fixture, not another harness. NativePanelNavigation is 134 lines; PanelControls is
1287. The old 55-line input component is replaced, not left parallel. Coordinator
changes are wiring/context only. Shared code remains shared; the macOS universal
helper and bootstrap compile/check successfully. Windows is not compiled or live
verified here. Logs: native-navigation-release.log, native-navigation-standard.log,
native-navigation-build-final.log, native-navigation-io.log.

Contract mapping remains the numbered native-entry contract: first-open/restart,
Save protection, Native return, gestures, independent bands, panel doors and page
state have assembled fixture coverage. Real pictures, correct native Find clip,
physical inputs, floating blink, GPU/maximize rendering, Relay/Deliver and Windows
remain live/platform gates. Native single-command execution is not picture proof.
The repair does not claim to fix the separately reported floating blink.

## Next live gate

Requires explicit candidate-specific installation authorization and Jordan closing
Resolve. Test double-click from hidden Source, Match Frame with Source hidden,
and the real Find command with Media Pool hidden. For each: correct clip/frame,
existing container/tab revealed, stable placement, and any blink recorded.
A successful result must include native output, not merely a reveal log.
Candidate source/binary hashes and review diff: build/macos/native-navigation-candidate/.

## Authorized installation of navigation repair — October 26

Jordan authorized and Resolve was verified closed. Installed 5ad307b-dirty; all
four helper bundle files match and all 24 protected settings/preferences are
unchanged. Receipt: build/macos/navigation-install/receipt.json. Recovery checkpoint:
/Library/Application Support/CineWaltz/OpenGate/Transactions/6A12E319-D283-4241-AF67-72BCB94BFDB4.
This supersedes the candidate-pending statements above. Live acceptance of native
double-click/Match Frame/Find output and retained reveal remains pending; floating
blink is not claimed fixed. No merge, push or publication.

## Live review after installation — October 26

Jordan reports: "find in media pool doesn't work. everything else does" in the
requested three-command test. Treat this as positive live evidence for the tested
double-click and Match Frame output/reveal, not blanket acceptance of all product
requirements or an explicit withdrawal of every historical blink report.
Session 69e2746a-3e4f-4e90-bd4d-07dc2c6ebc32 / PID 29588 / 5ad307b-dirty records
clean shutdown. Evidence: build/macos/navigation-live-review/session.jsonl and
user-results.json.

Match Frame: native action 856 -> queued reveal 857 -> complete 872; second test
1435 -> 1436 -> 1454. Custom Media Pool double-click: armed 1282 -> native source
activation queues 1289 -> existing Source door completes 1307. These traces
accompany Jordan's result, rather than substituting for correct native pictures.
Find has no navigation.native-action observation. workspaceShowPanelMediapool at
797/941/987/1046 is Show Media Pool, not proof Find executed or failed to select a
clip. Do not blame Jordan's invocation or change mappings from this observation.
The exact Find input/dispatch path remains to be identified; the QAction-based
adapter is incomplete for the reported sequence.

Architecture assessment: automatically preserving the parked native tree addresses
the observed full-layout visibility dependency introduced by OpenGate hosting.
It is an empirically supported compatibility technique, not proof of Resolve's
internal implementation. Source reveal replaces a demonstrably invalid standard
Qt-item-view assumption with a gesture-scoped native Source Clip activation
adapter. That adapter depends on Resolve control identity and release-time
behavior; both are evidenced for this runtime and remain compatibility risks for
other Resolve versions. NativePanelNavigation identifies explicit intent and uses
the existing single showPanel/save door; it neither loads clips nor owns a second
visibility model. The large coordinator only wires state and callbacks. Existing
standard-item fallback, Native return and ordinary-launch behavior are preserved.

Contract 9 is partially met: tested Source navigation works, Find remains open,
and Reverse Match Frame is a separate outstanding contract item. No contract
completion or main merge is claimed. Keep this checkpoint recoverable on the
feature branch; recommend identifying/fixing native Find before integrating the
whole navigation change. A future explicitly authorized partial merge must state
its exact accepted scope and leave Find open. No merge/push/publication performed.
