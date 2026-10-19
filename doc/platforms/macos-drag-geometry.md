# Drag coordinate and benchmark input diagnosis

An intermittent one-pixel stress failure initially looked like a geometry error. Read-only sampling found no difference between requested and Qt geometry, but the first drag had only two sampled stages (one move). Lifecycle evidence showed `drag.end` reason `button-no-longer-down` before the benchmark's intended release. Programmatic native-window movement had generated an unsolicited no-button hover move while the fixture was injecting a held-button stream. Production correctly cancelled that no-button drag; the fixture then continued measuring a gesture it had already ended.

`EventWorkApplication` now isolates only the benchmark's explicitly scoped scripted pointer stream from no-left-button mouse moves and reports the ignored count. Its explicit releases, deactivation, exact-position and frame-budget assertions remain. Production input/cancellation is unchanged. The ordinary panel-session fixture separately checks missed releases and stale moves without this isolation. Both baseline and candidate comparisons must use this same corrected harness.

With isolation, all functional assertions passed, including exact tracking and explicit release. One run reported commit 27.931 ms and viewer 18.808 ms p95, so the unchanged 16.667 ms gate still fails. Do not claim smoothness or a speed win from fixing the benchmark input model.

`DragGeometryTrace` records one detailed mismatch at most per drag plus aggregate counts. It compares Qt and QWindow geometry with requested coordinates, and queries Cocoa content/frame/backing scale only when a mismatch occurs. It never compensates positions or writes native geometry. This is diagnostics, not a coordinate workaround.
