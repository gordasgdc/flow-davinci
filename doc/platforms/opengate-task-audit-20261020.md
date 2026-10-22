## Current integration review — caller state and Keyframes (2026-10-22)

User authorized implementation, isolated feature commits, combined verification
and installation of the correct candidate. Resolve is currently open: do not
replace its loaded helper or terminate it. Installed helper remains 53e23ee,
verified by SHA-256 d7e4c40820665b6e13fc53049296c21dbd6a52b1e4516c433cef5eb68810e6ce.
No merge to main, publication or prototype release is authorized by this repair.

Integration: integration/panel-callers-keyframes-live-candidate, built on 53e23ee.
- fix/panel-caller-state-sync: native checked notifications cannot generate panel
  commands; radio-like checks reflect selected/visible presentation. Header input
  calls the existing controller instead of operating leased native slots. Toolbar,
  Panels and workflow dispatch share the presentation query; queued recalls are
  invalidated across workspace transactions. Placement remains separate.
- fix/keyframes-panel-parity: reuse the native Keyframes btnUndock through a small
  NativePlacementCaller adapter, with the existing panel/session/group/expansion
  controllers. No parallel Keyframes host or content implementation.
- New assembled caller regression fails five checks against 53e23ee and passes
  the repair. A new Keyframes regression reproduced its native tray bypass before
  the placement adapter; repaired checks pass floating hide/recall, tab grouping,
  standalone/grouped maximize restore, page return, process restart and Native
  ownership return. These are synthetic Qt checks, not live Resolve acceptance.

Full clean-commit hidden matrix, production build and assembled Native entry
review remain pending. Displayed fixtures and replacement require Resolve closed.
The native GPU/keyframe content, actual pointer capture and physical remapped-key
behavior need live acceptance. Source/Program visual fit remains live review.

Public labels should be OpenGate. Internal Unbound identifiers/settings paths and
Wolfgang's inherited attribution remain. The exact reported “Unbound Test” label
has not yet been located; do not claim it fixed. Public release cleanup, capability
compatibility checks and repository migration remain separate follow-ups.
Windows implementation/release work follows the user's Mac flow acceptance;
no Windows install is possible here. Future prototype notes must distinguish Mac
live evidence, Windows build evidence and external confirmations.

## Live panel-caller follow-up (2026-10-21 after 53e23ee installation)

Open: Media Pool/Index automatic visibility feedback loop in OpenGate Resolve
Layout, floating Media Pool recall/placement inconsistency, and toolbar checked
states not matching actual presentation (including visible Mixer). Logs from
session 2370aadd-a044-4120-9153-cbecb206ba25 show repeated tab selection between
Media Pool and Index without corresponding input clicks. Do not mark live flow
accepted on the strength of the prior fixture passes.

Add Keyframes to this same acceptance sweep. It is already registered as
frameEditPageKeyframeEditor / KeyframeEditor / contents-session in PanelControls;
registration alone does not establish working native surface discovery or behavior.
Verify its real widget, detach control and title dragging, floating and grouped
show/hide/recall, toolbar/native caller and checked-state synchronization,
backtick maximize/restore, saved placement and Native/OpenGate/page transitions.
Reuse the existing panel/session controllers; do not build a parallel Keyframes UI.
No live Keyframes acceptance has been established.

## Current follow-up: Native to OpenGate restore repair (2026-10-21 evening)

Branch `fix/native-workspace-entry-transaction` includes the viewer toolbar fix.
The installed `dbb5c8d` is still the live build; this repair is not installed,
merged or published. Live acceptance remains open.

Evidence: the installed session repeatedly dispatched Inspector through invisible
`btnEditToggleInspectorInEFTM`, then timed out. The user's visible SSM Inspector
click triggered Resolve's native action; Resolve Layout and Jordan then logged
complete restoration. The recorded hierarchy puts EFTM/DSM in non-current toolbar
stack pages. The new assembled regression reproduced acquisition failure before
the repair (three assertions failed).

Repair reuses the existing native control and workspace coordinators:
- Select the current native toolbar branch, including when the complete band is
  hidden; reject inactive variants. Reconcile a mirrored checked state against
  actual target visibility before one native activation, then await real readiness.
  No Inspector-specific click, native ancestor force-show or new UI layer.
- Suppress ordinary session vacancy/visibility reconciliation during native return,
  and vacancy collapse while the managed plan is still acquiring its panels.
- Keep checkpoint writes deferred through forward restoration and asynchronous
  rollback. Completion is connected to deferred native-plan mounting. Preserve
  the prior selected workspace alongside its layout; selection requests alone do
  not persist. Capture unsaved current geometry before beginning a new request.
- Readiness timeout fails acquisition instead of trying to detach an unavailable
  surface. A superseded forward pass cannot mount after rollback starts. Rollback
  failure blocks checkpoint overwrite and reports its own failure, without recursion.

Focused assembled checks pass for stale checked states, inactive EFTM preceding
SSM, mutually exclusive sidebars, hidden-toolbar restoration, failed managed
restore returning to the prior tree and selection, and failed Native restore
leaving no partial floating hosts. Hidden-band comparison checks tree/tab identity
and split proportions: captured pixel heights grew from [333,332] to [372,371]
when the band was hidden, as required. Coordinator checks cover pending rollback,
rollback failure and save protection. Broader final validation/build receipts are
recorded under `build/macos/native-entry-repair-verification`; fixture evidence
cannot establish actual Resolve initialization or native rendering acceptance.

Required live check after reviewed installation: close Inspector in Original
Resolve UI, select Jordan and Resolve Layout without manually opening Inspector,
repeat with Workspace Toolbar hidden, switch projects/pages and verify the saved
tree, floating placements, selected name and absence of native background leakage.
Confirm both monitor callers are visible and preserve their panels' locations.

## Current follow-up: visible monitor callers (2026-10-21 evening)

Branch `fix/visible-viewer-toolbar-callers` corrects the existing toolbar accessory,
without adding a second toolbar or changing panel caller callbacks. Its controls
used an Ignored horizontal size policy and only a maximum width; the layout could
allocate zero width despite the outer strip having room. Assign the calculated
bounded width to the controls so Source Monitor and Program Monitor are visible.
Existing label/icon/overflow selection remains in place.

The assembled native-entry fixture now checks actual visible button and controls
width. It fails on the previous sizing (one failure) and passes with the correction
(zero failures). This is fixture evidence, not live Resolve acceptance. Not installed,
merged or published. Native-to-OpenGate acquisition/transaction repair remains open;
this toolbar correction does not resolve that separate failure.

# OpenGate task audit — 2026-10-20

This is the current acceptance list reconstructed from Jordan’s requests, source,
installation receipts, branch state and reported live failures. Historical
checklists and build notes are evidence, not proof that this list is complete.

**Task-list review completed; implementation is resuming from this list. Further
installation, publication, merge to main and live-project changes remain on hold.**
Jordan required this review before another install. Do not continue the pending
temporary-project proposal as if it were approval.

## Displayed-test repair — floating restore continuation

The displayed candidate run caught real restore failures: sequential acquisition
only scheduled its next step for a pending managed native plan, so floating-only
workspace/group reload could stop after its first panel. Fixed continuation to
advance while any desired session is still inactive. Pending groups also block
checkpoint replacement, and final group assembly runs the completion reporter.
Focused eleven-panel session, group reload and native-workspace UI suites now pass.
The isolated reliability rerun passes with the application active (prior displayed
run reported inactive state and focus assertions failed). Final clean-commit
combined displayed run remains required. Retain the failed displayed receipt;
no repair from these branches has been installed or merged into main.

## Native-binding fixture correction — supersedes timing-only diagnosis

The clean 978bcce follow-up still failed, so fixed waits were not sufficient.
Inspection found fixture diagnostics, generated workflow scripts and keyboard
presets shared one watched temporary directory. Creating those unrelated files
could invalidate NativePresetBindings before queued dispatch. Production separates
these directories. The fixture now mirrors that separation, isolates legacy
Shortcuts.json too, and retains bounded expected-count delivery and per-key state.
The isolated corrected fixture passes. Production shortcut code is unchanged.
Earlier passing timing-only runs did not establish the cause; retained failure
receipts remain part of the evidence. Final clean follow-up and displayed checks
are still required before installation review.

## Combined candidate verification — review pending

Production source 47f747a: 40/41 hidden suites pass; native-preset-bindings retained
an intermittent synthetic-input failure. Single processEvents / MetaCall delivery
was insufficient: a later preset rewrite could precede queued dispatch. A short
fixed wait also failed intermittently. The fixture now waits for its explicit
expected invocation counts with a one-second deadline while processing events;
negative cases still drain events and assert no invocation. Per-key focus/counter
diagnostics retained. Two isolated runs of the bounded fixture pass. No production
shortcut code changed. Final clean-commit follow-up and displayed suites pending.

Helper, bootstrap, launcher and universal architecture audit passed at 47f747a.
Task scope is still preinstall review, not installation or main merge. Physical
multi-display/project-switch behavior and user preset visual match remain live gates.

## Verification follow-up — display policy and failed acquisition

Combined 6b27fd6 matrix: 40/41 suites passed; panel-commands exposed clamping of
intentional partial off-screen placement on an unchanged display. Corrected policy
preserves that placement and clamps when display bounds change or disappear.
Focused display and panel-command fixtures pass. Native-frame-adjusted fallback
coordinates are recorded separately so automatic saves preserve original identity.

Added rollback on preset acquisition failure: capture prior complete page checkpoint,
release partial hosts and resume that view; clear the request identity before rollback
to prevent recursion. A fixture that disables Media Pool activation passes: prior
Native view returns and no partial floating hosts remain visible. Existing Original
Resolve UI command is unchanged. Final clean-source matrix/build still required.
The multi-display feature currently concerns OpenGate window placement; toggling
Resolve's own native Dual Screen configuration is not implemented or claimed.

