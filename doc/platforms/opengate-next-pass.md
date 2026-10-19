# OpenGate next-pass acceptance list

OpenGate is by **CineWaltz**, built on **Unbound by Wolfgang Woehl (MIT)**. It is a separate macOS fork. Existing internal identifiers and storage locations are retained for compatibility, not used as the product label.

Implementation, fixture checks and live Resolve acceptance are separate. None of the items below is closed by compilation alone.

| Priority / item | Current state | Required acceptance |
| --- | --- | --- |
| 1. Reopen the last view | Autosave/startup restore exists; pin-only autosave and footer pin snapshot fixed; two-process modified-view fixtures pass for floating and attached footer; actual Resolve restart acceptance pending | Restart Resolve: restore latest modified layout, not merely a named preset. Match display/frame, splits, tab selection, hidden doors, pinning, dividers, footer, toolbar and workspace identity. No native-layout fallback or duplicate shell. |
| 2. Independent panel commands | Configurable OpenGate commands implemented and hidden fixtures pass | Source affects only Source; Program only Program. Reuse grouped doors. Retrieve panels from hidden native shell. Verify typing/dialog conflicts and persistence. Native keyboard-editor registration remains unverified. |
| 3. Timeline undock | Dedicated reversible native control implemented; fixture checks pass | Attached toolbar travels with Timeline. Explicitly detached tools stay separate. Verify actual Resolve control visibility and drag. |
| 4. Stacking/pinned doors | Timeline-group/Inspector policy changed; pinned-door fixture passed | Confirm user-visible Inspector stays above primary workspace and pin never prevents door changes. Identify actual native hosts in logs. |
| 5. Responsiveness/blinking | Compared with installed-source baseline; candidate functional assertions pass, dock commit remains ~26 ms p95 and fails 60 Hz gate | Measure against installed baseline. No redundant hide/show/order churn; retain rendering, diagnostics and interaction behavior. |
| 6. Red/custom clip hues | Guarded native-color and namespaced metadata probes prepared; no live storage result or color feature delivered | Prove rendering/storage/reopen/undo and safe native fallback without clip or edit loss. Picker/names/commands follow verified storage. |
| 7. Adaptive toolbar + attachments | Footer-to-Timeline implementation integrated after hidden ownership, displayed groups and both restart fixtures pass. Combined/live acceptance pending; toolbar orientation/general attachments not delivered | Wrapping/vertical layout, left/right/top/bottom placement, toolbar follows its explicitly attached panel until detached. Page Navigation defaults to Timeline’s bottom edge, travels with Timeline until explicitly detached, remains separately callable, and restores its attachment on reopen. Preserve native actions/states. |
| 8. Independent scaling | Investigation only | Per-panel minus/plus/100%, accurate input and GPU rendering, exact native metric restoration. |
| 9. Familiar first-launch layout | Native-first startup exists; unified movable default not delivered | First launch looks familiar, later launches restore last view. Reset Default Layout stays rearrangeable; Original Resolve UI remains separate recovery. |
| 10. OpenGate identity | Candidate labels, launcher, menu and diagnostics built and checked; installed build unchanged | OpenGate by CineWaltz everywhere user-facing; Wolfgang foundation credit and MIT license retained. No Relay branding. |

Installed rollback: `00bef62`. The `2650fb3` candidate passed 18 functional suites; responsiveness failed only frame-budget assertions (docking commit 27.032 ms p95 and viewer command 19.795 ms p95). That is not acceptance of smoothness.
