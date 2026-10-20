# OpenGate task audit — 2026-10-20

This is the current acceptance list reconstructed from Jordan’s requests, source,
installation receipts, branch state and reported live failures. Historical
checklists and build notes are evidence, not proof that this list is complete.

**Task-list review completed; implementation is resuming from this list. Further
installation, publication, merge to main and live-project changes remain on hold.**
Jordan required this review before another install. Do not continue the pending
temporary-project proposal as if it were approval.

## Actual state

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
