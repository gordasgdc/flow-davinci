# OpenGate UI menu pass

Rename the top-level Workspace UI menu to OpenGate UI, preserving its internal
identity and saved workspace data. Add Show Panel in Workspace → Edit, listing all registered
Edit panels including Source Monitor, Program Monitor, Timeline, both toolbars,
Mixer and Page Navigation. Callers reveal existing groups/tabs without hiding an
already visible panel; an unmanaged panel uses the existing detachment route.
Entries are disabled on other pages for now, and queued requests validate page
context again. Native recovery and saved workspaces remain available.

The shortcut editor tooltip distinguishes OpenGate bindings from Resolve's
native Keyboard Customization registry. The native registry currently exposes
the OpenGate Workflow Integration launcher only, not the individual commands.

User reports during this pass: Timeline track/header/ruler/clip-lane scaling is
not established by the native-metrics implementation, although other panel
scaling appears useful. Source Monitor startup displayed blank contents until
redocking/re-detaching; live logs show source preparation unavailable at startup
and retain the source/offline stack, but do not yet prove why its inner page was
blank. These remain separate issues rather than completed menu work.
