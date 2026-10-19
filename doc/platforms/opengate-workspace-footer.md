# Workspace footer candidate

Page Navigation attaches at the bottom of the primary modular workspace host,
outside the Timeline tile. Prefer the visible Timeline host, otherwise the largest
visible content host; the independent Workspace Toolbar is excluded. Rebuilding
a group inserts content above the footer. Explicit native/floating placements
remain available, and an unavailable modular owner uses the native footer.

Legacy `attachment: timeline` settings are accepted and reported as `workspace`
after attachment. Saved arrangement files are not rewritten by this migration.
Changing workspace owners releases the old viewport before acquiring the new one.

The native Show Page Navigation action and OpenGate visibility controls share the
same footer owner. Native check-state updates are signal-blocked; native action
dispatch applies visibility after its original handler completes. No second F12
shortcut is registered. The actual native action is found by its English label,
so localized Resolve action discovery remains unverified.

The hidden accessory fixture passes ownership, overflow, owner destruction,
owner changes, native action dispatch/check-state, legacy preference loading and
native fallback without an unexpected floating window. This does not establish
actual F12 routing, full-width native rendering, group rebuild geometry, playback,
or page-switch safety inside Resolve. Displayed reopen and live acceptance remain
pending. Use the separately reviewed page-transition safety branch in the combined
test candidate; it defers existing native ownership changes outside Hide dispatch.
