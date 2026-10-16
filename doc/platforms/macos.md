# macOS development status

The macOS port is not implemented. Its source belongs in `src/platforms/macos/`,
build and packaging infrastructure in `infrastructure/macos/`, and native tests in
`tests/platforms/macos/`. Root CMake configuration on macOS currently fails with an
explicit status message, before looking for Windows tools or Qt development files.

## Inspected development machine

Read-only inspection on 2026-10-12 found macOS 27.0.1 (26A434), arm64, Resolve
21.1.1 / bundle build 21.1.10010, Xcode with Apple Clang 21 and CMake. Resolve's
QtCore dylib and executable contain arm64 and x86_64 slices; Qt libraries report
5.15.2. No Qt development installation was found in the locations inspected.

Resolve enables hardened runtime. `codesign` reported an invalid entitlements blob,
so effective third-party loading permissions remain unresolved. No loading method
has been validated, and the application was not modified or injected during that
inspection.

## First implementation milestone

### Startup probe on 2026-10-16

The standalone loading helper builds and writes its marker when inserted into
the test host. Resolve launched with the same request, but its marker stayed
empty after startup; helper loading was not demonstrated.

Using `codesign -d --entitlements - --arch arm64` decodes the installed app's
entitlements successfully. The earlier `:-` invocation produced misleading
invalid-blob warnings. Both slices declare `disable-library-validation`, but
neither declares `allow-dyld-environment-variables`. This makes startup environment
filtering a likely explanation, pending further validation. No app modification
or re-signing was performed.

The OpenFX discovery probe loaded during a normal Resolve launch. Its log records
both `OFXLoader` (no Qt singleton) and the actual `Contents/MacOS/Resolve` process
(Qt singleton present). Resolve's callback ran on a worker thread. This proves
native loading and Qt visibility in the unmodified installed application, but
does not establish support for manipulating internal panels through OpenFX.

The second probe successfully queued a read-only active-window check on the main
dispatch queue inside Resolve. Its log confirms `main_thread=1` and a present Qt
application. No active Qt window was reported at that early startup point.
It retains its own library handle so the queued callback survives host unloading.
The bundle advertises zero effects and does not initialize the shared controller.
Panel enumeration succeeded with Qt 5.15.2 on the main thread. The inventory
contains Edit Inspector, separate source/timeline players and the timeline
container. An experimental Qt window session and control window now target
Inspector, Timeline, Source Monitor and Program Monitor individually. Source
Monitor must be visible in dual-viewer mode before detaching its stack.

Native fixture tests pass for 30 detach/restore cycles, box stretch/alignment,
grid spans, splitter order/sizes, independent panels, page visibility, close,
unsupported/hidden targets and session lifetime. These tests caught and resolved
premature placeholder insertion into QSplitter. Actual Resolve playback, viewer
rendering after reparenting and docking behavior still require live validation.
The build refreshes the OFX binary timestamp when the companion changes so the
scanner invalidates its cache. Reliable loading across unchanged cached launches
is still pending; this is a diagnostic development installation, not a release.

### Remaining milestones

1. Establish compatible arm64 Qt development headers/configuration and link to
   Resolve's existing Qt libraries without introducing another Qt runtime.
2. Prove a loading mechanism in an unmodified Resolve installation, then locate its
   existing QApplication and enumerate widgets on its GUI thread.
3. Implement the window integration and placement contracts, and a test adapter.
4. Demonstrate reversible docking of one pane in a fresh Resolve session.
5. Add Cocoa snapping, dark captions, responsive resizing, Option bypass, Retina
   geometry and Spaces/fullscreen handling, then validate playback and persistence.
6. Define independent Mac packaging, dependency auditing, signing and notarization.

Do not treat an empty native implementation or a successful compile as macOS
support. The existing Windows release path must remain independently buildable.
For common boundaries see [architecture](../architecture.md).