## Display-aware workspace follow-up — implementation under verification

Branch feature/workspace-display-placement builds on the viewer-button/preset
branch. Shared display-placement policy records logical screen identity and local
coordinates without assuming two displays. The macOS Qt adapter supplies screen
inventory. Floating panels, groups and primary-window snapshots retain placement;
Qt-maximized state is stored separately from normal geometry. Missing-display
fallback preserves the original record during automatic saves; intentional moves
record a new placement. Legacy coordinates remain readable.

Isolated tests pass four-display placement, negative desktop coordinates, display
reordering/relocation, lower resolution, missing-display fallback, unchanged saves,
reconnect and ambiguous-identity geometry tie-breaks. Physical multi-display and
Windows adapter acceptance are not established. Independent page-state fixture
passes. Assembled flow, final matrix and production build remain pending.
No install before Jordan reviews; preserve the installed main baseline and local presets.

## Toolbar/preset follow-up evidence

Branch feature/workspace-toolbar-viewer-buttons integrates the isolated restore fix.
Existing Source/Program callers are reused, moved into available space in the same
native toolbar rather than the bounded native left-button row. No duplicate caller
or panel commands added. Hidden assembled entry fixture passes readable allocation,
constrained-width overflow, native show/hide, drag and Native-return behavior.

Bundle now contains only immutable Resolve Layout and the captured saved Jordan
Tetewsky Preset (stable shipped id opengate.preset.jordan-tetewsky.edit). Other
local arrangements are untouched. Fresh-profile and prototype-installer fixtures
pass visibility, single user-preset inventory and no-overwrite checks. No package
was built, installed, merged to main or published. Preset rendering still needs
Jordan's live comparison after review. Multi-display work follows separately.

## Follow-up after approved baseline — preset restore failure

Jordan reports selecting saved OpenGate presets or Resolve Layout after a project
switch leaves separate floating panels with the native workspace exposed. Original
Resolve UI recovery works. The best-so-far tag remains a preserved checkpoint,
not a claim that this newly reported case works. No repair installed yet.

Incident logs at 22:01 UTC show Media Pool activation fails, no native-plan assembly
request follows, yet arrangement.restore is logged. The saved Jordan Tetewsky
Preset exists with a managed tree, proportions, tabs and visibility; backed up
locally along with logs. Do not overwrite it or ask the user to rebuild it.

Repair isolated on fix/project-switch-workspace-restore: sequentially acquire
native surfaces so mutually exclusive sidebar activation cannot hide another
pending surface; resolve native callers from the main window (toolbar may be a
page sibling). Resume assembly after each acquisition. Report completion only when
required sessions/groups are ready; preserve last complete checkpoint while pending.
The new deferred-sidebar regression fails against the installed baseline and passes
with the repair. Assembled first-open and restore ordering checks pass. Live acceptance
and broader checks remain pending.

Next requirements: each page retains its own mode/unsaved state; unsupported pages
stay Native; an OpenGate arrangement must not leave native background or placeholders
exposed. Bundle only Jordan's saved preset, retain Resolve Layout. Existing viewer
callers need visibility/overflow review. Multi-display snapshots must preserve screen
identity and placement across n screens with reversible missing-display fallback.
No install before user review; next install must use a backed-up fresh-user profile.

## User-approved baseline — 2026-10-21

Jordan reports installed 32e3be1 is working very well and explicitly approves
merging this basic-flow checkpoint to main, pushing to the personal OpenGate
repository and tagging it as best so far. This supersedes earlier merge/publication
holds for this checkpoint only. This is user-reported live acceptance, not a claim
that every prior case or platform has been tested. Installed source: 32e3be1;
startup helper match and Ready verified. Fresh-profile reset did not complete;
do not claim the current session was a pristine first launch.

Next isolated branch: add Program Monitor and Source Monitor buttons to the
existing Workspace Toolbar. Discuss placement/behavior before implementation;
reuse existing panel commands and native styling. Keep this approved baseline.
Fusion and platform validation remain separate. Private clip-color notes stay private.

## Latest status — 2026-10-21 preinstall repair review

This section supersedes historical “current state” and install-authorization notes
below. Installed helper remains c033f3d, a failed live candidate. Resolve is closed.
Repair branch: feature/macos-opengate-default-flow. Nothing newer is installed.
091268b is the recorded known-best baseline; it is not proven to be the exact build
in the “closest to good” screenshot. Preserve it and all user-state backups.

8621617 passed 39 hidden suites; displayed checks passed 6/8. Panel-groups retained
obsolete independent-Timeline-toolbar assertions and an unsafe narrow-Timeline
expectation. Reliability exposed a real repair defect: the empty-transfer-host
check also prevented ordinary widget recovery after native reparenting. Narrowed
that guard to transferred layouts; the displayed reliability suite now passes.
Updated group assertions preserve the attached toolbar and native painting minima.
Final clean-source checks and production rebuild remain required before install.
Live Timeline painting, divider behavior and latency remain acceptance gates.

On the next authorized install, back up current OpenGate state, then present the
first-user Resolve Layout experience without old arrangements/setup controls.
Do not delete user work or alter Resolve keyboard mappings. No publication.

## Historical state — earlier 2026-10-21 flow review

- Installed runtime remains **6f395c4**. No changes in this flow review have been
  installed, merged to main, pushed or published. The known-best **091268b**
  software checkpoint remains preserved independently of current user settings.
- Candidate branch `fix/macos-workspace-viewport-maximize` includes separate preceding
  menu, native header-drag and native footer-band commits. This is a development
  candidate, not an accepted release.
- Final basic-flow production build **d717d12** passes all34 hidden and all8
  displayed suites on the clean unchanged commit. Exact receipts are below.
  The assembled test includes direct native controls, all four band combinations,
  Native return, grouped maximize, smaller-workspace resizing and process restart.
- Universal helper/bootstrap/launcher and30 workflow targets built; signature,
  architecture and matching-dSYM checks pass. These are local fixture/build checks.
  No new live Resolve startup, native pointer/rendering, physical-key latency or
  Relay/Deliver acceptance is claimed.
- Maximize decision is settled: the current workspace viewport between visible
  bands, expanding into either hidden band's space. The viewport follow-up is
  implemented and passes the strengthened assembled fixture; see latest evidence
  below. This supersedes older monitor-gate notes.
- Fusion panel docking/arrangement backend, full native Timeline body scaling,
  GitHub fork separation and live Windows/older-macOS compatibility are **not done**.
  These were not completed by the Native Edit flow fixes.

## Historical state at original audit (superseded)

- Installed runtime: **541cd25**, branch `fix/macos-visibility-independent-commands`.
  It was installed before the latest request to stop and audit; do not describe it
  as merely built or as an accepted release.
- Installation receipt: `build/macos/command-test-install-receipt.json` in that
  worktree. The prior helper, Stabilize script, settings and native keyboard
  preference files were backed up under `build/macos/checkpoints/pre-541cd25-20261020T150740Z`.
  Installer hashes verified arrangements and native keyboard preferences unchanged.
- Fresh Resolve startup logged helper 541cd25 and the active native keyboard
  preset without parse errors. This is startup evidence only. The last read-only
  API query reported Media page, project open, no current timeline.
- This session was launched with `OPENGATE_NATIVE_PRESET_SHORTCUTS=1` and
  `OPENGATE_TIMELINE_TAB_SCROLLING=0`. **Hotkeys are not enabled by ordinary launch
  without the opt-in flag.** Closing/reopening normally is not equivalent to this test launch.
- Remote `personal/main` remains **3c9b2a7**, landing-page documentation on the
  published prototype. The release is unchanged. New command work is on separate
  feature/fix branches, not main. Never push to Wolfgang’s `origin` remote.
- GitHub reports `fork: true`, parent `wolfgangw/unbound`, default branch `main`.
  Fork separation has not happened.
- The tab-scrolling improvements are **uncommitted** on
  `feature/macos-timeline-tab-scrolling`, based on 92e6e6c. Its latest isolated
  fixture passed. They are not in installed 541cd25. Preserve these edits.

## Current work order — Jordan's instruction

1. Finish the basic Native Edit user flow, including workspace-viewport maximize,
   run the assembled checks, then review honestly before installation/live testing.
   Jordan will live-test this basic-flow build first. Consider merging that verified
   baseline to main before beginning Fusion; no automatic merge or release claim.
2. Implement Fusion using the same interaction structure, in separate functional
   branches. Keep layouts page-aware; do not invent cross-page panel ownership.
3. Address Wolfgang's branding/artwork, licensing presentation and standalone
   repository requests after Fusion. Preserve inherited notices and all work.

The reported Resolve Free missing-menu case and installer coexistence/capability
checks remain release-compatibility work; Free support is not claimed. This order
supersedes any suggestion to interrupt basic flow/Fusion for branding cleanup.

## Status meanings

Implemented means source exists. Fixture-tested means an isolated modeled test
passed. Live-verified means the requested behavior was observed in actual Resolve
on the identified build. An unchecked acceptance item remains open even if its
implementation or fixture is complete. A successful command dispatch is not proof
that stabilization or scrolling actually happened.

## Commands and crashes

