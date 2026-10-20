# Timeline-name tab scrolling attempt

Branch: `feature/macos-timeline-tab-scrolling`. Not installed or merged into main.
Responsiveness optimization is below this work in priority.

The captured native target is an unnamed `UiTiTimelineTabBar` beneath
`timelineTabsHolder`. It is distinct from the `unboundPanelTabs` group tabs.
The existing snapshots omit unnamed children, so its exact Qt base class and
scroll model remain unverified. No private offsets, guessed click coordinates,
or timeline-selection actions are used.

The opt-in adapter (`OPENGATE_TIMELINE_TAB_SCROLLING=1`) has two supported paths:

- Reuse a uniquely discovered native QAbstractScrollArea horizontal scroll model.
- For a QTabBar-compatible target, delegate to its original left/right QToolButton
  arrows. This is discrete native stepping, not arbitrary pixel-perfect scrolling.

A seven-pixel overlay scrollbar excludes the Qt native arrow area; wheel/trackpad
input over the tab surface is routed to scrolling. Modifier gestures pass through.
The adapter never changes currentIndex, tab order, close buttons, dropdowns, the
plus control, native ownership or layout constraints. Scroll operations are queued
and bounded. It does not schedule updates from every paint event.

If neither supported model exists, attachment is refused and a bounded
`timeline.tabs.capability` record lists class/method signatures and child types.
No timeline names or tab text are recorded. No scrolling slots are invoked based
on guessed signatures. This is a real feasibility gate, not a claim that the
custom Resolve strip already works.

Fixtures pass: scrollbar and wheel preserve selection; native arrows remain usable;
Qt and native-model delegation; unsupported target unchanged; teardown retains tabs
and removes only the added scrollbar. Next step is an opt-in native capability
capture during a planned restart, then live verification of arrows, dropdowns,
close/plus controls, selection and ordering before enabling this feature normally.
