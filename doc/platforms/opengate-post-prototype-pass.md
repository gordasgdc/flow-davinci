# After the accepted 0.0.1 prototype

The published tag is immutable. The accepted prototype is the baseline for this fork's master, not a claim that every feature is complete. Keep subsequent implementation work in separate branches that can be compiled together for an install trial. Preserve saved arrangements, native keyboard assignments, pinning and group-door behavior.

## Before responsiveness work

- **Compatibility:** explicitly set and audit deployment targets for every C/C++/Objective-C++/Swift compile and link, the launcher metadata and installer gates. Check API availability before selecting the minimum OS. Inspect the finished Mach-O binaries and test the packaged build on the oldest OS we claim to support. Do not merely remove the macOS 27 installer check. Publish a new version; retain the exact 0.0.1 artifact.
- **Custom hotkey execution:** trace one native assignment all the way from key receipt to command resolution, bridge delivery, target invocation and visible outcome, with correlated diagnostics. Start with Source Monitor and preserve native assignments plus backtick. Listing commands is not success. Verify independent Source/Program doors, grouped panels, typing/dialog context, persistence and Stabilize/Fixed Playhead separately. Do not introduce another unsynchronized editor.
- **Duplicate rendering / controls:** the reported Timeline has two title rows and an overlapping-squares action beneath the group header. TimelineControl owns an injected unboundTimelineHeader; trace visibility across layout transfer, group rebuild and native recovery before removing it. Assert one usable placement control after each transition, while retaining a native Timeline control when ungrouped.
- **Unified placement affordance:** use one shared vector/control implementation for floating, group and toolbar surfaces rather than arrows, overlapping squares and Detach text doing inconsistent jobs. State-specific tooltips must distinguish Float, Attach and explicit Dock in Native Workspace. Hide/reveal remains a separate door action. Reuse existing placement ownership; do not accidentally return an arranged panel to the hidden native shell. Keep the pin light and existing group behavior.

## Responsiveness is last, investigation first

Only begin after the above features have an install candidate. First take a bounded, read-only investigation (at most 15 minutes) of one user-visible slow interaction against the currently installed baseline. Report the measured slow stage, one proposed change and its validation plan before optimization. Do not expand into a general performance rewrite or repeat benchmarks without a new reason. If there is no clear culprit within that bound, or the scope grows, stop and discuss the findings with Jordan. Fixture dispatch timings are not GPU presentation/playback measurements. Keep the existing performance budget honest.

## Merge / install discipline

Keep branch provenance and the checklist current. A compile or fixture pass does not establish live Resolve acceptance. Only replace the helper with Resolve closed; preserve both normal and trial settings, record the exact installed commit and retain rollback artifacts. Runtime updates and broader OS compatibility require another tested release. Neither this plan nor the prototype baseline merges anything into Wolfgang's upstream repository.
