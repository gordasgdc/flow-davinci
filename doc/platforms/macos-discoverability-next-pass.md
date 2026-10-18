# Discoverability and panel behavior discussion — October 18, 2026

Planning notes requested by Jordan. No implementation is implied or acceptance
claimed. Preserve the current grouping, tabbing, saved arrangements, independent
floating windows and fourth traffic-light pin control. Do not merge upstream
wholesale or change established features to simplify this pass.

## Requests captured

- Fix Timeline/Inspector cross-boundary clipping and narrow-panel behavior.
  Inspector may need internal scrolling when native controls cannot fit.
- Make undocking visible through a dedicated native-header symbol beside the
  native options button; retain existing menu routes. Investigate safe native
  header drag-to-detach, excluding buttons, text editing and other native input.
- Reduce dependence on a visible empty native backdrop and its checkbox, while
  retaining a permanent Native UI recovery action, page navigation, native
  dialogs, Cinema Viewer and correct focus/stacking with workflow plugins.
- Hidden panels need persistent discoverable callers: a panel launcher/button
  indicating where the hidden panel belongs, menus and assignable commands.
  Hiding must not destroy remembered layout, tab membership or split proportions.
- Add a keyboard button in the UI to open shortcut configuration. Investigate a
  native Keyboard Customization category named Unbound; do not assume adding a
  QAction makes a command appear in Resolve's shortcut database.
- Give temporary maximize/restore a named assignable command (backtick default),
  preserving placement, proportions, selected tab, visibility and stacking.
- Give every supported window a Show/Hide or Reveal command independent of its
  Dock/Undock command. Preserve native command mappings and single-viewer behavior.
- Explore per-panel minus/plus UI-scale controls, independent saved scale, reset
  to 100%, and exact native-state restoration. Window resizing or image zoom is
  not UI scaling. GPU viewer, coordinate, menu, sharpness and playback checks
  are required before offering this as working functionality.
- Extend the actual native Clip Color palette with bright Red and potentially
  named custom hues: picker, rename, edit and remove. These are additional stored
  clip colors, not aliases for existing colors or painted menu swatches. Give
  each a stable assignable command under Unbound / Edit Timeline / Clip Color
  if native registration is feasible. Preserve existing clip appearance when
  removing a custom palette entry unless explicitly replaced or cleared.
- Preserve Workspace UI and add Unbound Panel Manager there; rename the controls
  window accordingly. Reclaim vacated native space while keeping remaining native
  panels usable, rather than relying only on hiding the entire backdrop.
- Preserve snappiness; measure pointer-to-layout work with diagnostics enabled,
  repeated realistic arrangements and native input hit tests. Do not relax the
  existing performance gate or claim fixture timing proves playback latency.

## Upstream source examined

Fetched Wolfgang's v0.2.0 tag and origin/master (3f4ad6f), without merging.
The v0.2.0 README describes Windows builds, not a verified macOS solution.

- `src/core/panedecoration.h`: vector undock glyph inserted immediately before
  recognized native options controls; context callback opens page presets.
  A click toggles docking; button drag is eligible only while already floating.
- `src/core/paneheaderdrag.h`: observes recognized header subtrees; plain safe
  surfaces can move an existing PaneFloatingWindow. Controls/selectable labels
  are excluded. This does not demonstrate native docked drag-to-detach.
- `src/core/panesnap.h`: independent magnetic correction eased over 32 ms using
  t*t, while raw pointer displacement remains immediate. Aligns window edges
  across gaps, favors nearer windows on equal edge distances, keeps work-area
  snaps local and commits exact geometry on release.
- `src/core/pagepresetmenu.h`: page-specific preset popout launched from button
  context menus; stable rows, recall/update/delete/rename and Dock All.
- `src/core/panefloatsession.h`: hides vacated native parent/divider when suitable.
  README says Timeline expands into vacated viewer space. This is a useful
  contrast to our placeholders, not proof the entire native backdrop disappears.

## Proposed sequence and modular boundaries

