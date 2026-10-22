# OpenGate prototype delivery contract

Jordan's 2026-10-22 requirements: equivalent Mac/Windows panel behavior, focused
modules, a double-click install, reversible version changes, preserved presets,
easy removal back to native Resolve, and a support report users can send manually.
This extends the numbered native-entry acceptance contract; it does not replace it.

## User experience

- macOS downloads an OpenGate `.pkg`; Windows downloads an OpenGate installer.
  A `.pkg` cannot execute on Windows. GitHub releases eventually offer both assets
  with clear architecture/version labels. Publishing remains a separate action.
- Each installer checks OS, architecture, supported Resolve version/location,
  whether Resolve is running, installed OpenGate version/build, permissions and
  settings schema compatibility before replacing any files. A running Resolve
  yields a clear save/quit/retry message; never force termination.
- A first install starts in immutable Resolve Layout with real movable panels.
  Jordan Tetewsky Preset is an option. Existing users retain their last working
  state and all user presets across upgrades and compatible downgrades.
- Clean first-user preview is explicit and isolated from normal data. Installing
  another version never silently resets a profile to simulate a first launch.
- App replacement and user-data migration are separate transactions. Make a
  recoverable checkpoint before migration. An older release must reject or isolate
  unsupported newer settings without overwriting them. Do not promise arbitrary
  downgrade compatibility until a supported version-pair matrix passes.
- Uninstall is discoverable and double-clickable. With Resolve closed it removes
  OpenGate's loader/helper/launcher/workflow entry points, leaving native Resolve
  on next launch. Keep OpenGate presets/settings by default for reinstall.
  Never touch Resolve projects, media, databases or unrelated plugins. Remove
  user data only through a separate explicit choice.
- Export Support Report is discoverable, user initiated, and produces a file the
  user can attach. No automatic upload. Record install and runtime versions,
  source/build IDs, architecture, OS and Resolve versions, schema versions,
  transaction ID, phase, result, error and recovery location. Exclude projects,
  media and credentials; minimize/redact user paths and project titles. Describe
  any optional raw logs before including them. Failed preflight must also produce
  an accessible report; not only successful installations.

## Ordered implementation and acceptance tasks

1. Finish Mac Mixer diagnosis/correctness on its isolated branch. Verify the
   exact Edit widget, title drag, placement control lifetime, door highlights,
   float/group/dock/maximize, page return and exact Native ownership restoration.
   The alternate timeline Mixer unmatched logs alone do not prove this defect.
2. Finish measured menu/loading improvements without shortening readiness guards
   or parallelizing mutually exclusive native acquisitions. Record before/after
   phase timings with the same user sequence; investigate Index readiness failure.
3. Extract existing Qt workspace behavior into shared core incrementally. Keep
   native window/input/placement/discovery adapters per OS. First extraction:
   WorkspacePanelCallers; Mac's PanelCallerMenu becomes diagnostic wiring only.
   Port retained sessions, layout binding/rollback, callers and bands through the
   same path; do not create a parallel Windows workspace implementation.
4. Windows integration branch uses shared behavior and preserves existing native
   window snapping/ownership mechanisms. Add the same assembled flow matrix:
   first open, preset protection, native title drag, vacancy reclaim, floating
   recall, inactive tab selection, all caller highlights, Mixer/Keyframes,
   Timeline toolbar, band combinations, maximize, restart, page/project changes,
   Native return and persistent Native gesture. Build on supported Windows MSVC/
   Qt toolchain. Mac compilation of shared code is not Windows build evidence.
5. Replace hardcoded prototype version/platform metadata with one release
   manifest: semantic version, build/revision, source commit, artifact hashes,
   supported OS/architecture/Resolve constraints and read/write schema ranges.
   Keep release artifacts immutable; installation may replace the installed copy.
   Root VERSION currently says 0.1.0 while legacy Mac packaging says 0.0.1;
   reconcile explicitly before assigning the next published version.
6. Harden installer transactions per platform: stage, verify hashes/signatures,
   preflight compatibility, checkpoint exact installed file inventory and relevant
   OpenGate settings, commit replacement, verify installed artifacts, receipt.
   A failed replacement must restore prior binaries or leave an explicit tested
   recovery action. Preserve unrelated plugins and user-created workflow files.
   Use a transaction-specific manifest rather than relying on a global last-path
   text file. Retire obsolete OpenGate commands by prior manifest, not wildcards.
7. Add corresponding uninstall/rollback actions and support-report export.
   Native keyboard assignments need explicit treatment: preserve user bindings
   while removing shipped OpenGate workflow entry points. Never silently delete
   assignments on the assumption that they are installer-owned.
8. Test actual installer scripts against temporary installation roots, including
   no Resolve, running Resolve, unsupported platform, fresh/partial/existing
   profiles, same-version reinstall, upgrade, compatible/incompatible downgrade,
   interrupted install, missing/corrupt payload, uninstall and reinstall. Verify
   user preset bytes preserved where no migration is required, exact migrated
   content where it is, and unrelated files untouched. Expand and inspect actual
   package payloads and compare against manifests.
9. Build the reviewed Mac candidate and `.pkg`, with accurate prototype notes and
   Wolfgang Woehl's MIT attribution. Jordan performs the next installation by
   double-clicking this artifact. Then live-check numbered flow plus uninstall ->
   native Resolve -> reinstall, preserved presets and support-report export.
   Do not replace the accepted live install as a side effect of packaging tests.
10. Produce Windows installer after successful supported-toolchain compilation
    and fixture checks. If external runtime confirmation is unavailable, label
    the prototype Windows runtime unverified. No claim of parity from source
    sharing or a successful compiler alone. Signing/notarization status and any
    resulting OS launch prompts must be accurately described for each artifact.
11. Publish to Jordan's OpenGate GitHub only when separately authorized. Offer
    checksums, manifest, accurate per-platform test evidence, removal/recovery
    instructions and prior release downloads. Preserve Wolfgang's license and
    credit; do not publish to the upstream remote by mistake.

## Current evidence and constraints

Accepted main: e885918; installed helper remains a7ce036. No new install performed.
Menu optimization: 6379004 on perf/panel-menu-refresh; hidden menu, command observer
and assembled first-open fixtures pass. Live performance remains unmeasured.
Windows workspace parity branch starts from that optimization and extracts shared
Panels behavior; hidden menu and assembled first-open/Native-return fixtures pass after the
extraction. It does not yet provide full Windows workspace integration.
This Mac has no supported Windows release toolchain and there is no checked-in
Windows CI workflow. Windows compilation needs an external build environment;
Windows live acceptance needs an external tester. Do independent shared/Mac work
without claiming those gates passed.
Legacy Mac package script already creates a .pkg and preserves existing profiles,
but has hardcoded version/architecture text, stale first-open descriptions, an
outdated receipt contract and incomplete transactional rollback/support reporting.
Legacy Windows packaging produces a ZIP of the older launcher/helper, not the
requested parity installer. Neither currently satisfies this delivery contract.
