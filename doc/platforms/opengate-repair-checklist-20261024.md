# OpenGate repair checklist

# Product definitions

Use three product terms: **Panel, Container, and Layout**. These definitions reflect
Jordan's October 25 clarifications and agreement. They describe required product
behavior, not completed implementation or live acceptance.

## Panel

A Panel is a native Resolve element: the native grouping of UI and its functions.
Each Resolve page has its own panels. Timeline includes the timeline, its toolbar
above, and its native timeline tabs. Inspector, Source, Media Pool and Mixer are
other examples. Native tabs inside a panel are not OpenGate tabs between panels.

A panel can float independently, have its UI scaled independently, or be docked
or tabbed with other panels. Its native functions remain intact. Mixer starts next
to Timeline, not inside the Timeline panel. Mixer follows the same panel rules:
independent floating, neighboring panels, and tabs within a container. Its native
layout relationship must not limit those operations.

When a panel is maximized, its contents must use the available maximized content
area and remain accessible without clipping. Enlarging only the outer window
while retaining the old content bounds does not satisfy this requirement. Restore
must return to the previous placement and size. This applies to Mixer as well as
every other panel; native painting needs live verification. Panel-specific limits
must be documented rather than silently losing native functions.

## Edit-page panel inventory

These are the panel identities already registered in the current coordinator.
This list defines their product boundaries, not proof that every operation passes.

| Panel | What belongs to it |
| --- | --- |
| Timeline | Timeline editing area, its toolbar and native timeline tabs |
| Source Monitor | Native source viewer and its playback controls |
| Program Monitor | Native timeline viewer and its playback controls |
| Media Pool | Bin navigation, clip contents and native Media Pool controls |
| Inspector | Native inspection/editing controls for the current selection |
| Mixer | Native audio Mixer and its controls; a neighbor of Timeline, independently movable |
| Index | Native Edit Index and its controls |
| Effects | Native effects browser and its controls |
| Metadata | Native metadata display/editing controls |
| Sound Library | Native sound-library search and controls |
| Keyframes | Native keyframe editor and its controls |

Every listed panel follows the same float, neighbor, tab, hide/recall and maximize
rules. Different native widget structures require adapters, not different user
rules. The current code also registers Timeline Toolbar separately internally;
that is not permission to expose it as a separate arranged panel. Header and
Footer follow their band definitions below. Actual native content boundaries,
including Mixer sizing, must be checked against Resolve before changing adapters.

## Container

A Container holds one or more panels, arranged beside each other, above/below each
other, or as tabs. A panel floating independently occupies a one-panel container.
Panels retain their identities and individual UI scales when grouped.

Agreed container rules:

- A floating container has traffic lights and a pin. New floating containers start
  pinned. Pinning belongs to the container; preserve an explicit unpin choice when
  hiding, recalling or restoring a saved layout. Embedded groups do not gain extra
  OS traffic lights.
- The container's traffic-light X hides the whole container without destroying
  its arrangement or overwriting individual panels' hidden/available choices.
- A panel's X hides that panel while remembering its place in its container.
  Its tab remains present and dimmed; clicking it reveals and selects the panel.
- Closing the active tab switches to the next available tab in tab order, like a
  page switcher. It skips deliberately hidden panels. Closing an inactive panel
  does not change the currently selected tab.
- Closing the last available panel hides the container, even if remembered hidden
  tabs remain. Do not reopen a deliberately hidden panel to keep the container up.
  For a one-panel container, panel X and traffic-light X have the same visible
  result: the container disappears and remains recallable.
- Recalling a panel restores its retained container and selects that panel in its
  remembered position. Recall of the whole container restores its previous
  presentation; it does not unhide individually hidden panels.
- Hiding a split panel lets visible siblings fill its space; recall restores the
  remembered split placement. Switching tabs preserves the container's outer size.

Tabbed panels have three distinct states:

| State | Appearance | Clicking the tab |
| --- | --- | --- |
| Active | Highlighted | Keeps the panel selected |
| Available behind another tab | Normal | Selects the panel |
| Deliberately hidden | Dimmed but readable | Reveals and selects the panel |

Show/hide hotkeys use the same visibility rules. A hotkey for a hidden or covered
panel reveals/selects it in place. Invoking it while that panel is presented hides
it and selects the next available tab; when none remain, the container hides.
With two available tabs this behaves like flipping a two-sided card. Hidden and
merely unselected panels must remain distinct internally.

"Window" is an OS implementation detail, not another product object Jordan must
manage. Native window handling remains necessary underneath floating containers.

## Deliberate free movement — proposed clarification

