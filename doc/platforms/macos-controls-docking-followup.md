# Controls and docking follow-up

Branch: `macos-docking-compass-and-controls`.
Rollback: `macos-docking-tabs-and-maximize` at 38d0616; installed build 21466f2
and archived binary/symbols remain unchanged.

## Evidence before another Resolve restart

Read-only captures of installed Source and Media Pool windows show the diamond
on Source and no visible pin on Media Pool. The controls screenshot shows clipped
left-side actions and horizontal scrolling. Captures remain local, outside Git.
Installed diagnostic records show repeated drag completion without a target;
one Inspector edge target was active for about 38 ms. These records do not prove
that no targets ever appeared. Native title-bar movement did not enter custom
pointer-drag detection. Pointer hit areas differ from moving-window contact edges.

Qt 5.15.2 source confirms disabled focused controls call focusNextChild, and
QScrollArea::focusNextPrevChild calls ensureWidgetVisible. An isolated hidden
fixture reproduces a 730-pixel horizontal jump with the original QScrollArea and
0 with the guarded row update. Foreground process remains unchanged, all native
fixture windows remain offscreen. The test uses no project or Resolve widgets.

## Acceptance checklist

| Requirement | Candidate | Evidence | Live Resolve |
| --- | --- | --- | --- |
| Click Undock without jumping the controls view | Preserve both scroll axes during row updates; retain normal Tab traversal | Hidden baseline 730 / candidate 0 PASS | Pending |
| Pin consistent on titled panels/groups/controls | Native circular button uses green/yellow traffic-light spacing and height | Hidden geometry and toggle checks PASS; rendered image inspected | Pending |
| Pin remains accessible with hidden title bar | Circular Qt fallback in reserved header margin | Hidden visibility check PASS | Pending |
| Targets discoverable without screen-wide zones | One five-way compass after deliberate 180 ms hover; four split buttons and center tab; no auto free-drop docking | Non-overlap geometry PASS; rendered compass inspected | Pending |
| Native title bars enter docking path | Observe native move start/update/release; native movement retained, no Qt mouse grab | Hidden notification/release fixture PASS | Physical drag pending |
| Input and maximize stay usable | Existing capture guard and reversible maximize retained | Full GUI suites compile; rerun deferred while Resolve open | Pending |

Verification commands:
- `bash infrastructure/macos/test-layout-plans.sh`: no-GUI geometry PASS.
- `bash infrastructure/macos/test-controls-hidden.sh`: background-only reproduction,
  native pin, rendering and titlebar cleanup PASS. Uses
  QT_MAC_DISABLE_FOREGROUND_APPLICATION_TRANSFORM and WA_DontShowOnScreen; checks
  foreground PID and native window visibility before exit.
- Full session/group/reliability GUI suites must run later with Resolve closed;
  compile-only invocations do not count as runtime passes.

Rendered candidate controls: `/private/tmp/unbound-pin-candidate.png` and
`/private/tmp/unbound-compass-candidate.png`. These are test-only artifacts.

Live handoff after testing/install: verify small-window Undock keeps the clicked
row stationary; pin size/spacing and functionality on Media Pool, viewers, group
hosts and Unbound controls; hover via native titlebar and member header; create
side-by-side Media Pool/Inspector with Source below, then a tabbed alternative.
Verify previews, release outside targets, Option bypass, buttons, dividers and
backtick restore. Cinema Viewer remains a separate unverified item.

## Built checkpoint

Candidate **7e91be0**, not installed. Helper and bootstrap builds PASS; native
bootstrap handshake fixture PASS. Matching binary/dSYM UUID:
19C47796-0BB9-347B-ACD4-FEB4929E0CE2. Archive:
`build/macos/candidates/7e91be0/`.
All three full GUI suites compile in compile-only mode; no displayed GUI suite
was run during the user's open Resolve session. Hidden background fixture and
no-GUI geometry suite passed. Installed helper remains 21466f2.

## Full GUI verification after Resolve closed

All three displayed GUI suites PASS: session/persistence/input/viewport, nested
groups/tabs/visibility/restore, and reliability/focus/shortcut diagnostics. The
session suite includes scroll-position preservation in the small controls window,
matched native traffic-light pin geometry/toggle and hidden-titlebar fallback,
and deliberate hover revealing the docking compass. Its covering-window fixture
now allows 80 ms for WindowServer to commit the foreground cover before sampling
native order. The initial immediate sample failed; the committed-order run passed.
These fixtures still do not establish physical native-titlebar drag behavior or
live Resolve acceptance, and Cinema Viewer is not yet claimed fixed.

## Installed cc2c233

Installed after confirming Resolve closed. All three GUI suites, hidden background
regression checks and no-GUI layout-plan checks passed; helper/bootstrap build
and native SDK handshake passed. Installed files match built SHA-256 hashes:
- Bootstrap: e6fe27395aa4d67da4bd47feba35759d322130ca1b21db1bf5222f9521459d44
- Qt helper: 2c4925d3fd6ef46801f7f637d3a6a432fa17d6ba1674060faaf48d475c9a4215
Matching symbol UUID: 6716D1A9-A273-342B-80BE-AE7AD5882804.
Archive: `build/macos/checkpoints/cc2c233/`. Prior 21466f2 archive preserved.
Live acceptance remains pending. Test small controls-window scroll stability,
native circular pin on Media Pool/viewers/groups/controls, physical native-titlebar
and member-header hover/drop, tab versus directional split, free release, and
backtick restore/input accessibility. Cinema Viewer remains unverified.
