# Workspace restoration ownership

`WorkspaceRestoration` owns preset preflight and restore sequencing, startup native-anchor/panel resume ordering, and delayed group/background settlement. It knows panel session descriptors and the native workspace binding; it cannot access the manager's private fields or widgets. Applying UI preferences and initiating individual panel transitions are narrow callbacks.

The manager delegates preset sequencing and startup resume. It still builds its UI and coordinates groups; this is not a claim that its overall size/ownership problem is finished.

A generation invalidates superseded delayed group/background work. Preset replacement, native resume, Dock All and shutdown cancel older work. Existing 120/200 ms settlement delays remain; removing them requires actual readiness evidence, not guessed shorter delays.

Core-only tests pass missing-preset/inactive-page refusal, validation-before-docking, blocked-dock and apply-error boundaries, resume ordering, unavailable-panel reporting, superseded callbacks, explicit cancellation and shutdown. Group and fresh-process restoration fixtures compile. Displayed combined acceptance remains required before installation.
