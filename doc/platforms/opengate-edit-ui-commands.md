# Edit command candidate: visibility-independent Fixed Playhead

Fixed Playhead runs only on the active Edit page, regardless of timeline panel visibility, focus or clip selection. `PanelControls` supplies active-page identity separately from native panel visibility.

The candidate prepares Timeline View Options using the native `UiMenuButton::MenuAboutToShow()` Qt signal, which the installed native library emits before reading its menu and entering popup display. It does not send mouse input, show the menu, move focus or require a native shortcut assignment. It identifies the real Edit settings row by caption. Custom-painted captions are read through Qt's public QWidget rendering and QPaintEngine text callbacks into a bounded text-only device, without reading object memory or invoking binary addresses. It then triggers the native row's QAction. Ambiguous, disabled or absent targets fail closed.

The prior `viewTimelineScrollingFixed` fallback is removed: its Qt checkmark changed in logs without proving Edit scrolling changed. No success claim is based on that checkmark.

The isolated fixture passes both handler states with page, menu and custom-painted row hidden; it checks no focus change, rejection of another caption, rejection outside Edit and refusal of the unrelated global QAction. This validates the adapter, not the actual Resolve backend. Native preparation, caption extraction and real playback mode still require live acceptance in a fresh candidate session.

Stabilize now dispatches the original native Workflow Integration action by stable command label, independent of its numeric menu position and Inspector widgets. The generated script runs `GetCurrentTimeline().GetSelectedClips()` and `TimelineItem.Stabilize()` on selected video items in Edit. Empty/audio-only selection and absence of a current timeline fail explicitly; no playhead or media-pool substitute is used. Both menu and hotkey enter this script. The native helper refuses an older request-posting script to prevent recursive dispatch.

The script publishes a bounded atomic completion result. A read-only result watcher logs the API result, attempted count and completed count. Dispatch acceptance is not recorded as completed analysis. No external scripting preference or additional shortcut store is required. The existing native-preset adapter still supplies editable bindings; it remains opt-in for this acceptance candidate.

Fixtures cover hidden Inspector routing, changing native menu positions, disabled/ambiguous actions, absent/legacy wrappers, selected video/audio filtering and API failures, real wrapper execution with fake API objects, remap/preset-switch routing, and completion consumption without replay. Actual Resolve script-host invocation and analysis still need live acceptance.
