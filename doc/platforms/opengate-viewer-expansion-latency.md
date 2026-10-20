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
