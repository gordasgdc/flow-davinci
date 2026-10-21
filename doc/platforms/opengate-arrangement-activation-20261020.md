# Arrangement activation report — before 0.0.2

Read-only investigation of installed runtime a6fdcf6, process 42993. No Resolve
actions or user settings were changed. Public 0.0.2 is not packaged/published.

## Confirmed events (UTC, 2026-10-20)

- 16:39:08.876: workspace.menu.command Restore, ID
  opengate-native-emulator-v1. The intended menu command reached the owner.
- 16:39:09.118: footer.dock.failed, attachment-ownership-changed.
- 16:39:11.481: arrangement.restore.failed, frameConformTimelines. Other
  panels had already docked, so this attempt can leave an intermediate layout.
- 16:39:32.561: same arrangement command again.
- 16:39:36.821: arrangement.restore for the same ID, unavailable empty.
  This is completion logging, not visual acceptance of every layout property.
- 16:40:38.596 through 16:40:52.840: five Source-to-Program native transfer
  attempts rejected with Choose available panels from this native page.

The records do not prove wrong-item dispatch or missed clicks. They establish
correct-ID dispatch followed by a failed restore, then success on repetition.
The broader W2 arrangement/mounting regression in the task audit remains open.

## Source observations and hypotheses

WorkspaceRestoration.preset docks existing panels before applying a target. Its
failure path does not restore the earlier layout. PanelControls.dockAll invokes
footer.dock (void) and continues even if footer ownership release fails. The
restore preflight sees the still-active Timeline and rejects the new target.
The temporal relationship is proven; footer failure as the cause of the Timeline
failure still needs a fixture/reproduction. Do not bypass ownership checks.

NativeArrangement.add has a compound guard for active/grouped state, same page,
native owner window and duplicate identity. Current generic error does not say
which condition rejected Source. Add bounded reason diagnostics and test actual
unloaded/loaded Source identities before changing these ownership requirements.

Workspace menu actions schedule work for the next Qt turn; unlike the command
submenu, they do not wait explicitly for native popup tracking to finish. This
is a review target, not an established cause of the reported restoration failure.

## Release gate

Hold 0.0.2 packaging/publication while diagnosing first-click restoration and
Source transfer rejection. Preserve the working hotkey baseline and main merge.
Work on fix/macos-arrangement-activation; no main change or new install yet.
Acceptance should cover first-click restores between Native Edit UI, Native
Emulator and existing saved arrangements, footer attached to Timeline, Source
unloaded/loaded, repeated switches and no unwanted changes to named presets.

## Scoped Source transfer fix

Confirmed source bug: NativeArrangement.add compared the session's original Edit
page with host->parentWidget(). A scoped arrangement is parented under its
restoration marker, so that check rejects valid same-page panels. The coordinator
now retains its actual initialization page and validates against that identity.
Its lifetime observer and retirement clear the page reference. Native owner
window, grouped/active state, ID and duplicate guards remain unchanged.

The native-arrangement fixture passes a new scoped Program/Timeline arrangement,
adding Source to Program, rejection of a foreign-page panel without mutation,
and restoration of original panel parentage. This fix is not installed or merged
to main; it does not resolve footer ownership or prove live acceptance.
