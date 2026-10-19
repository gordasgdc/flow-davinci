# Modular Edit recovery candidate

Live 1f73bf6 logs show Mixer visible inside the floating Timeline but rejected by the native-page ancestry precondition. Session detachment now accepts an explicitly supplied managed owner only when it contains the target and its floating host belongs to the same native main window. Docking keeps the original retained slot/layout; unrelated windows remain rejected. A native-session regression exercises Mixer detach, hide, reveal and exact layout restoration. Native-window/live acceptance is pending.

Show-only OpenGate callers reveal nested native panels through Resolve's real toggle controls, rather than trying to detach them again. Manager Show/Hide controls are available for native toggle-backed panels even before those panels have individual floating sessions. Native-control and requested/handled visibility results are logged.

Floating group visibility now recursively collapses empty tab/split branches, retaining GroupVisibility's saved divider masks. Children below an inactive outer tab retain logical visibility so later selection can reveal their contents. A hidden fixture covers an emptied Media Pool/Index column and exact width restoration.

Page Navigation is now an explicit row in the manager with Show, Hide, Float, Native Dock and Attach to Timeline. Native Show preserves native placement rather than unexpectedly floating it. Existing Timeline attachment and floating paths remain. Free grouping/arbitrary footer placement is not implemented by this change and must not be represented as complete. The starter workspace uses the supported Timeline-bottom attachment.

Deferred until after the prototype release: collapsed-drawer edge handles and richer discoverability. No new drawer chrome is added here.
