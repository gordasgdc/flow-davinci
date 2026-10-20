# Platform compatibility audit

Upstream reviewed: wolfgangw/unbound master 96c665a, release v0.3.0. Fetched only;
no upstream merge, installed-file replacement or change to Windows behavior.

The installed OpenGate 091268b helper and bootstrap each declare minos 27.0.
Resolve's Qt Core has arm64 minos 11.0 and an Intel slice, so the inherited compiler
default, not a known Qt requirement, produced our macOS 27 binary floor.

This isolated research branch adds one build-target module consumed by the helper,
bootstrap, discovery probe and Swift launcher. It defaults to macOS 11.0 and builds
universal arm64/x86_64 output. Both architectures compile; helper bootstrap and launcher
self-tests pass on this Apple Silicon machine. audit-compatibility.py verifies real
Mach-O targets, architecture sets, launcher plist and signatures. Those checks pass.
No claim of actual older macOS or Intel hardware support follows from compilation.

Public 0.0.1 packaging is intentionally unchanged; it still labels its existing arm64
artifact macOS 27. Do not publish that old packaging script as a new universal release.
The combined new feature candidate needs fresh builds, live acceptance, a compatibility
matrix and revised installer metadata before a new package.

## Selective upstream adoption

- Study infrastructure/macos/architectures.sh and installer/build.sh for universal
  checks, immutable candidates and distribution. Our target module follows the same
  explicit-target principle without replacing panel behavior.
- Upstream src/platforms/macos/loading/lua is a possible Free/Studio startup adapter.
  It must start OpenGate's controller, not upstream's separate controller; installing
  both would risk duplicate hooks and conflicts. Do not copy the loader blindly.
- Upstream src/platform contracts isolate native windows, coordinates, settings path
  and event handling. Adopt adapters after an interface-by-interface comparison.
- Upstream's src/core/controller.cpp now implements its shared catalogue and lifecycle.
  Replacing ours wholesale would discard OpenGate arrangements, commands and bands.
- Windows still needs OpenGate's new features moved behind those contracts, then a
  real Windows build and Resolve test. Existing Windows source is not proof of current
  OpenGate feature parity. No Windows binary was built or advertised here.

## Release gate

Apple Silicon current OS: candidate compilation/self-tests only. Older macOS: live
not tested. Intel: compiled, native hardware not tested. Windows: feature port and
validation pending. Keep these labels separate from deployment targets. No package,
installation, upstream/main merge or publication was performed.
