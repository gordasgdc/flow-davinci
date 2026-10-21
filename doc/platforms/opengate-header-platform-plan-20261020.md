# Header, styling and compatibility work

Baseline: installed 091268b. Work starts on feature/macos-pinned-workspace-header;
keep runtime changes isolated from upstream adoption. No packaging before live acceptance.

1. Permanent Panels recovery on native and custom surfaces, independent of header visibility.
2. Dedicated reserved top-band controller: one visibility model, native toolbar retained,
   reversible mounting and migration from legacy floating/grouped toolbar state.
3. Auto transition on intentional panel rearrangement; preserve current positions.
   Native return explicit. Page hide/show must not count as user rearrangement.
4. Footer native appearance, using native sources rather than replacement artwork.
5. Separate upstream compatibility evaluation: deployment targets, universal slices,
   loader and platform boundaries; no wholesale merge or replacement of working commands.
6. Combined build, live test, then packaging. Responsiveness remains on hold.

Module boundaries: WorkspaceHeaders owns title-bar Panels access; header band controller
owns toolbar placement/visibility; panel sessions retain native ownership restoration;
workspace coordinator chooses transitions; native platform adapter owns OS windows.
Shared cross-platform policy must not acquire Cocoa dependencies.

Recovery implementation: every registered panel appears directly in each surface's
Panels menu, without an All Panels submenu. Native main receives title-bar access
without replacing its title. Existing primary-only floating-window list remains
primary-only. Installed runtime is not changed by source work.

## Additional timeline scaling requirement

Jordan requested that Timeline interface scale include its body, track headers, ruler,
labels and controls, so smaller fits more information and larger improves readability.
Existing toolbar/tab scaling is insufficient. Investigate native track/ruler metrics
and supported scale paths before implementation; preserve coordinate mapping, clip
hit targets, edit operations and scrolling. Timeline time zoom and track-height choices
are distinct user controls and must not be silently reset. This task follows header,
native styling and platform compatibility; performance/responsiveness remains on hold.

Timeline scale range: retain smaller sizes and extend the requested upper range to
300%, beyond the present 150% UI cap. Use native fonts/control metrics and device-pixel
aware rendering; do not enlarge a raster snapshot. Validate Retina sharpness, ruler
text, controls, hit targets, scrolling and overflow at 100/150/200/300 percent.
Research the native-painted body separately where Qt font metrics do not apply.

## Timeline-tab scrolling follow-up

Live 150% screenshot shows the added scrollbar intruding into tab contents. Reserve
clear space below tabs; verify at each supported scale. Jordan also reports discrete
stepping: current code deliberately invokes native left/right buttons. Investigate an
actual continuous native scroll position before promising smooth pixel movement.
Do not use private Qt memory offsets or present arrow stepping as smooth scrolling.
Keep this follow-up on the timeline scrolling branch, independent of header changes.

Header verification: hidden native-control identity/signal routing, reserved geometry,
hide/reclaim, menu recovery, primary transfer, original-slot restoration and sibling
native-toolbar placement pass. Header access and bottom-accessory regressions pass.
Full session/native workspace compile passes; displayed checks deferred while Resolve
runs. Automatic Native/managed mode now retains the current native/managed layout;
it does not load a starter preset or require a second root-layout conversion. Live
header/footer ownership acceptance is still pending.

## Fusion follow-up (after arrangement work)

Jordan requested a page-aware sweep of Fusion panels and viewers after arrangement
reliability is finished. Inventory actual widgets and ownership; classify supported
undock/dock, group, Show/Hide, persistence, page-return and shortcut behavior. Use the
same Panels model and native styling as Edit, with page-specific availability.
Cross-page Fusion viewers alongside Edit are a later experiment only: prove current
rendering, context ownership, focus, shortcut routing and safe page transitions first.
Do not advertise mixed-page viewers or implement them as part of the initial sweep.

Fusion scope clarification: reuse the established Edit structure and native styling;
no new interaction design or mixed-page experiment. FIRST make workspace storage,
selected arrangement, save/restore validation and menus page-aware so Fusion cannot
overwrite or activate Edit state. THEN apply the same panel behavior to Fusion.
Cross-page windows are explicitly out of scope for this pass.

## Fusion implementation and review requirements

Priority: finish the existing arrangement/header/footer task list before Fusion work.
Fusion is an extension of the established Edit behavior, not a new interaction model.