Jordan raised accidental docking while positioning a floating group. Recommended
rule: hold Option on macOS / Alt on Windows to move freely, suppressing both
docking/tab/group targets and magnetic snapping. This is momentary, not a saved
setting. It applies to a single panel or a whole container. Pressing it during a
drag clears the preview; holding it through release leaves the dragged item
floating at the chosen position. Releasing it before mouse-up re-enables normal
target evaluation. Moving a group must preserve its internal arrangement.

The shared floating-host preview and release paths already check this modifier;
this is not a new shortcut implementation. Existing tests cover unsnapped Option
release. All native title, panel-title, grab-zone and whole-container entry paths
still need explicit acceptance, including pressing/releasing the modifier without
moving the pointer. Shift already controls edge alignment; do not repurpose it.
Record this as proposed UX clarification until Jordan agrees. Clear drag guidance
should expose the rule so it is discoverable without remembering a shortcut.

## Layout

Consolidate "Layout" and "Workspace" into one product concept: **Layout** (working
label). A layout records panel/container placement, sizes, grouping, tab order and
selection, visibility, per-panel UI scale, pinning, and master-bar visibility.

The current layout includes unsaved changes. A saved layout is a named copy that
can be recalled; it is not a different kind of workspace. Preserve per-page state
and the protected built-in Resolve Layout. Existing internal identifiers and file
schemas need not be renamed to simplify the interface.

## Header and footer

The Header shows which panels are currently presented and provides controls to
reveal/select or hide them. A header button reveals and selects its panel if hidden
or behind another tab; pressing it while the panel is presented hides it using the
same rules as panel X and the show/hide hotkey.

The Footer navigates Resolve pages. Both bands retain their independent hide/show
behavior; hiding a band releases its occupied space. One primary interface owns
these bands; they do not migrate to whichever floating container gains focus.

The far-right Panels dropdown provides access to every panel on the current page,
including hidden panels, plus a clearly labeled Containers section for recalling
existing groupings without hotkeys. Keep individual panels directly accessible.
Recalling a container restores that grouping; it does not load a different layout.
Saved Layouts remain clearly separate because loading one may rearrange the whole
interface. Panels access remains reachable when the Header is hidden.

## Recall after display changes

Recalling a panel must make it reachable on a connected display. If its floating
container was on a disconnected display or is now unreachable offscreen, bring
that whole container onto the display containing the primary OpenGate interface.
Preserve grouping and size where possible; constrain placement/size enough to keep
its title and controls accessible. Do not detach the requested panel or reset the
layout to achieve visibility.

A container deliberately spanning connected displays stays where the user put it
as long as it remains reachable. Reconnecting an external display does not move
containers automatically. Explicitly recalling a saved layout may restore its
external-display arrangement when that display is available.

# Repair status

Close/hide/recall has an isolated implementation and local checks. Installation
and live acceptance remain pending. Workspace capture/save extraction is implemented locally;
the seeded saved-output comparison and six affected hidden fixtures pass.
Clean universal candidate `093c9de` and all nine displayed suites pass.
[Verification and remaining gaps](opengate-workspace-persistence-20261025.md)
separate those results from installation and live acceptance. Jordan does
not need to review the technical assessment line by line.

