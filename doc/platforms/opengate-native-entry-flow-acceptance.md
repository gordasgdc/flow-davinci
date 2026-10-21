# OpenGate workspace entry: current acceptance contract

This supersedes the earlier native-slot-first interpretation. Implementation and
fixture results are not live Resolve acceptance. Detailed evidence and gaps live
in opengate-workspace-flow-v2.md and opengate-task-audit-20261020.md.

1. First launch enters OpenGate in Resolve Layout, reusing the existing emulator
   panel tree and real native controls. No emulator selection, background switch
   or Panel Manager setup. Later launches restore the last working state for each
   supported page, including unsaved modifications while Resolve Layout is selected.
2. OpenGate Workspaces lists the immutable Resolve Layout first, then saved user
   workspaces. Update Current Workspace is disabled for that built-in and unnamed
   views, enforced below the UI. Save as New creates a writable user workspace.
3. Use Original Resolve UI restores the real native layout/owners, preserving the
   last complete OpenGate checkpoint. Selecting an OpenGate workspace re-enters it.
4. Hotkeys and temporary backtick maximize/restore remain usable in Native mode;
   temporary expansion does not switch modes or load a default arrangement.
5. A persistent title drag, detach or grouping action in Native enters OpenGate
   and completes the gesture without loading Resolve Layout or resetting unrelated
   panel positions. Native-origin changes form an unnamed working arrangement.
6. One primary workspace owns the native-style Workspace Header above its panel
   area and complete native Page Navigation below it, including Home and Settings.
   Floating groups never take ownership based on size, focus or Timeline location.
7. Each band is independently pinned or hidden. Hiding releases its height;
   showing reserves it. Docked content and maximize use the actual content viewport
   with both bands, either one or neither. Ordinary floating windows may overlap.
8. Drag visible panel titles as well as detach buttons. Ordinary native clicks
   retain their behavior. Remaining docked content reclaims vacated space; native
   visibility updates cannot reopen empty leased slots. Detach does not shrink
   the user's primary window. Native return restores exact owners and constraints.
9. Panels is a flat current-page inventory including Workspace Header and Page
   Navigation. Optional parked panels must be reachable. No All Panels, Edit,
   Workspace or Floating Windows inventory subtrees. Header controls cannot overlap
   title/Inspector or become unreachable. Menubar Panels remains available with
   the header hidden; check titlebar/button discoverability in live Resolve.
10. Native painting must remain correct throughout resize, stacked timelines,
    docking and maximize. A blank timeline until a width threshold is a live bug;
    dummy-widget fixtures cannot prove this fixed. Preserve native minima or
    provide reachable overflow instead of breaking painting.
11. Supported pages retain their independent working state on departure/return.
    Unsupported pages keep native behavior. Fusion adapters follow separately,
    sharing these contracts; Windows parity needs its own integration and tests.
12. Exercise the assembled first-open -> title drag -> vacancy reclaim -> band
    combinations -> maximize/restore -> Save/Update protection -> restart -> Native
    -> temporary maximize -> persistent Native gesture -> OpenGate selection flow.
    Then live-test actual pointer gestures, native GPU rendering, page switching,
    remapped keys and Relay/Deliver before release acceptance. Keep build, fixture,
    installation and live acceptance evidence separate. No overstatement.