- Route backtick (`), often called tilde, through the same maximize/restore command
  and target resolution used by Edit. Do not introduce a separate Fusion toggle.
- Reuse the existing window minimize/maximize/restore, docking/undocking, grouping,
  Show/Hide and page-return functions. Fusion Panels / Show Panel entries must expose
  the same operations and state semantics, filtered to the current page.
- Keep native content ownership, constraints, focus, key routing and saved state under
  the existing session/coordinator contracts. Add page-specific discovery/adapters
  where needed; do not duplicate working Edit machinery or invent alternative UI.
- Organize Fusion modules to mirror Edit's discovery, session, decoration, command and
  workspace boundaries. Extract shared behavior when necessary; preserve Edit behavior.
- Separate branches by functional responsibility, not one broad Fusion branch:
  page-aware workspace persistence/menu/selection; Fusion panel discovery and reversible
  docking; Fusion panel/window command integration; and Fusion saved-layout/page-return
  integration. Adjust boundaries to actual dependencies, documenting the base commits.
- Each branch needs a concrete scope, relevant verification and an explicit dependency
  list so it can be evaluated independently. Assemble dependent changes on a test
  integration branch for live evaluation; keep feature commits separately removable.
- Verify shared backtick targeting, window commands, Panels visibility, docking and
  restoration on both Edit and Fusion. No cross-page content mixing in this pass.

Future only: panels may eventually be called across pages. That requires explicit
source-page lifecycle, rendering and ownership support; do not weaken current page
isolation to simulate it.

## Shared header/footer across pages

The pinned header and footer are shared workspace infrastructure, not Edit-only or
separately invented Fusion controls. Reuse Resolve's native page semantics.

- The reserved top band presents the active page's native workspace controls. On
  Fusion, discover and use Fusion's controls/panels through the same header controller;
  do not retain Edit controls or introduce a separate Fusion header design.
- Header Show/Hide remains discoverable through Panels and existing synchronized
  command/menu entry points. Preserve the established visibility/pinning policy across
  page changes; page departure alone is not a user hide or rearrangement operation.
- The complete native footer and its page switcher work across supported pages,
  including navigation into and out of Fusion. Retain Home, Project Settings and native
  actions/signals. Do not replace native navigation with a custom page model.
- Footer Show/Hide and per-surface placement use the existing shared owner and stay
  synchronized with Resolve's native visibility command and other entry points.
- Panels remains reachable when either band is hidden. Page switches must safely
  release/rebind native controls, preserve reserved space and avoid duplicate bars,
  stale Edit actions, clipped controls or layout contamination between pages.
- Verify Edit → Fusion → Edit, hidden/shown bands, saved arrangement restoration and
  repeated switching. Keep Fusion-specific discovery/adapters on its functional
  branches; shared controller changes remain separately reviewable dependencies.

## Diagnostics and release acceptance

Jordan requests logging throughout the existing fixes and Fusion extension. Log
page identity/transitions, saved arrangement id and operation, native panel/source
identity, transfer/restore outcome, header/footer placement/visibility, command route,
and failure reason. Include build/session identity and correlate lifecycle operations
so retained crash reports and the preceding event trail can be compared. Use bounded,
coalesced diagnostics rather than per-paint/per-frame spam; do not capture project
content or keystroke text. Tests must assert meaningful success and failure events.

Target a live-testable candidate this evening, not an unverified cross-platform
stability claim. 0.0.2 packaging follows live acceptance. Older macOS/Intel hardware
and Windows require their own build/runtime evidence; universal binaries alone do
not establish support. Keep unfinished Fusion functionality out of release claims.

## Confirmed page behavior: Native UI and Native Emulator

Jordan confirmed behavioral consistency with Edit as the requirement:

- Fusion has its own Native Fusion UI and Native Emulator, using the existing
  arrangement/session architecture rather than a new workflow.
- Fusion panels can be rearranged and their arrangements saved/restored through
  the same functions and interaction patterns as Edit.
- Each page independently retains its last active Native/custom mode, selected
  arrangement and last modified layout in memory and through session persistence.
- Example: leave a custom Fusion arrangement for Native Edit UI. Returning to Fusion
  restores that Fusion arrangement; returning to Edit restores Edit's remembered
  Native UI. Changing either page must not replace the other's mode or layout.
- Restart restores each page's last state through the established last-view
  persistence mechanism, including unsaved modifications independently of a preset.
- Apply the shared header/footer, Panels, window/command and source ownership rules
  above. No mixed-page panels in this pass. Functional Fusion branches remain
  separately reviewable and follow the existing arrangement fixes in priority.

Working cadence: report verified progress and unresolved issues, then continue the
next authorized task. A status update is not a request to pause implementation.

## Confirmed single entry flow — replaces emulator mode

Native Edit is the normal entry point, with native appearance/controls and OpenGate
Panels access. Rearrange directly from that surface; Native Emulator is not a
required separate mode. Existing saved arrangements remain available. Unsaved
changes persist as the page's current layout. Choosing Native Edit explicitly
restores Resolve's original Edit layout, while choosing an arrangement restores
that arrangement. Each page retains its independent current state. Active-page
native header controls and complete native footer occupy reserved outer bands
when shown; both remain hideable. The optional Panel Manager is an advanced tool,
not a required step. Do not delete or rewrite user-named legacy emulator presets.