| ID | Requested behavior | Reviewed status | Remaining acceptance / work |
| --- | --- | --- | --- |
| C1 | Stabilize works with Inspector hidden, using selected timeline clips | Implemented in 541cd25: original native workflow action runs the internal selected-clip API script; no visible-button fallback. Seven API tests, generated-wrapper tests and hidden-Inspector routing fixture pass. | **Open:** actual native workflow-host execution from menu and remapped hotkey with Inspector hidden. Check selected video, multiple selected items, no selection, audio-only selection, outside Edit and real completion logs. The earlier user success with Inspector visible is not acceptance of this replacement. |
| C2 | Fixed Playhead toggles Edit scrolling with either state, independent of panel visibility, focus or clip selection; excludes Cut | Implemented in 541cd25: native menu preparation signal, semantic custom-painted row lookup and its actual QAction. Ineffective global scrolling QAction removed. Hidden page/menu/row fixture passes without focus movement. | **Open:** native signal preparation, row discovery, actual scrolling on/off, hidden Timeline, other-panel focus and menu/hotkey parity in Resolve. Qt checkmark changes alone do not pass. The shared shortcut filter still blocks text editors/dialogs; review modified-key focus behavior rather than silently promising every focus context. |
| C3 | User-remappable keys; native keyboard editor remains authoritative | Native preset reader and live file watcher implemented. Fixture now routes real Stabilize command registration through a native workflow action, including old-key removal, alternates, floating window, preset switch and parse recovery. Earlier live logs proved a user remap reached the registry. | **Uninstalled fix:** ordinary-launch adapter implemented on this branch; full helper/bootstrap build and native-preset fixture pass, including a subsequent rerun. Installed 541cd25 still requires opt-in. Live save/switch timing, keyboard layouts and exactly-once operation remain unaccepted. No hard-coded Option+Shift bindings may substitute for the user preset. |
| C4 | Commands appear in a sensible submenu as well as keyboard customization | Current native workflow scripts remain in their original menu locations. OpenGate’s shortcut entry opens the native keyboard editor. | **Built, uninstalled:** branch `feature/macos-command-submenu`, commit 7dd6a60, adds an independent Commands menu delegating to shared IDs. Command-menu and workspace-menu fixtures pass; full helper compiles. Cocoa/live backend acceptance pending. Native workflow actions stay in place. |
| C5 | One reusable maximize/restore operation; backtick and per-panel commands use it | Registered panels share `PanelExpansionController`; hovered and explicit panel commands exist. | **Partial:** verify exact geometry/group/tab restoration and target behavior live. Current hover lookup does not implement the historical checklist’s last-active fallback when no panel is under the pointer. It is not a generic maximizer for every arbitrary native Resolve window. Clearing a native hovered-panel assignment can re-enable the legacy backtick fallback; clearing/remapping semantics need explicit acceptance. |
| C6 | Existing native assignments and contextual commands keep working | Page-aware conflict handling and text/dialog guards are fixture-tested. Native preset input is read-only. | **Open:** user’s real conflicts, Source/Program independence and native shortcut behavior in main/floating layouts. Alt+Shift+W has a known native collision; do not silently override it. |
| C7 | Full command names readable in Keyboard Customization | User screenshot shows native Command-column elision, including OpenGate labels. | **Implemented, uninstalled:** branch `fix/macos-keyboard-command-readability`, commit addc855, wraps command text with computed multiline row heights and full-name tooltips. Narrow-width, row-separation and teardown fixtures pass; rendered fixture inspected. Native custom-delegate acceptance remains open. Registered labels and binding identities are unchanged. |
| S1 | Stop the shortcut/menu crash regression | Native workflow regrouping and host mutation removed; observer fixtures pass. Reported helper stacks implicated the old mutation path. | **Mitigation, not proven resolved:** repeated real menu/editor opening, key use, remapping and lifetime/teardown checks without a new matching crash. Own-menu refresh behavior also needs actual Cocoa acceptance. |
| S2 | Investigate Fairlight crash and reliable page switching | Evidence review complete: reported fault is native Fairlight code, no helper frame in that stack. Unsafe synchronous Edit Hide ownership work was deferred; fixtures pass. | **Open:** root cause not established. Cycle Edit/Fairlight/other pages, native context menus, project reopen and layout restoration. Do not claim the defensive ordering change fixes the reported crash. |
| S3 | Log evidence that can identify OpenGate’s involvement in crashes | Build/PID/session, breadcrumbs, clean-shutdown state and bounded native archive correlation implemented. Matching binary/dSYM retained for earlier implicated incidents. | **Open live acceptance:** verify useful incident output and timestamps; time correlation is not causal attribution. Do not replace Resolve’s crash handler. |

## Headers, layout and scaling

| ID | Requested behavior | Reviewed status | Remaining acceptance / work |
| --- | --- | --- | --- |
| U1 | No overlapping project title and added controls in Native Edit UI | Bounded OpenGate accessory container, labels/icons/overflow thresholds and title-space fixture implemented. | **Reported defect still open:** verify Native Edit UI, especially after Workspace Toolbar removal/float. Do not treat the working Native Emulator screenshot as proof this case is fixed. |
| U2 | Compact one-strip attached and floating manager; no wasted title row; floating defaults pinned | Compact toolbar wrappers, standalone-strip native chrome and remembered default pin are implemented. | **Open:** real narrow/wide attached/floating layouts, moving/redocking and no wasted space. User reported difficult docking above windowed groups. |
| U3 | One clear placement symbol with understandable states; respect Wolfgang’s artwork request | macOS uses OpenGate’s independently drawn gate/arrow glyph through shared placement controls. Original licenses preserved. | Code audit complete. **Open visual acceptance:** verify every toolbar/header/menu route uses the same symbol and descriptions; do not claim entire UI consistency from one shared class. |
| U4 | No duplicate Timeline / Program Monitor labels or redundant per-panel controls | `GroupPanelHeader` hides the separate name row when tabbed and moves controls into the selected tab strip; old native-added headers are hidden in managed groups. | **Open:** native layouts, native nested headers, tabs, switching selected members and other pages. A native label plus an OpenGate label can still require targeted diagnosis. |
| U5 | Title shortens by available width, full title on hover, controls stay stationary | Outer native header title truncation and full tooltip implemented; fixtures pass. Panels button is bounded to its title-strip height. | **Open:** Native Edit UI’s separate native project label, long/renamed titles, display scales and floating strips. No hover expansion/animation into controls was implemented or promised. |
| U6 | Cramped controls remain reachable using scroll/overflow, across other panels/pages | Sidebar/toolstrip overflow wrappers and several reachability fixtures implemented. Added OpenGate doors compress; original native controls are preserved. | **Incomplete coverage:** Media Pool, both viewers, Timeline toolbar, Inspector and other pages need narrow-width review. Existing native controls can still clip; added OpenGate controls can disappear at zero accessory budget. Do not claim an application-wide fix. |
| U7 | Inspector reset controls accessible; cue to scroll when too narrow | User located reset buttons; no missing-reset bug established. Horizontal overflow hints added to Inspector; fixture passes. | **Open:** live discoverability, scrolling to all reset controls and hit testing at small widths. Do not invent a reset-button restoration fix. |
| U8 | Sensible UI scaling, including cramped Timeline | Native font/control/layout metric scaling and percentage/reset controls exist. | **Not complete full-panel scaling:** custom-painted Timeline tracks/GPU surfaces are not covered. User reported toolbar/tab scaling while tracks stayed unchanged. Confirm the intended product scope and verify native input/rendering and overflow. |
| U9 | Page Navigation behaves as a native-like footer: attached/permanent or intentionally hidden, remembered | Timeline attachment, hide/show callers and persistence/reopen fixtures exist. | **Open:** actual page switches, independent detach, native restoration and restart. No extra blank footer/header rows or lost recall path. |
| W1 | Multiple magnetic surfaces, each header controls its contents; global bar can recall all; callers stay synchronized | Shared panel caller/visibility owners, multiple group hosts and per-window/global header menus implemented. | **Open:** at least two independent floating surfaces, hide/restore, native caller state, no duplicate traffic lights and selected-tab preservation. Broader native anchoring claims require live evidence. |
| W2 | Existing named arrangements retain mounted elements instead of floating duplicates | Stable panel IDs, saved trees and native/group owners implemented; many restoration fixtures exist. Installation preserved saved arrangement bytes. | **Reported regression still open:** restore each existing user arrangement, compare intended mounting, splits, selected tabs, hidden states, pinning and scale. Unchanged install hashes do not prove correct runtime restore. Never overwrite a named preset automatically. |
| W3 | Reliable float/detach/redock, including manager above a windowed group | Existing placement and docking owners implemented with restoration fixtures. | **Open:** reproduce disappearing-on-undock and difficult toolbar docking reports, verify drag/release targets and recovery paths. |

## Repository, experiments and release

| ID | Requested behavior | Reviewed status | Remaining acceptance / work |
| --- | --- | --- | --- |
| G1 | OpenGate name, correct GitHub landing branch, restrained attribution and no lost code | Verified OpenGate identity, main default, landing-page update, feature branches, MIT and historical source preservation. Full-history bundles, release files and GitHub metadata backups exist. | Complete for default-branch/name preservation. Refresh preservation snapshot before any irreversible repository step; check uncommitted branch work separately. |
| G2 | Proper independent OpenGate repository, respecting Wolfgang | **Not done:** GitHub still reports a fork of wolfgangw/unbound. | Review the exact Leave fork network confirmation and its metadata effects against backups. No delete/recreate or force-push solution. No message to Wolfgang/support is authorized. |
| G3 | Easy Mac install and lower minimum OS in a later pass | Published prototype package retained unchanged; current exact artifacts require macOS 27. | **Not fixed:** lower deployment target/compatibility requires a separate build and actual supported-OS testing. Do not relabel the existing binary as compatible. Signing/notarization also remains outside the current prototype’s delivered guarantees. |
| G4 | Merge accepted work branch by branch, without regressing Stabilize | Branches/checkpoints preserved; main has only prototype plus landing docs. | **On hold:** new Stabilize, Fixed Playhead and UI work are not live accepted. Do not merge or publish merely because they compile or fixtures pass. |
| T1 | Timeline-name tab scrolling in addition to native arrows, isolated and removable | Separate branch has scrollbar/wheel adapter. New uncommitted fix passes two-way drag monotonicity, small trackpad-delta accumulation, selection preservation and teardown fixture. | **Open experiment:** native arrow route remains discrete, not pixel-smooth. Live rename/font/resize/close/add tab coverage and usability needed. Improved branch is not installed/merged; must remain independently excludable. |
| P1 | Responsiveness LAST, investigate before optimizing, stop/discuss if scope grows | Prior bounded synthetic viewer-latency report exists; it does not establish input-to-video or export/playback improvement. | **Explicitly ON HOLD.** No new responsiveness optimization, performance spiral or claim about Thunderbolt/storage cause. Revisit only after higher-priority acceptance and an agreed bounded measurement. |

