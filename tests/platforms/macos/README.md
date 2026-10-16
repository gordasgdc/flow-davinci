# macOS native tests

Reserved for the loading probe, Cocoa window tests and the adapter for shared pane
fixtures. A native backend and validated loading mechanism are prerequisites.
See [platform status](../../../doc/platforms/macos.md).

Build and self-test the startup helper without Resolve:

```sh
bash infrastructure/macos/build-loading-probe.sh
```

The helper writes a PID marker when loaded. It has no Qt dependency and does not
change panels. A passing host test does not prove that hardened Resolve will load it.
After quitting Resolve normally, the optional live experiment is:

```sh
bash infrastructure/macos/run-loading-probe.sh
```

This starts the installed executable with a process-local library-loading request.
It does not modify or re-sign the app. Check the printed marker file for evidence;
an empty file is not success. The window backend is still unimplemented.

## OpenFX discovery probe

The installed Resolve OpenFX SDK provides a native plugin entry point. Build a
discovery-only bundle with:

```sh
bash infrastructure/macos/build-ofx-probe.sh
```

The build checks the two exported discovery functions in an isolated test host.
Install `build/macos/ofx-probe/UnboundLoadingProbe.ofx.bundle` under
`/Library/OFX/Plugins/`, then start Resolve normally. The probe advertises zero
effects and writes process identity, main-thread status and Qt singleton presence
to `~/Library/Logs/Unbound/ofx-loading-probe.log`. No Qt functions are called and
no widgets are changed by discovery. When Qt is available, the probe retains its
library and queues a read-only active-window check on the main dispatch queue.
A scanner process entry alone does not prove access to Resolve's UI. Remove this
bundle after quitting Resolve to uninstall the diagnostic.

## Widget inventory

Prepare matching development headers once (no Qt runtime build or installation):

```sh
git clone --depth 1 --branch v5.15.2 https://github.com/qt/qtbase.git build/macos/qtbase-source
cd build/macos/qtbase-source
./configure -opensource -confirm-license -nomake examples -nomake tests -no-openssl -no-dbus -no-opengl
perl bin/syncqt.pl -version 5.15.2
cd ../../..
bash infrastructure/macos/build-widget-probe.sh
```

Reinstall the complete OpenFX probe bundle with Resolve closed. The optional
widget inspector loads only in the Qt host on the main queue and links to the
installed Resolve Qt libraries. It requires runtime Qt 5.15.2. Three snapshots,
after 2, 10 and 30 seconds, record widget names, classes, parent names, visibility
and geometry under `~/Library/Logs/Unbound/widgets-{1,2,3}.json`. Open a project
during this interval to include its panels. These are local diagnostic files;
review them before sharing. Snapshot capture does not modify widgets.

The experimental control window also offers separate Undock/Dock buttons for
Inspector, Timeline, Source Monitor and Program Monitor. Open the Edit page and
enable dual viewers before detaching Source Monitor. Panels move only when the
corresponding button is clicked; closing a host docks its panel. Playback and
native viewer rendering still need validation in Resolve.

Run native session fixtures after preparing the headers above:

```sh
bash infrastructure/macos/test-panel-sessions.sh
```

These create temporary Qt test windows using the installed runtime. They verify
layout slots, stretch/alignment, grid spans, splitter order/sizes, 30 repeated
cycles, close-to-dock, page visibility, independent three-panel operation,
unsupported/hidden target rejection and restoration on session destruction.
