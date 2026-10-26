# Native navigation checkpoint — October 26, 2026

This checkpoint restores native Media Pool double-click loading and destination
recall on ordinary launch. Jordan reports double-click, Match Frame and Find in
Media Pool working in the tested arrangement. Floating Source can still blink
briefly during loading; this checkpoint does not claim uninterrupted presentation
or completion of the broader OpenGate acceptance contract.

## Fixes and architecture

- Preserve the retained native Edit tree's effective visibility when the full
  managed arrangement has no native remainder. Its pixels and pointer interaction
  remain masked. The previously opt-in experiment is now automatic for that case.
- Observe Resolve's custom Media Pool view through its native Source activation
  during the double-click gesture. The old QAbstractItemView-only check rejected
  this real native view. Loading remains Resolve's responsibility.
- Reveal Source for native navigation and Media Pool for native Find through the
  existing retained panel door, preserving placement and grouping.
- Observe both native QAction commands and the existing Find QShortcut dispatch.
  Jordan's R shortcut does not emit the QAction trigger. Reuse the sole preset
  reader to identify the assignment; create no custom Find command, shortcut,
  replay, preference change or second visibility controller.
- Keep navigation observation in NativePanelNavigation and coordinator wiring in
  PanelControls. Reuse fixture setup and the existing preset model.

## Evidence and recovery

The installed candidate identifies itself as c7920d6-dirty. Its helper SHA256 is
8dc04d67cbd6726d63ebee5a607dea21ec66d44c1169e31d9d36e04436e7c542.
The dirty production source is included in this checkpoint, not omitted from it.
Local candidate and installation receipts reside in build/macos/find-shortcut-candidate
and build/macos/find-shortcut-install in the native-loading-normal-launch worktree.
All four installed payload files matched and 24 protected settings/preferences
were unchanged. Recovery transaction: EB9F2B5C-6230-4995-810E-5085AA7A010B.
Live session: 5de8867e-2df3-495b-86fd-2a2b6f9d0b31.

Universal macOS production compilation and bootstrap checks passed. Assembled
navigation fixtures cover native activation once, retained placement, remapped
Find, ambiguous dispatch exclusion and removal of stale shortcut associations.
Native preset regression passed. Fixtures do not establish native picture
acceptance; Jordan's live report supplies the tested behavior result.

## Remaining work and compatibility assumptions

Floating Source blinking remains open and is next. The native masked-tree
technique, native control identity and activation ordering are Resolve-specific
compatibility assumptions. Shortcut identity requires a readable supported saved
preset; missing/unsupported/unpersisted identity fails closed. Windows execution
is unverified. Reverse Match Frame and all other incomplete checklist items remain
open. No further speed optimization is part of this checkpoint.

Jordan authorized merging and pushing this checkpoint, then investigating the
blink. This is a source checkpoint, not a new installation or release package.
See opengate-native-navigation-reveal-20261026.md and
opengate-native-loading-reconciliation-20261026.md for detailed evidence/history.
