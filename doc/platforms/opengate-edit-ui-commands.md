# Edit command candidate: visibility-independent Fixed Playhead

Fixed Playhead runs only on the active Edit page, regardless of timeline panel visibility, focus or clip selection. `PanelControls` supplies active-page identity separately from native panel visibility.

The candidate prepares Timeline View Options using the native `UiMenuButton::MenuAboutToShow()` Qt signal, which the installed native library emits before reading its menu and entering popup display. It does not send mouse input, show the menu, move focus or require a native shortcut assignment. It identifies the real Edit settings row by caption. Custom-painted captions are read through Qt's public QWidget rendering and QPaintEngine text callbacks into a bounded text-only device, without reading object memory or invoking binary addresses. It then triggers the native row's QAction. Ambiguous, disabled or absent targets fail closed.

The prior `viewTimelineScrollingFixed` fallback is removed: its Qt checkmark changed in logs without proving Edit scrolling changed. No success claim is based on that checkmark.

The isolated fixture passes both handler states with page, menu and custom-painted row hidden; it checks no focus change, rejection of another caption, rejection outside Edit and refusal of the unrelated global QAction. This validates the adapter, not the actual Resolve backend. Native preparation, caption extraction and real playback mode still require live acceptance in a fresh candidate session.

Stabilize is unchanged on this branch and still has the known visible-Inspector dependency. Its separate API replacement is not included here.
