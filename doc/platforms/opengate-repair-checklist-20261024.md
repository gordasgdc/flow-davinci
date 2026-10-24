# OpenGate repair checklist

The next repair is **close, hide, and recall for floating panels and groups**. Jordan does not need to review the technical assessment line by line. The intended behavior is already clear; implementation and verification are the engineering responsibility.

Reference: [behavior review](https://chatgpt.com/space/page_23161bac62ac81918906bc4da65af2b5), Jordan’s annotated HTML, and the source assessment of build `ee078c2` on October 24, 2026. The source assessment is complete. The repairs below have not been implemented.

# First repair

**Required behavior:** X hides an OpenGate floating panel or whole group. Recall brings back the same host, size, position, pinning, and group membership. Explicit docking remains a separate operation. Match Frame must still load the correct source frame as well as reveal Source.

**Confirmed defect:** `src/core/panelsession.h:533` handles Close by docking. `src/core/panelgroup.h:286` handles Close by dissolving the group and docking its members. Existing session and group tests explicitly expect this outdated behavior.

- [ ] Implement on an isolated branch while preserving the installed build.

- [ ] Route ordinary managed-window Close through existing hide mechanisms; keep explicit docking, temporary expansion, native return, and application shutdown correct.

- [ ] Replace contradictory tests with checks of retained host identity, geometry, pinning, group membership, and hidden state.

- [ ] Verify recall through existing panel callers, including a group with hidden members and nested tabs.

- [ ] Check saved and unsaved restart behavior, page departure and return, explicit dock, and shutdown.

- [ ] Produce a reviewable diff and separate compilation and fixture results.

- [ ] Obtain Jordan’s explicit authorization before installing a candidate.

- [ ] Verify the real X button and recall in Resolve, including the correct Source picture for navigation commands.

Do not claim this repair fixes playback disappearance, blinking, or native loading. Those have separate acceptance checks.

# Remaining repair order

1. **Startup and Timeline sizing.** Compare saved divider sizes, applied sizes, and final native content bounds. The reported screenshot has a visible Timeline header but an unusably compressed track area. Check first launch separately from restored layouts; preserve checkpoints rather than resetting arrangements.

2. **Visibility and recovery.** Identify the actual cause of floating-window disappearance and blinking. Separate explicit user visibility requests, native-content recovery, tab selection, and window ordering. Remove a conflicting decision only when demonstrated; do not add another keep-showing timer.

3. **Native loading and command reliability.** Preserve successful Source evidence and resolve the experimental launch-flag dependency. Verify double-click A then B, Match Frame, and Find in Media Pool separately, with correct pictures or clip selection and no layout changes.

4. **Dragging and docking targets.** Distinguish docking beside one panel from docking beside a whole stack. Check preview bounds and release results. Investigate deactivation cancellation separately.

5. **Labels and control consistency.** Fix clipped names and make Mixer’s drag affordance understandable while preserving its nested Timeline starting position.

These tracks may share findings, but their fixes and acceptance results must remain distinguishable.

# Code organization rules

- Reuse the existing layout tree and persistence format. Do not add a competing layout engine or a second independent state model.

- Make each component’s decision responsibility explicit. A new production state representation must replace an existing decision path, not sit beside it.

- Separate behavior repairs from mechanical file splitting. The large coordinator needs narrower responsibilities; line-count reduction alone is not success.

- Preserve native ownership restoration, grouping, tabs, magnetic docking, maximize, hotkeys, Fixed Playhead, Stabilize without Inspector, hideable bars, saved and unsaved arrangements, and the shared Windows foundation.

- Keep implementation, compilation, fixtures, installation, and live acceptance separate. A passing test that asserts the wrong behavior is not acceptance.

- Preserve a reliable native return underneath any future simplification of its user-facing controls.

# What Jordan needs to do

No further specification review is needed to start the first repair. Correct requirements only if they do not match the desired experience.

After local work is complete, Jordan receives a short explanation, a reviewable change, and a narrowly defined live test. Installation requires explicit authorization. No new layout tour or reconstruction of previously reported failures is required.

**Current status:** assessment complete; first repair pending implementation; installed software unchanged by the assessment.

