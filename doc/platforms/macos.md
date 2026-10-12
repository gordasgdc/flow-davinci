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
