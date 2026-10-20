> Current status: see [the 2026-10-20 task audit](opengate-task-audit-20261020.md). This document retains historical requirements/evidence; old installed-build statements below are not the current installation.

# One shortcut editor

User preference: retain Resolve's existing native assignments and use its native
keyboard editor as the assignment surface. The prior OpenGate dialog wrote a
separate Shortcuts.json and did not reflect changes to native presets. This was
an implementation gap, not synchronization.

This candidate routes the OpenGate command-menu entry to Resolve's real Keyboard
Customization window. A narrow native command-search lookup filters OpenGate
when exactly one search field is identified in UiActionsEditorCommandsWidgetImp;
otherwise the native editor remains available without a guessed filter. It does
not create a second assignment dialog or write the native preset format. Existing
legacy Shortcuts.json data and compatibility bindings are preserved, not deleted
or silently migrated. No native assignments are changed by this correction.

The native preset audit found existing application commands for Inspector, Media
Pool, Effects, Index, Mixer, Metadata, Sound Library, Keyframes and Page Navigation.
Source/Timeline Viewer focus commands and a viewer-context Show Source Viewer
command also exist. Focus, explicit show and independent door toggles are distinct;
no native focus command is redefined into a hide toggle based on its label.
The native panel toggle bridge remains the route for managed existing native
panel commands. A fully bidirectional legacy-binding migration is not delivered
by this candidate; the preferred user-facing assignment widget is now native.

Hidden navigation fixture passes for native command invocation, unique search
filtering, ambiguous-field refusal and unavailable editor handling. Live search
filter coverage remains pending. The change is prepared on its feature branch;
installed 9def8a6 still has the separate dialog until the next combined install.
