# Combined arrangement candidate

Installed baseline remains 091268b. The assembled candidate preserves accepted hotkeys
and Source docking and includes separately reviewable changes for:

- Pinned native workspace header and permanent title-bar Panels access.
- Flat canonical panel visibility entries on every surface.
- The original complete native footer, including Home/Settings, on the primary surface;
  native-painted callable copies on optional secondary surfaces.
- Timeline overflow with native constraints preserved and attached-toolbar recovery.
- Continuous native-tab viewport with a separate padded scrollbar row; native control
  metric range 70–300%, with combined tab/scale fixture coverage.
- Page-aware preset controls and independent last-view/mode/selection persistence.
- Automatic Native/managed mode inferred from actual ownership, preserving the current
  layout and preset identity. No starter preset is loaded on undock.
- Session/sequence-correlated control, page, command and ownership diagnostics;
  durable crash breadcrumbs and bounded read-only lazy page inventories.
- Universal macOS helper/launcher builds with an explicit deployment target and
  recorded Resolve build-host requirements.

## Evidence and open work

The clean 94e8704 build passed all 31 hidden suites, universal compilation, bootstrap
and launcher self-tests, and binary/signature audit. Later invalid-file preservation
and Resolve-host provenance follow-ups have dedicated passing fixtures and require
an updated combined rebuild/matrix. Feature lock and receipts identify exact source.
Displayed/native-window and fresh-process restart suites require Resolve closed.
No live test, installation, main merge, package or publication follows from fixtures.

Fusion panel undocking is NOT implemented: the available startup snapshot contains
its outer container, not verified lazy panel ownership. A loaded Fusion inventory is
needed before adding adapters to the existing sessions/commands. Fusion stays behind
the existing arrangement fixes, with functional branches separately reviewable.
Native-painted Timeline track/ruler scaling has no verified hook and remains open;
300% Qt metric support must not be described as full Timeline scaling.

Installed Resolve is 21.1.1 (21.1.10010), with an application minimum of macOS 15.
Our helper target 11.0 does not reduce Resolve's own requirement. Older OS, native Intel
hardware, other Resolve versions and Windows feature parity are not live validated.
GitHub default is main, but the repository is still a fork; standalone detachment is
pending. No history deletion or replacement repository was performed.

## Live acceptance before packaging

- Panels stays available with both bands hidden; each panel appears once and checks
  agree with actual visibility.
- Header stays pinned; native controls/signals and Show/Hide callers agree; legacy
  toolbar groups migrate without changing other panel arrangements.
- Original footer has Home/Settings, native navigation and synchronized Show Page
  Navigation; pin choices and native original-slot restoration survive page changes.
- Cramped Timeline and stacked timelines retain reachable content/toolbar and valid
  clipping/input. Native minima are preserved rather than silently reduced.
- Tab scrolling is continuous, with padding at each scale, and native close/reorder/
  context/plus actions retain selection and ownership through page changes/restart.
- Stabilize, Fixed Playhead, backtick, native remapping, alternates and cleared bindings
  still work from native and floating surfaces.
- Per-page last mode, selected preset and unsaved layout survive return/restart;
  Native return preserves the previous managed view and never loads a starter preset.
- Logs identify the build/session, request route, target and outcome; page inventories
  supply the missing Fusion/Timeline capability evidence without native mutations.

Responsiveness remains on hold. Scrolling and each feature commit can be omitted from
an assembled build without discarding the shared foundations or the other fixes.