Reference: [behavior review](https://chatgpt.com/space/page_23161bac62ac81918906bc4da65af2b5), Jordan’s annotated HTML, and the source assessment of build `ee078c2` on October 24, 2026. The source assessment is complete. First-repair implementation and local evidence are recorded in [retained window close](opengate-retained-window-close-20261025.md).

# First repair

**Required behavior:** X hides an OpenGate floating panel or whole group. Recall brings back the same host, size, position, pinning, and group membership. Explicit docking remains a separate operation. Match Frame must still load the correct source frame as well as reveal Source.

**Checkpoint defect:** In `ee078c2`, `src/core/panelsession.h` handled Close by docking and `src/core/panelgroup.h` handled Close by dissolving the group and docking its members. Tests explicitly expected this outdated behavior. The isolated repair replaces those paths with retained hiding.

- [x] Implement on `fix/retained-window-close` while preserving the installed build.

- [x] Route ordinary managed-window Close through existing hide mechanisms. Temporary expansion, native return and shutdown pass local checks; a separate existing full-layout Dock rejection remains recorded below.

- [x] Replace contradictory tests with checks of retained host identity, geometry, pinning, group membership, and hidden state.

- [x] Verify recall through existing panel callers, including a group with hidden members and nested tabs, in the assembled fixture.

- [x] Check named snapshot restoration, fresh-process unsaved restart, page departure/return, explicit dock to the native slot, and shutdown in fixtures. Live acceptance remains pending.

- [x] Produce a reviewable diff and separate compilation and fixture results; retain the intermittent displayed drag-suite failure as well as passing reruns.

- [ ] Obtain Jordan’s explicit authorization before installing a candidate.

- [ ] Verify the real X button and recall in Resolve, including the correct Source picture for navigation commands.

Do not claim this repair fixes playback disappearance, blinking, or native loading. Those have separate acceptance checks.

# Upstream comparison and direction — October 25

Jordan agreed to continue OpenGate's bounded structural repairs after reviewing
Unbound v0.5.0. GitHub's latest-release endpoint identified v0.5.0, published
October 24. The local upstream checkout is `2c3ccad`; its only difference from
the release tag is README text, so the inspected production source matches v0.5.0.

Upstream's `panecatalogue.h` centralizes native panel identities and policies.
That is a useful structural reference. Its sidebar and viewer sessions still
close by docking back into native slots. Edit viewers have their own lifecycle
and geometry adapters, including native single-viewer visibility handling.
Its catalogue does not register an independent Edit Timeline panel. It does not
provide OpenGate's split/tab container arrangement system.

Replacing the upstream button would be small; adding our agreed container,
hide/recall, band and full-layout behavior would require changes below that button.
There is no evidence that doing that again would be safer than repairing existing
OpenGate boundaries. Continue focused extractions with behavior checks; evaluate
specific upstream mechanisms for reuse individually. This source comparison is
not live acceptance of upstream or proof of the cause of our regressions.

# Live review and reopening priority — October 25

Jordan reported the installed `093c9de` trial positively and requested merge to
main. Local merge `8c66201` preserves main's release README and incorporates the
installed source history; no production changes were introduced by conflict
resolution. No push or new release. This does not close every historical defect.

Jordan's next priority is less latency and eliminating the visible sequence of
individual panels opening before the saved arrangement appears. Both cold launch
and in-session use matter; no specific in-session trigger was identified.

Three captured Resolve processes ran `093c9de` (PIDs 18735, 18762, 18785). Edit
resume to native-plan assembly request took approximately 4.44, 4.01 and 4.72
seconds respectively. These intervals include readiness and other event-loop
work; they are not isolated CPU measurements. Each session recorded 78–88 menu
preparations. Evidence is retained under main-release/build/reopen-evidence.

Code review: restoration acquires one requested panel at a time because native
Media Pool/Index toggles can be mutually exclusive. Each pass also revisits UI
wiring, including a full native menu scan. Individual sessions are presented
before the final native arrangement/group mounts. SurfaceReadiness is already
event-driven: its timeout is a deadline, not a mandatory delay to remove.

Next optimization must distinguish required native initialization from repeated
UI preparation. Measure and reduce repeated wiring first, then evaluate final
presentation without intermediate floating hosts. Do not hide the native root
or skip readiness as a cosmetic shortcut: prior Source-loading evidence depends
on native visibility. Retained in-session recall must preserve host identity and
must not unnecessarily replay startup acquisition. Evaluate real elapsed time,
intermediate visibility and final layout/picture correctness separately. The
accepted installation stays unchanged during this work. No speedup is yet claimed.

## First reopening optimization: native menu preparation

Implemented separately on `perf/reopen-menu-preparation`. `PanelMenus` now
inventories attached native menu owners once per synchronous `wire()` pass,
instead of rescanning all window descendants for every menu. Parent ownership
and conflicting attached owners are still checked. Opening a menu gets a fresh
inventory; there is no persistent cache or second controller. A scoped
`panel.menus.wire` timing span records the actual operation in future live logs.

The 12-panel/1,800-extra-button hidden fixture measures five samples of ten passes.
Baseline median: 4.443 ms/pass. Changed median including timing: 0.563 ms/pass
(about 87% reduction for this operation only). Logs: `build/macos/menu-before.log`
and `menu-after.log` in the optimization worktree. This does not establish a
Resolve reopening speedup or removal of the visible assembly sequence.

Ownership checks pass for ambiguous attachments despite a known parent, conflict
removal, changes between wiring and menu opening, parent-only custom menus, late
menu creation and exactly one command dispatch after repeated preparation.
First-open and delayed-Index startup flows pass. All five complete saved-output
checkpoints match the accepted fixture run with QT_HASH_SEED=0, normalizing only
random preset IDs. The new fixture is registered for Mac and Windows; Windows
execution remains pending. PanelControls is untouched; PanelMenus remains one
responsibility at 185 lines. No panel visibility, readiness or persistence policy
was changed. Candidate compilation and any live trial remain separate gates.

Do not change the presentation sequence merely to claim progress. The next
missing evidence is the optimized operation's real share of restoration time in
Resolve; existing logs do not isolate that cost. Use the narrow timing span in a
reviewed candidate before choosing a broader restoration/presentation change.

## Menu optimization live review — October 26

Installed `ffc7c98`: Jordan reports reopening is quicker, approves merge, and
still sees individual panels loading. Merged to local main as `c543851`;
no push/publication. Installation verified 54 payload files and 24 unchanged
protected settings files. All 54 hidden suites passed before installation;
this candidate's displayed matrix and Windows runtime remain unverified.

Two recorded live sessions show eight menu wiring passes each: total 206.9 ms
and 199.6 ms, median 28.4 ms and 26.4 ms per pass. Edit resume to native-plan
assembly request measured 1.862 and 1.856 seconds. Previous captured runs measured
roughly 4–5 seconds, but layouts and conditions were not controlled; do not turn
that comparison into a precise speedup claim. Jordan's report supports a
perceived improvement. Raw sessions are in `build/reopen-evidence/`.

The visible loading sequence remains unresolved. This optimization changes menu
preparation, not the sequence that acquires individual native panels, presents
their temporary hosts and then assembles the saved arrangement. The next bounded
work should target presentation during that existing restore transaction while
preserving native visibility/readiness, exact saved layout and failure recovery.
Do not simply hide native content: prior Source-loading failures make that an
unsafe assumption. Accepted installed `ffc7c98` remains the rollback checkpoint.

# Remaining repair order

**Next code-organization task:** give workspace capture and saving a focused owner outside `PanelControls`, reusing `WorkspaceSnapshot`, `WorkspaceSettings`, and `PageWorkspaceState`. Replace the coordinator's existing implementation rather than adding a parallel save path. Preserve file schemas, incomplete-restoration guards, native-return checkpoints and temporary-expansion snapshots. Verify production startup, named/unsaved restoration and restart before accepting the extraction. The local extraction is implemented; acceptance evidence is still being reviewed.

**Panel and container implementation:** use the product definitions at the top of this document. Consolidate existing panel identity, native ownership and supported operations; keep placement, visibility and group membership explicit. Any consolidated definition must govern production operations and replace scattered rules; the test-only `PanelPresentation` structure alone is insufficient. The agreed container policies above must be included in future implementation and acceptance checks; the earlier Close repair does not establish all of them.

1. **Startup and Timeline sizing.** Compare saved divider sizes, applied sizes, and final native content bounds. The reported screenshot has a visible Timeline header but an unusably compressed track area. Check first launch separately from restored layouts; preserve checkpoints rather than resetting arrangements.

2. **Visibility and recovery.** Identify the actual cause of floating-window disappearance and blinking. Separate explicit user visibility requests, native-content recovery, tab selection, and window ordering. Remove a conflicting decision only when demonstrated; do not add another keep-showing timer.

3. **Native loading and command reliability.** Preserve successful Source evidence and resolve the experimental launch-flag dependency. Verify double-click A then B, Match Frame, and Find in Media Pool separately, with correct pictures or clip selection and no layout changes.

4. **Dragging and docking targets.** Distinguish docking beside one panel from docking beside a whole stack. Check preview bounds and release results. Investigate deactivation cancellation separately.

5. **Labels and control consistency.** Fix clipped names and make Mixer’s drag affordance understandable with the same Panel rules as its Timeline neighbor.

These tracks may share findings, but their fixes and acceptance results must remain distinguishable.

**Separate docking finding:** in the assembled full layout, detach Inspector and use Dock Panel without first closing it: the session reports "The original panel slot changed." Close is not involved in this reproduction. Direct docking after native return passes. Do not widen the Close repair to change native slot restoration silently.

# Code organization rules

- Reuse the existing layout tree and persistence format. Do not add a competing layout engine or a second independent state model.

- Make each component’s decision responsibility explicit. A new production state representation must replace an existing decision path, not sit beside it.

- Separate behavior repairs from mechanical file splitting. The large coordinator needs narrower responsibilities; line-count reduction alone is not success.

- Preserve native ownership restoration, grouping, tabs, magnetic docking, maximize, hotkeys, Fixed Playhead, Stabilize without Inspector, hideable bars, saved and unsaved arrangements, and the shared Windows foundation.

- Keep implementation, compilation, fixtures, installation, and live acceptance separate. A passing test that asserts the wrong behavior is not acceptance.

- Preserve a reliable native return underneath any future simplification of its user-facing controls.

# What Jordan needs to do

No further specification review is needed to start the first repair. Correct requirements only if they do not match the desired experience.

After local work is complete, Jordan receives a short explanation, a reviewable change, and a narrowly defined live test. Installation requires explicit authorization. No new layout tour or reconstruction of previously reported failures is required.

**Current status:** first repair implemented and locally checked on an isolated branch; installation, live Resolve acceptance, Windows execution and coordinator extraction remain pending. Installed software is unchanged.
