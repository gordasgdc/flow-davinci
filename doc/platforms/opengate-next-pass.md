> Current status: see [the 2026-10-20 task audit](opengate-task-audit-20261020.md). This document retains historical requirements/evidence; old installed-build statements below are not the current installation.

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
| 8. Independent scaling | Reversible 70–150% per-panel native-metrics candidate implemented; full custom-painted control coverage and GPU acceptance pending | Per-panel minus/plus/100%, accurate input and GPU rendering, exact native metric restoration. |
| 9. Familiar first-launch layout | Real native startup retained; empty native branches compact and empty editing shell retires automatically; live acceptance pending | First launch looks familiar, later launches restore last view. Reset Default Layout stays rearrangeable; Original Resolve UI remains separate recovery. |
| 10. OpenGate identity | c9d6b1e installed with OpenGate launcher/workflow labels; original installed artifacts/settings archived | OpenGate by CineWaltz everywhere user-facing; Wolfgang foundation credit and MIT license retained. No Relay branding. |

Installed rollback: `00bef62`. The `2650fb3` candidate passed 18 functional suites; responsiveness failed only frame-budget assertions (docking commit 27.032 ms p95 and viewer command 19.795 ms p95). That is not acceptance of smoothness.

Priority update 2026-10-19: scaling and unified workspace take precedence over custom colors and further responsiveness experiments. The native-metric scaling candidate is explicitly experimental; custom-painted Resolve controls are not claimed covered. Fixed Playhead and Stabilize assignable UI dispatch routes have bounded lazy-target preconditions documented in opengate-edit-ui-commands.md. Performance gate remains open.

Installed candidate: **c9d6b1e**, 2026-10-19. All 24 functional suites passed on clean, unchanged source. Helper/bootstrap/launcher builds passed; installed signatures and byte hashes match the candidate. Original installation and settings archived in `build/macos/checkpoints/pre-c9d6b1ef-20261019T121831Z`. Resolve left closed for user acceptance. Main remains untouched. See `opengate-candidate-c9d6b1e.md` for exact test scope and limits.

Latest installation supersedes the above c9d6b1e record: **9def8a6**, installed
2026-10-19 after 28 clean-source functional suites passed. Includes OpenGate UI
panel callers, retained Edit page lifecycle, selected Source-page readiness
candidate, assignable hovered expansion and thirty native Workflow Integration
shortcut targets. Native keyboard-editor enumeration and actual Resolve page/
viewer acceptance remain pending. See `opengate-candidate-9def8a6.md`.