## Before another installation

- [x] Read back the full audit, correct stale status notes and present the open items honestly. Implementation may now resume; this does not approve another installation.
- [ ] Resolve source gaps that were missed: ordinary-launch hotkey policy, safe command submenu, focus/backtick clearing semantics, unresolved header/overflow and arrangement reports, with a clear scope for the next candidate.
- [ ] Review candidate contents by branch and record what is included/excluded; preserve the tab experiment separately.
- [ ] Record exact fixtures and their limits. Do not label C1/C2 successful from dispatch logs alone.
- [ ] Plan live acceptance without changing working clips or closing a user session. The temporary-project question is pending; it is not approval and no such test has run.
- [ ] Confirm build identity, rollback and protected settings before any later install. Installed, built, pushed, merged and published are different states.

Historical custom-color work and unclosed low-level workspace tests remain in
`macos-workspace-checklist.md` and `opengate-next-pass.md`; they have not silently
become completed. They do not authorize additional scope or responsiveness work.

## Uninstalled follow-up: normal-launch hotkeys

Branch `fix/macos-normal-launch-hotkeys` changes the native preset provider to
start without a launch flag, retaining `OPENGATE_NATIVE_PRESET_SHORTCUTS=0` as a
recovery opt-out. Its fixture passes ordinary launch, remap/preset-switch dispatch,
shared maximize command dispatch and clearing the native maximize assignment
without resurrecting a legacy key.

This branch is not installed. Important behavior to review before integration:
a native maximize command row with no assignment means unassigned. If the current
preset contains an empty row, bare backtick will need an explicit native-editor
assignment; silently retaining an independent binding would contradict clearing
semantics. Do not describe this as preserving every previous backtick default.

## Latest runtime investigation (2026-10-20, PID 40430)

The user's subsequent normal restart still loaded installed 541cd25, but no
`command.preset.loaded` event appears for that process. The installed version
requires the opt-in launch flag; this explains the lost adapter path on normal
restart. One Stabilize result reports attempted=1, completed=1, success=true at
15:34:41 UTC. Its origin is not established by that result alone. Fixed Playhead
failed independently with two matching settings owners at 15:40:37 UTC. Do not
select the first owner or infer successful native mode switching.

Normal-launch branch bb178ce has now compiled the full helper and passed bootstrap
checks. The native-preset fixture passed on rerun; an earlier hidden activation
run failed dispatch assertions, so fixture activation stability remains under
review. Nothing from this branch has been installed or merged.

## Current priority clarification

Responsiveness is excluded from active work per Jordan's latest instruction.
The remaining task list is not complete. Duplicate Fixed Playhead ownership has
a source fix on its isolated branch (8a72808): Edit/timeline ancestry scopes the
settings owner and menu; genuine ambiguity still fails closed. Duplicate-owner
fixtures pass and the full helper compiles. This does not prove the current
Resolve layout has a unique scoped owner or that scrolling changes correctly.

Normal-launch hotkey fixture passed again after the earlier activation failure.
Treat that first failure as a test stability concern, not evidence of a confirmed
production command regression. No new installation or live project changes.

## Assembled priority candidate

The four install priorities are assembled on
`integration/macos-priority-live-candidate`, runtime f03d165, with installed
541cd25 as ancestor. See `opengate-priority-live-candidate.md` for branch contents,
passed combined checks, limits and live-test steps. Header and overflow source
gaps now have candidate fixes and passing fixtures. Native acceptance remains
for the next live test; it is not a prerequisite to producing that test build.
GitHub standalone conversion and responsiveness are outside this candidate.
No new installation, publication, main merge or live-clip operation has occurred.

## User acceptance and main merge

After installing a6fdcf6, Jordan reported Stabilize and other hotkeys working and
authorized merging to main. This supersedes the earlier main-merge hold for this
candidate. It is not blanket acceptance of every UI/page/restoration case. The
installed runtime and feature branches are retained; main receives the combined
candidate with the repository landing page and upstream notices preserved.

## Isolated Source docking live test

Jordan requested installation for testing after the arrangement investigation.
The candidate is based on main plus 71a2886, retaining the installed a6fdcf6
hotkey/UI foundation. The only production-source delta from that installed
build is NativeArrangement's retained initialization-page identity.
Footer preflight ad7780a is explicitly excluded because its broader regression
checks failed. First-click arrangement restore is still an open issue.
The new Source ownership fixture passes; compile/bootstrap, native-preset and
full panel-session checks are required before installation. Preserve current
settings and snapshot the installed runtime for rollback. 0.0.2 remains on hold.

## Surface footer redesign requested

Jordan requested a build of synchronized surface footers: default at the bottom
of the primary docked surface, optional on other surfaces, accessible through
Panels, synchronized to native Show Page Navigation/hotkey, and never truncated.
Work is isolated on feature/macos-surface-page-navigation, based on installed
ca505c1. It retains hotkeys and Source docking and excludes failed footer-preflight
experiment ad7780a. The new design removes native-footer lease/reparenting rather
than weakening ownership guards. See opengate-surface-page-navigation.md.

Hidden navigation/migration checks pass; full group and cross-process restart
checks are compiled but deferred while Resolve is running. Build is requested;
installation, main merge and 0.0.2 publication remain separate. Responsiveness is
excluded. Do not mark W2/U9 or the historical restore failure live-accepted yet.

Surface navigation build review: hidden fixture covers duplicate inactive native
page-button modes and keeps Home outside the overlay. Native preset tests still
pass. Full helper/bootstrap build passed for the first implementation; the final
primary-surface refinement is being rebuilt. No installation or main merge.

## Pinned footer band build

Jordan refined the request to pinned/unpinned reserved bands, with no footer
docking and normal independent-window overlap. Production footer float/dock/lease
paths are removed. Hidden reservation, unpin/reclaim, content replacement, native
main/secondary visibility, migration and hotkey fixtures pass. Displayed group
and cross-process restart fixtures compile, but cannot run while Resolve is
open. Build and save the isolated branch; do not install or publish from fixture
results alone. Current installed runtime remains ca505c1.

## Authorized footer installation review

Jordan explicitly requested installation after the completed pinned-footer build.
Resolve is closed. Displayed group validation exposed a null QWidget event-filter
comparison when the native footer is absent; the guard now requires a real widget.
The group suite passes with that fix. Cross-process restart passes both placements;
its assertion now captures the surface identity before teardown restores native panes.
The group fixture explicitly shows its dynamically added native page button.
Rebuild and backed-up installation follow; hotkey mappings remain untouched.
Live footer acceptance, main merge and release remain pending.

## Pinned header source candidate (not installed)

Installed baseline remains 091268b. feature/macos-pinned-workspace-header isolates
permanent native/custom title-bar Panels access and a dedicated native-toolbar
controller. Header visibility routes share one state; the toolbar is reserved above
custom content, restores to its native sibling-of-page slot on departure, and old
floating-toolbar groups migrate in memory. Required native-tree toolbar restoration
remains compatible. Hidden ownership, migration, page-return, visibility/recovery and
accessory regression fixtures pass; panel-session and native-workspace suites compile.
Resolve is running, so displayed combined suites remain deferred. Automatic full
Native Emulator transition is not claimed complete; current header follows existing
primary surfaces without loading a starter preset. Native styling and compatibility
follow on isolated branches. No installation, main merge or publication.

## Compatibility research branch

research/macos-platform-compatibility builds the helper, bootstrap and Swift launcher
for macOS 11.0 with arm64 and x86_64 slices. Binary/architecture/plist/signature audit
and current-host self-tests pass. Older-OS, Intel-hardware and Windows feature parity
are not verified. Installed 091268b is unchanged; public packaging remains unchanged.
See opengate-platform-compatibility-20261020.md for selective upstream module analysis.

## Continuous tab viewport experiment

experiment/macos-timeline-tab-viewport replaces arrow stepping with a public Qt scroll
viewport only in recognized slots. Scrollbar owns a separate padded row; hidden one-pixel,
trackpad, dynamic-tab, selection, plus-button and restore checks pass. Resolve custom-tab
parent assumptions still need live acceptance. Branch is not installed or merged.

## Timeline scale range experiment

Native font/control scale accepts 70–300 percent in experiment/macos-timeline-scale-range;
fixture verifies 150/200/300 metrics, signal identity, no baseline drift and restoration.
Custom-painted track body and ruler scaling remain unimplemented and explicitly labelled.
Installed runtime unchanged. No claim of full timeline scaling or older OS live support.

## Combined header/style build review

The integration/macos-header-style-live-candidate combines the isolated header, native
footer style, compatibility, continuous scrolling and 300% metric branches. All six
combined hidden suites passed, including native preset hotkeys. The installed 091268b
remains unchanged. Full Auto Emulator conversion and track/ruler scale remain open;
this is a candidate for testing the implemented pieces, not task-list completion.
Build and compile remaining regression fixtures; displayed/live tests await Resolve
closed. No packaging, main merge or installation.

## Flat Panels menu review