1. Boundary and layout correctness: reproduce clipping, record actual native
   surface geometry, separate native minimum sizes from host minima. Verify
   render containment and click routing before wrapping any native content in
   scrolling containers. Retain exact original ownership/constraints on return.
2. Central panel identity/state and command registry: stable panel IDs; separate
   dock placement, visible/hidden state, active tab, minimized state and reveal
   action. Native symbols, menus, launcher buttons and shortcuts call the same
   commands. Show returns to the saved location, not a new floating host.
3. Header decoration and hidden-panel launcher modules: reuse the upstream
   native-anchor principle, preserve pin chrome; visible drag affordances and
   compact reveal buttons without blocking native controls or layout dividers.
4. Shortcut integration probe: identify actual native command registration and
   persistence. Native category if supported; otherwise clearly labeled Unbound
   shortcut editor opened by the keyboard button. Detect collisions and avoid
   intercepting typing. Do not fake native registration with injected rows.
5. Workspace presentation module: explicit Native UI versus custom presentation,
   automatically coordinated background behavior, permanent recovery, page-aware
   native controls and no hidden modal/Cinema surfaces.
6. Independent scale and clip-color feasibility probes, separately gated from
   layout fixes and disabled until their real native behavior is established.

Acceptance matrix: floating/split/tabbed/nested groups; active/inactive tabs;
repeated hide/reveal with unchanged outer geometry; source empty/loaded; native
single-viewer command; maximize/restore and Cinema Viewer; multi-display moves;
pin/unpin; page/project change; preset restart persistence and Native UI recovery.
Every command logs its stable ID, origin, outcome and duration; no entered text.

## Initial local API findings and probe requirements

The installed Blackmagic Developer/Scripting/DaVinciResolveScript.pyi declares
exactly 16 ClipColor names (Orange through Chocolate); Red is absent. FlagColor
and MarkerColor separately contain Red. SetClipColor/GetClipColor are present
for media-pool clips and timeline items. This is evidence of the documented API
surface, not proof that Resolve's internal storage cannot support custom hues.
Do not substitute marker/flag colors for clip colors.

The same API provides keyboard preset list/load/delete/import/export/current
operations. No command-registration API was found in that interface. Next probes
must inspect actual native actions and keyboard customization model identities,
then establish whether command persistence/dispatch accepts an Unbound ID.

Diagnostic probes should be explicitly invoked, scoped to the relevant menus or
dialog, and record action object/class IDs, command metadata types, standard color
choices, result codes and elapsed time. Avoid all-widget scans per mouse event,
entered shortcut-editor text, clip names, media paths and project contents. An
isolated scratch project is needed before any mutation/readback/reopen test;
never rewrite the user's working project or database to explore a color format.
No probes have yet established arbitrary clip-color persistence or native custom
command registration. No new runtime diagnostic probe is implemented by this note.

## Integration and completion policy

The preserved public snapshot is fbe835c on macos-responsive-group-transfers.
The next integration branch is macos-panel-discovery-and-layout. Existing branches
are cumulative checkpoints, not isolated features. Their existence alone does not
certify features as complete or make them safe to merge individually.

Keep focused commits for boundary/width fixes, panel state/commands, native header
decoration, panel manager/menu access, native space reclamation, shortcut probes,
color probes and performance verification. Each includes its tests and evidence.
Use separate feature branches when genuinely independent; dependent work can stay
as ordered commits on this integration branch. Avoid cherry-picking a UI feature
without the ownership/layout/command dependencies it requires.

Recommend a merge into this fork's default branch (master) only for a coherent
macOS baseline with native restoration, no cross-panel painting/input leakage,
verified hide/reveal and maximize round trips, viewer command behavior, persistence
and diagnostics, plus documented performance results. Windows build/shared-code
compatibility must be checked for any shared changes. Do not silently lower the
performance gate. Color/scale/registration experiments must not block unrelated
verified fixes indefinitely; unresolved experiments stay isolated and explicitly
listed, with a scope decision rather than a false completion claim. The overall
requested pass remains open until each request has implementation evidence or an
explicitly discussed feasibility outcome. No automatic merge to upstream or master.
