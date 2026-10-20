# Priority live-test candidate

Branch: `integration/macos-priority-live-candidate`. Runtime build: **f03d165**.
Installed baseline **541cd25** is an ancestor. No main merge is required to test
the complete assembled application. This branch is not installed or published.

## Included follow-ups

- Normal-launch native-preset hotkeys, alternate bindings, remap/preset switch and
  clearing semantics. Explicit modified Edit commands work with text-field focus;
  ordinary typing stays text.
- Selected-timeline-clip Stabilize route from the installed baseline retained.
- Fixed Playhead settings discovery scopes duplicated controls to Edit timeline
  ownership; two genuine Edit owners still fail closed.
- Independent Commands submenu; native Workflow Integration actions stay put.
- Native keyboard command-name wrapping with taller rows and full tooltip.
- Added toolbar strip no longer contributes its full label width to the native
  row. Spare-slot/title budgeting and labels/icons/overflow remain.
- Detached Source/Program/unloaded Source overflow, plus direction cues on all
  wrapped panels. Native widget constraints remain intact.
- Revised timeline-tab scrolling: nearest native stops, no drag feedback bounce,
  small trackpad-delta accumulation and cache invalidation after geometry changes.

Tab scrolling is enabled for normal launch in this candidate. It delegates to
native arrow steps and is **discrete**, not pixel-smooth.
`OPENGATE_TIMELINE_TAB_SCROLLING=0` is a recovery opt-out. Commits c065712 and
2091e08 are separately excludable for a candidate without the revised experiment;
other command/header/overflow fixes do not depend on them.

## Checks

PASS: full helper and bootstrap build; native-preset remapping; native Edit
command routing; command submenu; tab scrolling; overflow width/scale/redock
matrix; workspace headers; native keyboard navigation/wrapping; generated
workflow scripts; deferred workflow dispatch. These are fixtures, not acceptance
of the actual native Resolve behavior. No live clips were changed.

## Live test after installation

1. Launch Resolve normally. Check Stabilize from menu and a remapped shortcut
   with Inspector hidden and selected video clips; verify old key stops working.
   Also try a field with focus. Empty/audio-only selection must fail harmlessly.
2. Toggle Fixed Playhead from menu and shortcut, in both states, with another
   panel focused and Timeline hidden/shown. Confirm real timeline scrolling,
   not merely a checkmark. Cut must not be affected.
3. Native Edit UI: attach, float and hide Workspace Toolbar. Inspect project
   title/controls for overlap. Repeat with Native Emulator to catch regression.
4. Narrow sidebars, toolbars and viewers; change interface size; use horizontal
   scrolling to the final control, then redock.
5. Drag tab scrollbar both ways and use a trackpad; keep selected timeline,
   rename/add/close tabs and resize. Report stepping separately from bouncing.

The current native maximize row may be unassigned. Assign backtick explicitly
in Resolve Keyboard Customization if wanted: clearing a native row now means
unassigned and does not resurrect an independent legacy key.

## Installation protection

Install only with Resolve closed. Preserve existing arrangements/preferences and
checkpoint the exact current bundle before replacing it. The current install
remains 541cd25 until a separate installation occurs. Existing artifact/checkpoint
receipts are retained. GitHub standalone cleanup and responsiveness are excluded
from this candidate and do not block its live-test installation.
