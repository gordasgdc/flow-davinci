# Active work

Installed baseline: fbfc2a9. Never replace the running helper or close Resolve.
Preserve arrangements and native keyboard presets. No merges to main from experiments.

- [x] Isolate native menu mutations implicated in repeated crashes; candidate installed.
- [x] Verify live Stabilize menu execution and actual Option+Shift S/W Qt key values.
- [x] Native-preset adapter implemented and fixture-tested (explicit opt-in only).
- [ ] Live preset save/switch timing, selected-clip Stabilize execution and duplicate
      routing acceptance. Never claim unsupported native dispatch from absence alone.
- [x] Fixed Playhead exact native Edit action lookup, prior-style restore and target-result logs.
- [ ] Live Fixed Playhead action discovery/state acceptance.
- [x] Inspector overflow arrow/hint and scrolling fixture.
- [ ] Live Inspector appearance/reachability acceptance.
- [x] GitHub default `main`/OpenGate landing page and release/metadata/history backup.
- [ ] Permanent fork separation; settings UI unavailable in this session.
- [x] macOS placement artwork audit: independently drawn OpenGate glyph; MIT retained.
- [x] Header controls bounded away from project title; labels/icons/overflow fixtures.
- [x] Elided outer window-title full tooltip, with stationary controls.
- [ ] RELEASE BLOCKER: live native/attached/floating header overlap acceptance.
- [ ] Responsiveness last; bounded measurement before optimization.

Evidence: latest fbfc2a9 input log has Qt::Key_S (83), Qt::Key_W (87),
modifiers 167772160 (Alt|Shift); prior native binding observations remain empty.
Fixed Playhead request reaches registry but target matches=0. Bridge acceptance
is not proof of successful target invocation. Inspector horizontal overflow=33–64px.

Fixed Playhead contract: Edit page only; independent of panel focus and clip
selection. It changes timeline scrolling/presentation, not playback transport.
Stabilize remains conditional on the applicable selected clip/native target.

Read-only preset audit found native collisions: Alt+Shift+W also maps to
viewReferenceWipeModeCycle; Alt+Shift+F also maps to
FairlightTimeline.trimCrossfadeSelection. These require page-aware conflict
policy, not a blanket global veto or silent override. Alt+Shift+S has no
same-sequence collision in the persisted active preset. Audit parses all six
presets, retains alternate bindings and distinguishes absent preset from corrupt
input. Runtime provider remains disabled pending switch-timing validation.

Header title acceptance: elide by available pixel width; show full project name
in a tooltip. Hover must not move, reflow, or animate neighboring controls.

## Lowest priority: timeline tab-strip scrolling

Target is the timeline-name tab row above the timeline ruler, not track or
playhead scrolling. Investigate a slim horizontal scrollbar plus trackpad/wheel
support while retaining native left/right overflow arrows, tab close buttons,
per-tab dropdowns and the + control. Preserve selection, ordering and native
editing gestures. Feasibility inspection comes after current functional and
header work; no implementation has been started.

## Candidate checkpoint

Prepared separately: native-preset adapter (explicit opt-in), exact native Edit
Fixed Playhead lookup and exclusive-style restore, responsive header controls,
full window-title tooltip, and Inspector horizontal-scroll hints. Passing fixtures
are not live acceptance. Current installation remains fbfc2a9.

GitHub default branch is now main, based on the accepted prototype plus landing
page documentation only. Release assets and repository metadata are backed up;
full-history git bundle verified. Fork detachment is not performed: this session
has no working settings UI, and metadata loss requires review. One historical
issue/PR is in the backup. Original MIT notices and Windows source are preserved.

Header acceptance matrix clarified by Jordan's screenshots:
- Native Edit UI: overlapping native project title and added toolbar callers is
  the reported defect; validate the bounded controls here first.
- Native Emulator: current title strip/toolbar separation works; preserve it.
  Only tighten the far-right Panels pill to fit its title strip.
- Other saved arrangements and floating surfaces: preserve mounting and selection,
  verify no duplicate titles/traffic lights and no overlap at narrow widths.
