# macOS infrastructure

Experimental build and test scripts live here. No macOS release command,
notarization workflow or distribution package is implemented yet.
Use `build/macos/` for development output and `release/mac/` for future immutable
releases. See [platform status](../../doc/platforms/macos.md).

Start with a read-only inspection of the installed Resolve bundle:

```sh
bash infrastructure/macos/inspect-resolve.sh
# Optional: pass a different Resolve .app path as the first argument.
```

This reports architectures, linked libraries, signing flags, entitlements and
signature verification. It neither launches nor modifies Resolve. Failed
entitlement decoding does not establish whether helper loading is allowed.

Build the development helper and launchers after preparing the Qt headers:

```sh
bash infrastructure/macos/build-ofx-probe.sh
bash infrastructure/macos/build-widget-probe.sh
bash infrastructure/macos/build-io-helper.sh
bash infrastructure/macos/build-launcher.sh
bash infrastructure/macos/test-panel-sessions.sh
```

The native startup bundle is `build/macos/io-helper/Unbound.dvcp.bundle` and the
launcher is `build/macos/Unbound.app`. Installation and current limitations are
in the platform status document. Resolve must be closed before replacing helpers.

Native-window fixtures refuse to launch while Resolve is running, or when the
process check cannot establish that it is closed. To compile without displaying
test windows, use `UNBOUND_TEST_COMPILE_ONLY=1` with a test script. Only the
QCoreApplication diagnostics, layout-plan and workspace-page fixtures declare themselves
headless; they can run alongside Resolve. Responsiveness reports cover Qt event
turns with simulated input, not Resolve playback or GPU presentation latency.