`fix/macos-flat-panels-menu` lists every registered panel directly in each title-bar
Panels menu and removes All Panels/This Window duplication. Visibility checks and
Show/Hide delegate to canonical owners. Hidden workspace-header regression passed;
panel-session compilation passed. No install, main merge or publication. Fusion work
remains after the existing arrangement task list, isolated on separate branches.

## Complete native footer follow-up

`fix/macos-complete-native-footer` moves the original complete native footer into the
primary custom surface via reversible bottom-band ownership. Home/Settings are original
controls there, with native-painted callable proxies on secondary surfaces. Hidden
navigation regression passes; restart fixture compiles with updated ownership checks.
Displayed restart/live testing remains pending. This supersedes the earlier primary
page-only copy design. No install, main merge or publication. Fusion remains later.

## Interaction audit follow-up

`fix/macos-interaction-audit` records hidden and dynamically-created native button
activations, page transitions, per-session identity and monotonic event sequence.
Panel caller request/rejection/dispatch, header visibility, footer visibility/placement
and native accessory ownership requests now have explicit records. Crash markers retain
the session id and bounded native-ownership/page breadcrumbs; no fatal signal handlers
are replaced and signal observations do not assert native success or root cause.
Hidden interaction and crash-correlation tests pass; durable flush/rotation tests pass;
existing reliability suite compiles. No install, main merge or publication.

## Timeline cramped/stacked content follow-up

`fix/macos-timeline-overflow` uses the established PanelOverflow wrapper for Timeline
instead of relaxing its native width with no scroll path. Original native constraints
remain intact; dynamic stacked content can extend the range. Show attached Timeline
Toolbar now reveals its hidden ancestor chain through the existing recovery helper.
Hidden overflow fixture passes horizontal/vertical reachability, dynamic width growth,
unchanged native constraints, toolbar reveal and ownership release. Panel-group suite
compiles. Actual stacked timeline native paint and input still need live acceptance;
geometry overflow alone did not establish a GPU clipping defect. No install/main merge.

## Updated combined candidate verification

Flat Panels, complete native footer, interaction audit and Timeline overflow are now
assembled on integration/macos-header-style-live-candidate, with each feature's source
commit retained separately in the feature lock. The reviewed hidden matrix now includes
pinned header, complete surface navigation, continuous tab viewport, native preset
bindings, hidden interactions and Timeline overflow. Full helper rebuilding and hidden
matrix verification follow this checkpoint. Resolve remains running; no install or
live project interaction. Auto Emulator conversion, full native-painted track/ruler
scale and later Fusion support remain open; cross-platform stability is unverified.

## Command provider fixture isolation

The combined hidden matrix passed all suites except three legacy command-catalog
assertions. The fixture expected private bindings while normal-launch native preset
routing deliberately suppresses that store. `test/macos-command-provider-isolation`
explicitly selects the existing recovery opt-out only inside that legacy test block.
The corrected hidden command fixture passes. Production shortcut routing is unchanged;
ordinary native remapping/alternate/clear tests passed separately in the matrix.
## Shared page workspace state foundation

`fix/macos-page-workspace-state` adds a shared Qt-only PageWorkspaceState module and
routes Edit's existing last-view saves through it. Each page retains independent
last mode, selected arrangement and modified view; explicit Native return preserves
the previous managed view. Saved arrangement captures exclude the per-page ledger.
Native-return intermediate saves cannot replace the last complete page checkpoint.
Legacy Edit fields remain readable, and unknown page-state versions fail closed.
Core persistence fixture passes migration, independent Edit/Fusion mode/selection,
cross-page rejection, restart, interrupted-transition recovery and no recursive capture.
Panel-session/restart integration compilation passes. Fusion panel/coordinator wiring
is not enabled or claimed complete. No install, main merge or publication.
## Page-aware workspace controls foundation

`fix/macos-page-aware-arrangements` removes the controls' hardcoded Edit list/order,
retains selection independently per page in memory, and rejects cross-page restore,
update and dialog completion. Existing Edit callers retain their default behavior.
A hidden Qt fixture passed for same-named Edit/Fusion presets, filtered lists,
independent selection and mismatched capture rejection. This is a controls foundation:
page-switch coordinator wiring, per-page persisted selection and Fusion session
ownership/restoration are still pending. Fusion stays disabled until those exist.
Reuse Edit interaction/native styling; no mixed-page windows or alternate Fusion UI.
No new install, main merge, push or publication.

## Automatic workspace mode semantics

`fix/macos-automatic-workspace-mode` verifies and records Native→managed state from
actual saved panel/group ownership. It retains existing native and managed surfaces,
geometry and selected preset identity; it never loads the Native Emulator starter
preset on an undock. Explicit Native return uses the established docking/restoration
route, preserving the last managed page view. Existing NativeWorkspacePresentation
keeps surviving native editing content and retires only an empty shell; the shared
pinned bars follow the existing primary-surface policy. No extra root-layout conversion
is needed for this transition. Earlier notes calling for a separate Auto root conversion
are superseded by this retained-layout strategy. Hidden page-state tests pass; live
transition/bar ownership acceptance remains required. No install or publication.

## Read-only lazy page inventory

`research/macos-page-panel-inventory` captures bounded native page hierarchies after
Show/lazy child construction, including stable within-capture object/parent ids,
widget geometry/native flags and exposed scale/panel method/property names. It does
not call layout hints, property getters or methods, read control text, move widgets
or change native state. Four captures/page/session and 32 retained files bound storage.
Hidden fixture passes anonymous/duplicate ancestry, limits, no text, no detached
cross-page window and zero layout-hint calls. Existing startup snapshot only exposes
Fusion's outer container; actual per-panel ownership still needs a loaded Fusion
page capture before undocking can be implemented responsibly. Likewise native-painted
Timeline body scaling has no verified hook. Do not claim either feature implemented
from invented fixture object names. No install or live Resolve interaction.

## Tab viewport and scale integration

`fix/macos-tab-viewport-scale` marks the generated viewport as an adapter-owned metric
container while continuing to scale its real native tab child. This prevents scaling
an already-derived viewport height twice. Hidden integration fixture passes native
text fit, separate padded scrollbar/ruler geometry, selection-preserving scroll and
original-row restoration at 100/150/200/300 percent. It does not establish native-painted
Timeline track/ruler scaling or actual Resolve tab-layout availability. No install.

## Combined checkpoint before full rebuild

The assembled candidate now includes page workspace state, page-aware preset controls,
automatic retained-layout mode, read-only lazy page inventory and tab/scale integration.
Feature lock records each exact source branch tip and the legacy command fixture fix.
Tests do not close live acceptance. Full Timeline body/ruler scaling and Fusion panel
undocking remain pending actual native capability/ownership evidence. Older OS/Intel
and Windows runtime validation remain pending; no universal stability claim. The
installed 091268b remains unchanged. Run full universal build/audit and updated hidden
matrix from this clean source. Do not install, publish or merge main at this checkpoint.

## Preserve invalid workspace files

`fix/macos-preserve-invalid-workspace` refuses automatic writes after unreadable,
malformed or unknown-version Workspace.json input. Original bytes remain available
for recovery; failure appears in the page-state log. Hidden persistence fixture passes
both malformed and future-version preservation in addition to page isolation/restart.
This matches the existing arrangement-store preservation policy. No install.

## Resolve host compatibility provenance

Installed Resolve Info.plist identifies version 21.1.1 (build 21.1.10010), with macOS
15.0 minimum. `build/macos-resolve-host-provenance` records the build host's app/version,
minimum OS and Info.plist hash plus the current Qt 5.15.2 runtime contract. Compatibility
reports distinguish that application requirement from the helper's target 11.0.
Manifest fixture passes using an isolated fake application; no other Resolve version
or older OS is thereby validated. No install or publication.

## Final persistence/provenance review checkpoint

Clean source 94e8704 passed all 31 hidden suites with source unchanged and full universal
build/self-tests/audit. Subsequent isolated fixes preserve invalid Workspace.json bytes
and record the real Resolve build host; both dedicated fixtures pass. The candidate
notes now distinguish implemented fixes, live acceptance, missing Fusion/Timeline
capability evidence, platform limitations and standalone GitHub status. GitHub read-only
check confirms default main and isFork=true. No installation, main merge or release.
Run the final assembled rebuild and hidden matrix after committing this checkpoint.

## Expansion persistence isolation

User authorized speed fixes while preserving grouped maximize. Separate branch
perf/macos-defer-expansion-persistence extracts existing panel-state capture from
save(), uses it before expansion and coalesces persistence via existing250 ms save
timer on both expansion and restore. Shutdown/page checkpoints still explicitly
save. No host lifetime change or keyboard binding change. Adds synthetic latency
fixture assertion that active standalone/split/tab toggle dispatch does not perform
workspace.save synchronously. Fixture compilation passes; displayed benchmark
execution deferred while Resolve runs. Native reliability compilation passes.
No measured live speed gain claimed and no installation. Outer-band ownership,
logging overhead and other identified costs remain separate unfinished changes.

## Targeted input diagnostics

Separate perf/macos-targeted-input-diagnostics retains startup/dynamic button and
ActionAdded observations but stops rescanning all actions/buttons on window activation.
Full per-key command-context inventories require OPENGATE_VERBOSE_INPUT_CONTEXT=1;
ordinary key metadata and command routing logs remain. Hidden audit passes hidden
button, dynamically created hidden button, dynamically added action and page change
with one event each. No runtime installation or speed claim.

## Band transfer boundary

Live attach records identify standalone Timeline as band owner; MacPanelSession
attachToGroup subsequently donates its entire layout to a group cell. This carries
band wrappers into the leaf while the lease retains the old host as owner. Added
beforeLayoutTransfer boundary releases both native bands before donation; dockAll
also restores footer as well as header before Native return. Hidden fixture passes
standalone bands -> grouping -> outer group bands -> native slot restoration.
Fixture also exposed native-window recreation warnings in QScrollArea::takeWidget;
Qt5.15.2 source confirms it temporarily sets the native content parent to null.
Address this separately before live acceptance. No installation or claim the entire
workspace-band topology is complete.

