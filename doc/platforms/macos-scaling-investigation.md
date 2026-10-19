# Independent panel scaling investigation

Branch: experiment/macos-panel-scaling. This is research, not an implemented
scale control. Window resizing and viewer image zoom do not satisfy this feature.

Acceptance: independent minus/plus/100% controls; persist per stable panel ID;
text and controls scale together; keyboard focus, menus, hit coordinates and
GPU playback remain correct. Native docking/reset restores exact original
metrics. No QApplication font, stylesheet or process/display-wide scale changes.

Probe order:

1. Inventory explicit child sizes versus layout/style-derived metrics on
   Inspector and Media Pool. Keep any test changes scoped to a disposable host.
2. Test subtree QProxyStyle metrics/font/icon changes, including fixed-size native
   controls. Reject incomplete metric coverage rather than calling it full scale.
3. Evaluate actual geometric rendering transforms only with inverse input mapping
   and a verified GPU/viewer path. QGraphicsProxyWidget and Cocoa layer transforms
   are not assumed compatible with Resolve's native rendering.
4. Snapshot and restore only properties actually modified, using QPointer lifetime
   checks and idempotent restore. Compare original values after Dock All, project
   change, page change, shutdown and failed transfer.
5. Measure additional resize/paint cost with diagnostics enabled before exposing
   controls. Preserve the baseline build and native workspace constraints.

The current native clipping candidate changes owned group container views only;
it does not scale Resolve widgets and must not be represented as a scale feature.

## Native metrics candidate (2026-10-19)

`feature/macos-independent-panel-metrics` implements a reversible per-session
metric scale (70–150%). Group headers and the manager offer − / UI / +; panel
options offer the same percentages. The UI labels this experimental because
Resolve custom-painted constants are not covered by QWidget metrics.

Font, explicit minimum/maximum sizes, fixed control dimensions, icon sizes and
layout spacing/margins derive from captured baselines. No app font, stylesheet,
render transform, viewer-image zoom or process-wide scale changes. Lazy child
controls join on a coalesced child notification. Inherited fonts use their original
parent baseline, avoiding double scaling. GPU rendering is left native; live
coverage, sharpness, playback and pointer-hit verification remain required.

`interface_scale` persists per panel. Group transfer/expansion restore before
reparenting and reapply in the new host. Returning to Original Resolve UI restores
original metrics and preserves the preference for the next detachment. The
independent metric and actual session-transfer fixtures test both widget and
content-layout paths. This is not a claim of universal geometric scaling.
