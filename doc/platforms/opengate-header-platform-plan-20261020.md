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

Recovery implementation: All Panels is reachable from secondary surfaces too; native main
receives title-bar access without replacing its title. Existing primary-only floating-window
list remains primary-only. Installed runtime is not changed by source work.

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
runs. Full automatic emulator conversion remains separate from header placement.