## Native band release without transient windows

Qt5.15.2 QScrollArea::takeWidget source setsParent(nullptr). Replaced that release
with direct reparenting to retained native parking, removal of the scroll-area
observer and destruction of the owned wrapper before native restoration. Failed
native-slot restoration leaves content parked for an explicit safe retry, rather
than constructing a temporary top-level native window. Hidden accessory fixture
passes including zero parentless native-control transitions and no prior KVO
warnings. Hidden pinned-header and complete surface-navigation suites pass.
No installation. Baseline091268b restored at Jordan's request while development
continues; current settings and keyboard assignments were hash-verified unchanged.

## Automatic background flow

Ordinary layouts now use automatic native-shell visibility instead of inheriting
legacy hide_background/native_workspace_mode switches as manual requirements.
Explicit advanced manual-hidden policy remains available under collapsed Advanced
controls in Panel Manager and persists independently. Saved arrangements are not
rewritten; comparisons normalize legacy background state to automatic. Hidden
background-flow fixture passes default/legacy migration, hidden advanced controls,
explicit override persistence and return to automatic. Header/footer Show/Hide and
saved arrangements remain. This is flow policy, not a claim all viewport placement
or Native-return live acceptance is complete. Added fixture to hidden matrix.

## Native return transaction and result

Native Edit now receives an explicit dockAll result, reports/logs incomplete
restoration and does not continue as if a failed return succeeded. Band release
precedes panel transfers; each panel no longer performs a full workspace save
inside the Native return transaction. One completed native result is persisted
after transfer. Main-window maximized state is preserved; showNormal is used only
for a minimized window. Saved arrangements remain untouched. Native workspace UI
fixture compilation passes; displayed/live acceptance remains required. No install.
Confirmed single-entry flow recorded in header/platform plan.

## First maximize directly from Native UI

The first expansion may call requestUndock before surface readiness, whose tail
still performed an immediate full save. Route that case through the same deferred
save timer as already-floating expansion; ordinary explicit Undock retains its
existing checkpoint. This closes a second persistence path before maximize can
complete. No visible readiness or safety checks removed. No measured latency claim.

## Startup regression and sampled hot path

c9fdaa9 failed live startup acceptance. User force-quit and authorized rollback;
091268b restored with settings/mappings preserved. Backup before rollback is
header-style-live-candidate/build/macos/checkpoints/before-baseline-restore-20261021T040403Z.
A3-second sample of process72579 contains569 main-thread sample observations,
552 under PinnedWorkspaceHeader::eventFilter -> PanelControls::page -> recursive
QObject findChild during native construction. This is measured sample attribution,
not merely a speculative expensive path. Earlier launch exits had no new native
crash report; sample does not prove every startup failure is explained.

perf/macos-cached-page-discovery removes discovery callbacks from header event
filters entirely, retaining the observed page from refresh and scheduling page
Show events by known object-name prefix. PanelControls retains a QPointer Edit page,
resolves newly shown Edit pages directly and limits missing-page retries to100 ms.
Normal lookups on an existing page have no retry delay. Regression fixture sends
1000 construction events and asserts zero header discovery calls. Hidden header
fixture passes; native workspace fixture compiles. Full startup/runtime validation
still required. No new installation authorized or performed by this change.

## Authorized displayed validation before startup replacement

Jordan closed Resolve and authorized displayed tests, then replacement and a fresh
launch only if checks pass. First panel-sessions run stopped at two stale legacy
background preference assertions. Updated those to the confirmed automatic policy,
adding explicit manual-hidden and automatic-policy cases; no runtime behavior
changed to satisfy the old expectations. No installation. Continue all displayed
native-window tests, then rebuild exact final source and verify fresh startup.

Displayed panel-sessions passes after policy assertion update. Group fixture still
expected Workspace Header to have a float control; updated to assert pinned header
has none, Timeline Toolbar can float, native mode selection and Native return are
preserved. This updates the fixture contract only, not production behavior. Run all
remaining suites even when an independent suite fails, but never install until the
combined receipt passes.

Displayed reliability comparison:091268b itself fails the Cinema assertion because
it requires player->parentWidget()==host, despite existing viewer overflow wrappers.
New Timeline wrapper similarly invalidates the direct-parent expectation. Updated
assertions to require same retained host ancestry/window plus visibility, retaining
original-slot Dock assertions and repeated page-return cycles. This does not relax
ownership to an arbitrary visible widget or claim a production Cinema fix.

## User rejected incomplete Native entry flow

Jordan explicitly requires direct drags/controls from Native Edit, one flat Panels
entry with Header/Footer, reserved hideable outer bands, unsaved persistence and
Native Edit return. New acceptance contract lists observable steps and requires an
assembled fixture plus live evidence; no narrower component interpretation may be
substituted. Native-entry-flow removes Edit/Workspace categories from the menu-bar
Panels route and uses the same Show/Hide dispatch as title-bar Panels. Visibility
no longer calls requestUndock from the menu-bar caller. Removes the ordinary footer
placement submenu from title-bar access. Hidden workspace-menu and workspace-header
fixtures pass flat structure, shared state and page-switch cancellation. Remaining
Native header drags, native visibility and assembled proof are not yet implemented
by this menu change alone. Installed6f395c4 unchanged.

## Native header working branch — not installed

Source/Program header buttons now use the existing drag control and shared native
panel-drag coordinator. Click uses visibility without creating a floating host.
Native surfaces without Resolve toggle actions retain native ownership and record
visibility changes; explicit Native return validates all tracked owners before
restoring them. Show under a hidden owner reports unavailable rather than success.
Hidden native-surface-doors regression passes; page-workspace-state regression
passes including native visibility ledger round trip. Native-workspace-ui compiles.
These do not establish assembled flow, physical drag capture, or live acceptance.
Footer still has secondary per-button layout and optional pin model, both pending
correction. Installed6f395c4 remains untouched.

## Native footer reserved bands — not installed

On fix/macos-native-footer-bands, the primary surface leases the complete original
native footer without compressing its width. Secondary surfaces paint the complete
original footer at its logical size, including its native layout/background and
Home/Settings; clicks target the original native buttons. This is a whole-widget
mirror, not a second native widget or a reconstructed icon row. Narrow mirrors
scroll. No paint-event polling or render-triggered refresh loop is installed.
Footer is reserved on every supported workspace surface while globally shown;
ordinary per-surface pin choices are removed. Native visibility remains authority.
Hidden surface-page-navigation fixture passes complete native pixel comparisons at
1x/2x, original Home/Settings signal execution, page-button dispatch, hide reclaim
and show restoration of body bounds, narrow Settings reachability, original-slot
return and teardown ownership/constraints. Native-workspace-ui compiles. These are
Qt fixtures, not proof of live Resolve custom painting or latency. Header topology,
maximize gates and assembled flow are still pending. Installed6f395c4 untouched.

Confirmed with Jordan: visible bands gate docked/grouped/maximized content; hiding
a band removes its gate, and showing it restores that bound. Detached floating
windows may intentionally overlap bands; do not impose floating placement limits.

## Visible-band maximize gates — not installed

Expansion previously used screen availableGeometry without consulting visible
workspace bands. The expansion controller now obtains content bounds from the
header/footer owners. An expanded host with its own reserved band does not lose
that space twice; an external expanded host avoids visible original bands. Hide
removes the corresponding gate; Show reinstates it and refreshes current expansion
geometry. Different screens are not constrained by unrelated bands. Ordinary
floating movement has no gate callback or new restriction. Geometry changes log
panel.expand.bounds. Hidden header/footer owner fixtures and shared geometry/state
fixtures pass; native-workspace-ui compiles. Physical maximize/restore and assembled
flow still require a displayed fixture and live validation.

### Assembled Native Edit flow fixture

New native-entry-flow fixture passes one sequence through actual PanelControls,
menu registry, header controls and native drag callback: native entry with manager
hidden; direct flat Panels; Source/Program native Show/Hide; narrow header overflow;
both bands hidden/shown with content bounds reclaimed/restored; Source header drag
callback and first scoped docking; grouped maximize avoiding visible bands; hide
enlarging current maximize and Show restoring it; maximize restore; public Save As
with named arrangement; Native Edit original-slot return; saved arrangement menu
restore; and unsaved native visibility plus hidden bands across an actual child
process restart. This exposed/fixed saved Footer-hidden being discarded when the
native visibility QAction was discovered. The fixture uses synthetic native widget
names/layouts, hidden test hosts and a hidden naming dialog. It does NOT exercise
physical pointer capture, actual Resolve custom painting, startup injection, native
keyboard dispatch, page-crossing runtime or Relay/Deliver. Those remain live gaps.
Existing workspace-reopen expectations now require unified reserved footer bands
instead of independent per-surface pins, including legacy-pin migration coverage;
its displayed run remains pending.

## Footer discovery cleanup and initialization follow-up

Complete-footer mirrors no longer rebuild a separate per-page model or scan every
page button on refresh. Footer/widget/native-menu pointers are retained safely;
QAction discovery stops once found and missing-action retries are throttled. The
legacy native caller menu is discovered once, then retained when shown. Saved
Footer-hidden intent remains pending until the native visibility command appears,
rather than being discarded by early construction. Complete-native mirror clicks
log both request and dispatch. The assembled hidden flow passes after this cleanup;
a focused delayed-action startup test is included. These reduce avoidable work by
inspection; no measured live latency improvement is claimed.

## Review checkpoint — production source9545bb4

