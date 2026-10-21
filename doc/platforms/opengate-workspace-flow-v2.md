# OpenGate / Native flow — acceptance contract, 2026-10-21

Status: implementation in progress. Installed 85fc822 fails native vacancy reclamation and header/title overlap in Resolve. Its passing component fixtures do not establish this contract.

## User flow

- First open enters OpenGate with the immutable Resolve Layout. Reopening restores each supported page's last working state, including unsaved changes; never reload the default merely because it is selected.
- OpenGate Workspaces lists Resolve Layout first, then named user workspaces. Update Current Workspace is disabled for Resolve Layout and unnamed views and enforced below the UI. Save as New Workspace creates a writable copy.
- Use Original Resolve UI returns native owners/slots without deleting the OpenGate checkpoint. Backtick maximize/restore and non-layout commands still work there without changing modes.
- Starting a persistent drag/undock/group operation in Native enters OpenGate and completes that gesture without loading a default or discarding the visible arrangement.
- One primary workspace owns Workspace Header at its top and complete native Page Navigation at its bottom. These are pinned or hidden; they are not group leaves or inferred from the largest floating window. Ordinary floating windows may overlap them.
- Docked content and temporary maximize use the workspace content viewport with both bands, either band or neither. A hidden band releases its space.
- Grab visible panel titles as well as placement buttons. Native button clicks retain their normal behavior; drag threshold separates click from rearrangement.
- Moving/hiding a panel closes its old footprint. Native visibility/layout updates cannot reopen empty leased slots. Restore returns exact native ownership, constraints and controls.
- Panels is one flat current-page inventory, including Workspace Header and Page Navigation. No All Panels, Edit or Workspace category layers.
- Native panels retain correct rendering during narrow/wide resize, stacked timelines, docking and maximize; no blank timeline at smaller widths. Preserve real native minimums or provide reachable overflow instead of compressing into broken painting.
- Fusion follows in isolated work using the same shared contracts. No claim of Windows parity, older macOS or Resolve Free support without platform-specific integration and validation.

## Evidence required before ready-for-install

Record implementation paths, relevant regression results, and unresolved live gates separately. Exercise the assembled first-launch -> title drag -> vacancy reclamation -> hide/show bands -> maximize/restore -> save/update protection -> restart -> Native -> hotkeys -> drag into OpenGate sequence. Do not close a live failure with a component fixture. Review this contract and the user discussion before claiming readiness. Current install remains unchanged during development.

## Current implementation checkpoint (not ready for installation)

- Shared preset identity/protection and click-versus-drag gesture modules added under src/core. Windows integration is still pending; these modules do not establish Windows parity.
- The existing bundled emulator tree now has an embedded managed-plan form, using NativeArrangement/NativeRootLayout and existing panel sessions. Header/footer are excluded from that tree. First-launch seed and compiled recipe agree; legacy records remain hidden, preserved and recoverable.
- Explicit mode persists independently of transient maximize sessions. Native persistent gestures enter managed mode without loading a preset; a new Native-origin gesture is unnamed, so it cannot silently update a previously selected saved workspace.
- The primary band owner is the native main workspace, independent of Timeline location and floating-window size. Main Panels uses a real native-header layout slot where available; menubar Panels remains available with the header hidden. No Floating Windows inventory subtree.
- VacatedContents now guards both Show and ShowToParent while its native content is leased. Releasing the lease removes the guard before restoring native visibility. Native sizing lowers the resize floor without automatically shrinking the user's frame.
- Focused hidden tests passed for native-slot guard, immutable policy/recipe, title click/drag, existing embedded arrangement ownership, the revised Native entry sequence, and first-open title drag/band combinations/Native return/re-entry. The new first-open test also passed a separate-process restart with an unsaved detached panel and hidden header while Resolve Layout remained selected.

Outstanding before readiness: complete regression matrix and build; saved workspace Update/Save As assembled coverage; all optional panels in the embedded workspace; native and managed temporary maximize mode invariants; actual narrow timeline/GPU rendering and stacked timelines; live Native-origin drag space reclamation; primary header placement/styling in Resolve; page switching and Relay/Deliver. No installation, publication, main merge or cross-platform acceptance follows from this checkpoint.
