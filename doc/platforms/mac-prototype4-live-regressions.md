# Prototype 4 live regressions — 2026-10-23

Current repair branch: `fix/mac-v4-source-mixer-regression`, based on release tag
`0052c09`. Installed runtime reported by the manifest and live events is
`d749352`. Jordan installed prototype.4 and reports failures; it is not live
accepted. Earlier fixture passes do not supersede these reports.

## Reproduction evidence

- Double-clicking Media Pool media does not load it into Source Monitor in
  OpenGate. This affects ordinary clips as well as compound clips. Dragging media
  into Source works. Jordan confirms the same double-click works after choosing
  Use Original Resolve UI. Floating Source separately remains an isolation check.
- In Original Resolve UI, **backtick/tilde temporary panel maximize** produces
  paint corruption that persists until switching into and out of OpenGate.
  An earlier interpretation as OS minimize/restore was incorrect and is superseded.
- Panels overlaps a native Inspector control in Original UI. Jordan explicitly
  clarified that the title accessory belongs only in OpenGate workspaces. The
  numbered contract now reflects this; native menu access remains available.
- Floating Mixer then closing its host returns it to its native slot. That is
  also the v3 implementation. Jordan's question about X was not approval to
  redefine its behavior. No X behavior change is part of this repair.
- Source Monitor shortcut does not fire. Live records show Shift+W blocked by
  `native-preset-conflict: viewReferenceWipeStyleCycle`. This conflict also exists
  in older logs; no preset has been changed and no conflict guard bypassed.

Read-only copies are retained under `build/regression-evidence/live-first-report`
and `live-20261023T003311`, with rotated events included. In session
`0a3227f7-9370-42cf-a378-d5915810aa39`, sequences 8319, 8343, 8367 and 8390 record
Qt double-click receipt. Subsequent native Source `FocusReceived` and focus
transitions are present, with the player visible. **These do not establish clip
loading or correct painting.** Initial analysis missed double-clicks by reading
the wrong JSON nesting and omitting rotated events; the corrected result above
supersedes it. Native activation and successful display are distinct observations.

## Accepted-source comparison

Compared accepted runtime `88988d4` (production source also at accepted main
`28ff7d6`) against installed runtime `d749352`, following Mac-to-core file moves.
Diffs are retained under `build/regression-evidence/*.h.diff`.

| Area | Comparison result |
| --- | --- |
| Source preparation and Source page reconciliation | `viewerpreparation.h` and `sourceplayerpresentation.h` byte-identical after move |
| Overflow and native position | `paneloverflow.h`, `nativepanelposition.h`, `vacatedcontents.h`, `panelmetricscale.h` byte-identical after move |
| Viewer mode/action registry | `viewermodecommand.h`, `vieweractionregistry.h` byte-identical after move |
| Native root layout and workspace headers | Include redirected to platform contract; original behavior retained |
| Temporary expansion controller | Session type renamed to shared type; transaction logic unchanged |
| Panel session and floating host | Shared type names/platform adapter introduced; Mac host adapter aliases QWidget |
| Mac native chrome | No source difference between these checkpoints |
| Mac widget probe | Diagnostic directory changed from Unbound to OpenGate |

This comparison has not identified the cause of the two live viewer failures.
It does not prove v3 and v4 behave equivalently in Resolve. No old binary has
been installed for A/B testing, and no live v3 retest is claimed.

## Repair and evidence ledger

| Change | Implemented | Compiled / fixtures | Packaged | Installed | Live accepted | Published |
| --- | --- | --- | --- | --- | --- | --- |
| Panels title accessory only in OpenGate mode | Yes | Hidden workspace-headers and assembled opengate-first-open pass | No | No | Pending | No |
| Double-click/native item activation/Source selection diagnostics | Yes | Shared diagnostics, interaction audit, diagnostics and Source presentation pass on Mac | No | No | Pending | No |
| Source media loading repair | Cause unconfirmed | Pending | No | No | Failing report | No |
| Native backtick painting repair | Cause unconfirmed | Pending | No | No | Failing report | No |

The diagnostic test sends a real Qt item-view double-click and checks that its
native handler runs once, changes Source selection, preserves ownership and does
not expose a fixture clip name. It does not simulate Resolve decoding/GPU output.
New observers never invoke a private Resolve method or activate media themselves.

Focused logs are in `build/regression-evidence/`. The first shared-diagnostic run
compiled but aborted because sandboxed Qt had no screen access; that failed log
is retained. The permitted outside-sandbox hidden run passed. Full Mac regression
matrix, displayed checks and Windows runner validation of these edits remain
pending. This is not an installation-ready repair candidate.

The installed application, profiles, native preferences, projects and media have
not been changed by this investigation. No Resolve process has been terminated.

## Controller-preservation experiment

The corrected live comparison adds a specific clue: successful Native-mode
double-clicks at event sequences 9760 and 9848 are followed by two native
`btnSourceClipMode` pressed/clicked pairs. The failing managed attempts lack that
step. No OpenGate code invokes those buttons. This narrows the activation gap but
does not prove why Resolve skips it.

Mac inventory shows `WStackPage_Conform -> m_pConformPanel
(UiConformerPanelImp) -> native panel containers`. The existing root lease moves
that controller into a hidden stash and moves arranged players outside its
ancestry. The experiment leases the controller's presentation layout instead,
keeping the controller visible, inside its original page and above arranged
players. It extends the existing root lease, not a new layout engine. Mac adapter
discovery uses the direct child name observed in this Resolve inventory. Windows
retains its current page-owner discovery until its native topology is verified;
the shared nested-owner primitive is covered by the shared ownership fixture.

The first realistic-controller assembled run exposed wrong maximize bounds:
the outer controller rectangle includes a reserved strip. Production viewport
selection was corrected to use the actual managed body. Original assertions and
thresholds were retained. Failed log:
`opengate-first-open-native-controller-first.log`; passing rerun:
`opengate-first-open-native-controller-viewport.log`.

An equivalent hidden fixture compiled against unchanged accepted v3 source also
fails the new visible-controller ancestry assertion. It additionally fails the
new Native-mode Panels policy, as expected. The first baseline compilation failed
on the renamed host type; the retry maps only `WorkspaceFloatingHost` to its v3
name `MacFloatingHost`. Both build logs and the failing baseline flow are retained.
This demonstrates a pre-existing ownership gap, **not** a reproduced v3 clip-load
failure. No old binary was installed and no media decoding was exercised.

`b8de266e9a-hidden-20261023T054325Z/receipt.json` records 52/52 clean hidden passes
for the earlier Panels/diagnostic checkpoint. It does not cover this subsequent
ownership experiment. Its new complete matrix must include `native-controller-flow`.
The experiment has focused shared ownership, native entry and assembled flow
passes; complete hidden/displayed matrices, Windows runner coverage and live
Source loading remain pending. No fix for native backtick painting is claimed.