All34 hidden regression suites pass on clean unchanged9545bb4, receipt
`build/macos/verification/9545bb47d6-hidden-20261021T093444Z/receipt.json`.
Universal helper/bootstrap/launcher and30 workflow wrappers built; signature and
architecture audit passes. Minimum helper target11.0 does not lower installed
Resolve21.1.1's macOS15.0 requirement. Displayed group/reopen/reliability compile;
not run on this candidate. A further test-only assembled sequence passes fully
detaching Source/Program/Timeline through native drag callbacks, mounting original
bands on the outer primary surface, regrouping without donating bands into leaves,
and restoring original native panel+band slots via Native Edit.

User clarified that detached windows may overlap deliberately. Maximize of a
workspace smaller than the monitor is still awaiting a choice of workspace bounds
versus monitor expansion; do not claim that UX decision is settled. Current code
retains monitor expansion with visible-band gates. No physical-pointer, native
Resolve rendering, newly built startup or Relay/Deliver validation has occurred.
Secondary footers are complete native-painted mirrors with original-button click
dispatch, not independent native widget instances; hover/pressed feedback and
custom native painting need live review. No installation, main merge, push or
publication occurred. Installed6f395c4 and best091268b checkpoint are unchanged.

## Jordan clarification and Wolfgang external report — 2026-10-21

- Maximize decision is settled: fill the workspace viewport between visible
  header/footer; reclaim either hidden band's space. Current9545bb4 still starts
  from the monitor rectangle, so smaller-workspace bounds require a follow-up
  implementation and fixture before that flow is called complete. No new install.
- Add release-blocking investigation: Wolfgang reports installation succeeds but
  OpenGate UI menu is absent on his Mac mini running Resolve Free, both with and
  without Unbound. Obtain exact OS/CPU/Resolve edition+build/package version and
  bootstrap/helper startup logs. Compare Free/Studio and clean/coinstalled setups;
  do not diagnose this as a Windows executable problem or claim support yet.
- Packaging coexistence audit: current Mac prototype payload installs
  IOPlugins/Unbound.dvcp.bundle; postinstall retires Unbound.py to the backup;
  settings use Application Support/Unbound. These are concrete Mac shared-path
  behaviors. Separate OpenGate runtime identity/settings with safe migration and
  rollback before promising coexistence; never delete current user state.
- Licensing presentation audit: root LICENSE currently names Wolfgang only and
  package-prototype.py copies it directly into the installer license screen.
  Add appropriate OpenGate/CineWaltz copyright for original contributions and
  preserve inherited Wolfgang MIT notices. Branding/art cleanup is separate from
  required legal notices; inspect the exact reported folder before deletion.
- Verify which shortcut branches are actually pushed before telling external
  users they are available. Local feature branches are not publication evidence.

External-report evidence: the locally installed CodecPlugin/README.txt states its
plugins are supported in Resolve Studio; Workflow Integrations/README.txt likewise
describes Studio. OpenGate currently uses both routes. This makes Free-edition
bootstrap availability a leading compatibility question, not a proven diagnosis
of Wolfgang's machine. Installer currently checks major Resolve version but does
not establish edition capability before reporting success. Add capability-aware
preflight/clear unsupported-edition reporting to the packaging audit.

## Workspace viewport maximize follow-up — basic-flow branch

Work order is basic-flow install/live test and possible verified baseline merge,
then isolated Fusion branches, then Wolfgang's product identity/licensing/repository
requests. No install/merge/push/publication performed by this follow-up.

The expansion owner now retains its originating workspace before any temporary
detachment. Native layouts supply their actual page content rectangle; custom
groups supply their laid-out content union excluding visible native band wrappers.
No monitor rectangle substitutes for a smaller workspace. Native frame margins
remain inside the content bounds. Active expansion keeps the native bands on the
retained workspace rather than letting primary-surface selection chase the
temporarily expanded panel. Expanded secondary hosts hide their redundant footer
mirror and restore it afterwards. Standalone primary surfaces keep their outer
frame and let their existing layout reclaim/show band space, preventing repeated
self-shrinking. Ordinary floating placement is not routed through these bounds.

Strengthened hidden assembled fixture passes exact Native Edit viewport bounds,
all four header/footer visibility combinations, workspace resizing during maximize,
standalone primary frame stability, fully rearranged group-local viewport bounds,
grouped restore, saved arrangement/Native return and fresh-process unsaved state.
This remains synthetic Qt evidence. Production build/full regression recheck and
physical/live Resolve checks are separate gates, not implied by this pass.

## Wolfgang follow-up — updated task scope and acceptance

Wolfgang clarified the main report is that OpenGate would not load. A conflicting
path remains his hypothesis, not a diagnosis. He encourages obsolete-code cleanup,
OpenGate's own copyright identity and a fresh standalone repository. He is willing
to retry0.0.2. Jordan's work order remains basic-flow live test/possible baseline
merge, then separate Fusion branches, then the requested identity/repository cleanup.

Post-Fusion cleanup acceptance:
- OpenGate/CineWaltz copyright for original contributions appears in the project
  and installer; inherited portions retain applicable Wolfgang MIT notices.
- Audit obsolete code, copied artwork and installer/runtime entry points by actual
  references and build coverage before removing them. No blind folder deletion or
  global replacement of compatibility identifiers. Preserve working foundations.
- Propose a fresh/standalone repository migration only after backing up all refs,
  branches, tags, local changes and relevant repository metadata. Verify the new
  default branch presents OpenGate. Keep the old repository recoverable until the
  migration is verified; no force-push/history loss or fork detach by assumption.

Release compatibility acceptance (separate from branding priority):
- Trace cold-start bootstrap/helper/menu logs on known Mac/Resolve edition/build.
  Investigate actual loading capability on Free versus Studio before claiming
  Free support; missing log directory alone does not prove helper never loaded.
- Audit shared Unbound runtime/script/settings paths, backups, uninstall behavior
  and both installation orders. OpenGate must not unexpectedly retire/overwrite
  the other product or user settings. A path conflict remains unproven.
- Provide Wolfgang a0.0.2 candidate only after the relevant compatibility checks,
  with accurate tested requirements and useful diagnostics.

## Displayed basic-flow regression findings and corrections

First displayed run at332f79a had three failing suites: panel-sessions synthetic
regional-preview case, panel-groups late native footer caller discovery, and viewer
expansion timing's old screen-width readiness assumption. Native-arrangements,
native-workspace-ui, workspace-reopen, reliability and controls-hidden passed.

- Production correction: explicit FooterPanel::wireMenu now invalidates the
  one-time caller search, preferring the current native root's menu. This discovers
  late-created menus without reinstating full-tree scans on ordinary refresh.
  New hidden late-caller regression and displayed group suite pass.
- Synthetic-drag fixture correction: a native hover with no physical button held
  arrived while the fixture yielded230 ms, finished its synthetic drag and grouped
  the windows before the regional-preview assertion. Diagnostic logs show
  button-no-longer-down and both old hosts hidden. The fixture now excludes
  spontaneous mouse moves only during its explicit held-button synthetic sequence.
  Explicit Qt moves, covering-window occlusion, native-release tests and all
  production drag handlers remain intact. Displayed panel-session suite passes.
- Timing-fixture correction: stateReady now checks exact originating workspace
  viewport bounds, rather than host.width>=original frame.width. The latter is
  false when maximizing a large floating panel into a smaller workspace, which is
  now the agreed behavior. Retain host identity, expansion property, original-frame
  restore, no synchronous saves, exact tree restore and observed proxy-paint checks.
  Timing rerun passes with zero timeouts. First Qt proxy-paint p50 expand/restore:
  standalone7.93/2.86 ms, split27.99/27.50 ms, tabs28.37/25.15 ms. These measure a
  fixture and changed viewport geometry, not physical-key/GPU/Resolve latency;
  they do not establish a live speed improvement.

## Final basic-flow pre-install review — d717d12

- Hidden matrix:34/34 PASS, clean unchanged source; receipt
  `build/macos/verification/d717d12f0f-hidden-20261021T101833Z/receipt.json`.
- Displayed matrix:8/8 PASS, clean unchanged source; receipt
  `build/macos/verification/displayed-d717d12f0f-20261021T102131Z/receipt.json`.
  Suites:panel-sessions,panel-groups,native-arrangements,native-workspace-ui,
  workspace-reopen,reliability,controls-hidden,viewer-expansion-latency.
- Complete native-entry flow, exact viewport maximize, all four band combinations,
  late footer command/caller initialization, original-slot Native return, optional
  named arrangements, unsaved restart and group-band ownership have fixture proof.
- Primary native footer identity is preserved; secondary footer mirrors paint the
  complete native source and dispatch its original controls. Actual native custom
  painting, hover/pressed feedback and physical pointer capture need live review.
- Existing ordinary-launch hotkey/remap/workflow dispatch foundation is included;
  native-preset and workflow regression suites pass. No new live remap, Stabilize
  completion or Fixed Playhead result is claimed on d717d12.
- Software built and audited; installed6f395c4 remains unchanged. No install, push,
  merge or publication. User requested this review before installation.
- Next: review, install for live testing, fresh startup, Native entry/drag/bands/
  maximize/save/Native return, remapped keys and Relay/Deliver. If accepted, consider
  a main baseline; Fusion work starts afterwards on isolated branches. Wolfgang
  identity/repository cleanup follows Fusion. Free support is still unverified.

This final documentation checkpoint does not change the production source tested
and built atd717d12; compare src/infrastructure/tests to establish that explicitly.

## Background/Native UI carryovers explicitly reviewed before installation

Jordan requested an explicit review of the former Native UI/Emulator/background
flow. NativeWorkspacePresentation now automatically hides only an emptied native
shell, restores it on content return/page departure, and keeps native arrangements
as the real main window. Native Edit returns original slots and clears the manual
override. No Emulator starter is loaded as a prerequisite for dragging/rearranging.

