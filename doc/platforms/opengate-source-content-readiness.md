# Source content readiness candidate

Live c9d6b1e logs from 2026-10-19 show unavailable native Source preparation,
a hidden source/offline stack being detached, followed by an outer-stack Hide
and recovery. The user reports an empty window until docking/undocking. Existing
logs did not expose the selected child page, so they do not prove the root cause.

A hidden Qt fixture reproduces one matching failure mode: showing the outer
QStackedWidget does not reveal an explicitly hidden current child. A scoped
boundary reconciliation reveals that same selected child only when its Source
stack is actually visible and Cinema Viewer is inactive. It does not switch
stack indices, load clips, recreate GPU surfaces or disturb closed doors.
Ownership/group/door/recovery boundaries log current index, each native child
page state, selected-page state and any reveal. This provides evidence to
separate page visibility from a remaining GPU/rendering issue on the next run.

The fixture passes for selected Source, selected offline page, closed door and
unrelated stack. Actual Resolve blank-surface acceptance remains pending.
