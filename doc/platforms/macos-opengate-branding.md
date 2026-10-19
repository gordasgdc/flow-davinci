# OpenGate identity

User-facing macOS identity: **OpenGate, by CineWaltz**. No Relay designation. Foundation credit: **Unbound by Wolfgang Woehl (MIT)**. His copyright/license stays unchanged. Manager/shortcut windows, Workspace UI entries and About, launcher/app, Workflow Integration entry and diagnostic component use OpenGate.

Existing C++ namespace, stable widget/command IDs, plugin binary paths, environment variables and saved data/log directories remain unchanged to preserve arrangements and compatibility. Diagnostic module names still identify the actual loaded binary. The new Workflow Integration entry is `OpenGate.py`; the old source shim is compatibility-only, not a second installed menu entry.

The macOS launcher builds as OpenGate.app. Its UI, diagnostics and About credit identify CineWaltz and Wolfgang. The app/request channel retains existing data paths, so current installations do not lose state.

Launcher/request self-tests, hidden menu attribution test and log identity/rotation tests pass. Installation must preserve/remove the old visible launcher/menu entry as a backed-up migration rather than leaving duplicate products.
