# Viewer maximize/restore latency

Branch `perf/macos-viewer-expansion-latency` starts at installed shortcut candidate
`8ae8bd6`. The focused fixture uses full PanelControls transitions, native floating
hosts, real split/tab groups and isolated temporary settings. It measures the
command callback, observed ready state, and first paint of a synthetic QWidget
viewer proxy. It excludes physical input, the Python shortcut ingress, video
decoding and GPU/display presentation.

Each placement gets one cold pair and 20 warm maximize/restore pairs. Every pair
must return the exact outer frame. The strengthened fixture checks split sizes and
tab selection and deliberately moves the underlying group during the cold
expansion to exercise full restoration. These extra assertions were added after
the first baseline; warm timing boundaries are unchanged.

Baseline production source is `8ae8bd6`. Results in milliseconds:

| Placement | Expand first proxy paint p50 / p95 | Restore first proxy paint p50 / p95 | Restore callback p50 / p95 |
| --- | --- | --- | --- |
| Standalone | 9.66 / 10.31 | 2.62 / 2.78 | 1.57 / 1.67 |
| Split group | 29.11 / 46.02 | 19.79 / 29.99 | 8.37 / 12.88 |
| Tabs | 29.50 / 45.80 | 21.40 / 31.50 | 7.15 / 11.91 |

There were zero readiness/paint timeouts. Raw reports are retained locally under
`build/macos/viewer-expansion-*.json`. This is a focused latency investigation,
not a pass of the broader responsiveness gate or proof of actual Resolve latency.

The installed shortcut candidate separately passed all 36 functional suites at
`build/macos/verification/8ae8bd6a76-functional-20261020T010321Z/receipt.json` in the
main checkout. Installation preserved workspace/profile and native keyboard
preference hashes. Performance work is separate from that installed runtime.

## One bounded optimization experiment

Restoration previously rebuilt the original group unconditionally after returning
the borrowed viewer content. The candidate compares the complete retained group
snapshot with the saved snapshot, and skips rebuilding only when they match.
A changed frame/tree/selection still takes the original restoration path.
`panel.expand.restore` records whether a group rebuild ran.

| Placement | Restore callback p50, baseline → candidate → repeat | Restore first-paint p95, baseline → candidate → repeat |
| --- | --- | --- |
| Standalone | 1.57 → 1.58 → 1.56 | 2.78 → 2.83 → 2.78 |
| Split group | 8.37 → 4.89 → 4.67 | 29.99 → 31.19 → 30.80 |
| Tabs | 7.15 → 4.76 → 5.10 | 31.50 → 31.51 → 32.96 |

The experiment consistently reduces grouped restore callback work, but does not
establish a first-paint tail-latency improvement. First-paint medians varied too:
tab restore measured 21.40 → 12.26 → 23.21 ms. Expansion production code was not
changed, so variation in expansion measurements is not attributed to this patch.

The full panel-session regression passes, covering expansion, backtick, original
ownership, docking, pinning and persistence. The strengthened focused fixture
passes with zero timeouts and exact frame/split/tab restoration, including the
changed-underlying-group fallback. The experiment is committed separately from
the benchmark and remains uninstalled and unmerged to the integration candidate.
The bounded investigation stops here; native window creation, GPU presentation
and actual input-to-video-frame latency need a separate measurement before a
larger design change. No responsiveness-gate pass is claimed.