Remaining carryovers, not silently claimed removed:
- Panel Manager Advanced retains Hide native shell (manual override), restored
  only from explicit background_policy=manual-hidden. Old hide_background and
  native_workspace_mode fields alone do not enable it. Existing explicit override
  settings can therefore affect startup until Native Edit resets them.
- A saved arrangement named Native Emulator remains as a legacy preset; user saved
  arrangements are not deleted/renamed. It is not a second rendering engine.
- The old0.0.1 package/profile bundle still defaults to that Emulator preset and
  its installer text describes the old flow. This was not clearly stated in the
  pre-install review. Update first-install defaults/instructions before0.0.2
  packaging to present Native Edit plus optional saved arrangements. The current
  live-test software replacement must preserve settings and must not run the old
  seed-profile/package-prototype path. No packaging or installer change performed.
- Resolve's main window/native owners remain alive when its NSWindow is ordered
  out. This is ownership-preserving hiding, not unloading Resolve or proving zero
  background processing/latency. Actual shell/page behavior and Relay/Deliver
  remain live acceptance gates; fixtures are not proof of native GPU behavior.

## Reversible clean-start arrangement view — current install request

Jordan authorized hiding manual background controls and the existing arrangements
without permanent deletion, then installation after checks. Use per-arrangement
hidden metadata, preserving IDs/layouts and all existing user settings. New Save
Arrangement As entries remain visible. Advanced > Show Hidden Arrangements reveals
old entries without changing the current layout. No new Emulator/profile mode is
introduced and no sample preset is seeded for this test. Keep existing runtime
backup and verify preferences/arrangement content before and after installation.

## Lowest priority research — clip colors via Qt meta-object interface

After basic flow, isolated Fusion work and Wolfgang cleanup, investigate the
provided lead: clip colors may travel as a typed Qt signal payload (Sm2ClipColor),
and Fusion composition settings may be exposed by UiFusionCompositionSettingsWidgetImp.
Reuse keyboardprobe.h's metaObject()->method(i).methodSignature() inventory technique
to inspect actual objects/signatures, payload type registration, valid arguments,
ownership and GUI-thread invocation. Enumerability alone is not execution proof.
Use an isolated research branch, logs and reversible fixture/live experiments; no
Resolve binary patching or current-session probing. Do not claim clip-color support
until the real signal/slot route and observable result are verified.

### Clip-color research addendum — lowest priority, not current install work

Provided lead names exported delegate types UiMediaPoolClipThumbItemDelegate,
UiMediaStorageFileThumbItemDelegate, UiConformEffectListItemDelegate and UiDBItemDelegate.
Verify the actual active QAbstractItemView/delegate instances rather than inferring
view coverage from exported names. A proposed wrapper delegates original painting
and sizeHint then overlays a hue strip. Preserve original per-view/per-column
delegates, interaction/editor/tooltip/accessibility behavior, ownership, lifecycle
and native delegate replacement; restore only when OpenGate's wrapper is still
installed. Qt's public delegate API is not evidence BMD supports this integration.

Reversible magenta-row probe may establish whether the delegate reaches the chip;
never assume its duration or success. If not, evaluate transparent viewport overlays
with visualRect, clipping, scrolling and model/reset tracking; QProxyStyle applies
only where rendering actually goes through QStyle. Timeline custom painting versus
QGraphicsScene and its overlay route remain unverified. No such probe is authorized
inside the user's live project as part of basic-flow installation.

Proposed extended-color model: stable OpenGate ID/name/hex plus one native fallback
color. Investigate SetThirdPartyMetadata and SetClipColor on actual supported item
types, including whether TimelineItem has the needed metadata API and how repeated
uses of one source clip behave. Native collaborators/filters/sorting retain only
the fallback family; exact-hue ordering/filtering is not thereby supported. Verify
persistence, project round trips, removal of OpenGate and no clip/edit data loss
before claiming extended-color support.

### Clean-start implementation evidence before final build

Old records use hidden metadata, not a new profile/Emulator mode. ArrangementStore
retains all records/find-by-ID, visible lists omit hidden entries and page reorder
preserves hidden record positions. Advanced reveals hidden records without loading
a layout. Save Arrangement As is directly on OpenGate UI; new records are visible
by default. Hidden manual-background checkbox and expander remain in code for
compatibility but have no normal presentation. Current runtime automatic policy
and native shortcuts are unchanged.

Assembled native-entry fixture passes clean startup with two hidden legacy records,
no old preset choices, direct Save As, hidden manual control, new preset creation,
Advanced reveal (four total selector entries including Unsaved) without native
layout change, the existing drag/bands/maximize/Native return sequence, and restart.
Python clean-preparation regression proves unchanged profile/workspace/shortcuts,
record/layout preservation, byte-preserving backup and rejection of a modified
workspace. Hidden workspace-menu suite passes. User now explicitly authorized
installation after final checks; no further review stop is required.

## Current OpenGate / Native flow correction — 2026-10-21

### Live repair checklist after c033f3d (not accepted)

Reference: Jordan's `closest to good.png` / 2026-10-20 07:26:34 screenshot.
Reuse the existing Workspace Toolbar and native Page Navigation. Do not redesign.

- [ ] Permanent main-window top strip: window controls, project title and Panels.
      Panels remains there when Workspace Toolbar is hidden; it does not migrate
      to Inspector or another floating panel. Avoid project titles on panel hosts.
- [ ] Reclaim the existing Workspace Toolbar from the parked native page tree.
      Pin it directly beneath the permanent top strip. Show/hide independently;
      hidden toolbar releases only its own space. No background toggle or setup.
- [ ] Reserve the actual native title-bar safe area. No panel, tab, scale control
      or toolbar may overlap the window controls/project title/Panels.
- [ ] Keep native Page Navigation pinned below content, complete with Home and
      Settings. Hide releases its band. Both/either/neither band works with maximize.
- [ ] Keep Timeline and its toolbar together for now. Remove separate toolbar
      detach control/menu entry; guard independent detach commands. Narrow Timeline
      uses horizontal overflow without clipping native controls or blank painting.
- [ ] Give Mixer the existing float/dock glyph and title drag in its existing
      header. Being nested inside Timeline must not suppress its own controls.
- [x] Close/recall report withdrawn: Jordan confirmed the hotkey was mapped
      incorrectly. No production behavior change is authorized for that report.
- [ ] One readable title per panel/tab. No duplicate Timeline row, raw widget
      identifiers or unexpected project-name substitutions after transfers.
- [x] Shift-W mapping report resolved by Jordan; do not change mappings or
      describe it as a confirmed OpenGate regression.
- [ ] Compare latency against preserved working builds. Trace repeated layout,
      menu/header refreshes, widget scans, persistence and diagnostic work; measure
      drag, maximize/restore and page changes. Logging stays diagnostic and useful.
      Do not claim the main cause or improvement without evidence.
- [ ] Remove whole-Timeline scrolling wrapper. Keep Resolve's native track/ruler
      viewport and its own scrolling; only the attached toolbar may overflow.
      Verify track headers remain painted while resizing and scrolling. This
      rendering failure predates the new flow and is not fixed by rollback alone.
- [ ] Divider drags must resize only. Limit panel-title gestures to the actual
      text/control hit area, reject stale gestures, and diagnose empty retained
      hosts. No claim of resolution until the reported live sequence is tested.
- [ ] Dock targets: retain broad edge targets; use one bounded center tab target
      and neutral interior around it. Passing over a panel does not dock. Release
      on a highlighted target commits; neutral release leaves the window floating.
- [ ] Verify assembled real ownership topology (toolbar inside parked page),
      recovery, persistence, native clicks, rendering and overflow. Fixture passes
      are distinct from live Resolve acceptance. Review differences before install.

Installed c033f3d is a failed live candidate. Uncommitted repair work has not been
installed. Keep the prior builds, user arrangements and rollback checkpoints.
Fusion/platform expansion and release work follow the basic-flow repair.

Repair checkpoint 7687b5e: 37/39 hidden suites passed. Native preset fixture
initially looked focus-dependent. Retained logs from a later failure showed the
stronger issue: immediate assertions preceded queued command callbacks, and the
fixture changed presets before delivering them. Explicit MetaCall delivery fixes
the focused fixture with exact counts; production key dispatch is unchanged.
Native-entry maximize failed for an entirely vacated page; repaired the empty
content fallback and its focused suite passes. Subsequent repair work adds
toolbar-only Timeline overflow, bounded title hit areas/neutral dock space and
last-panel toolbar release ordering. These need a final matrix and live tests.
Installed helper remains c033f3d; no repair has been injected into running Resolve.

45244e8: 38/39 hidden suites passed; the queued-callback fixture correction above
was verified afterward. Final nested-Mixer follow-up removes the window-name
heuristic too: only its own group cell can suppress a panel's native control.
The focused workspace/menu suite passes that additional case. A final source
build and displayed tests are required; Timeline painting, accidental undocking,
empty hosts and latency remain live acceptance gates, not closed by fixtures.

The installed 85fc822 build failed live vacancy reclamation and header overlap.
It is not accepted. The agreed flow now defaults to the existing emulator-shaped
OpenGate workspace, with immutable Resolve Layout, independent Native recovery,
one primary band owner, persistent working state and Native gesture transitions.
See the updated opengate-native-entry-flow-acceptance.md and implementation/evidence
in opengate-workspace-flow-v2.md. Work is isolated on feature/macos-opengate-default-flow.
No new installation or publication has happened. Source checkpoint61679a9 builds;
its expanded hidden matrix had38 passes and a completion-log timing failure that
passed in isolation. A bounded event-consumption wait is being verified in that
fixture; working hotkey production code was not changed for that failure. Final
matrix/displayed checks and live rendering remain open. Do not mark flow complete.
