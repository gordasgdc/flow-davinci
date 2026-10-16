# macOS infrastructure

Reserved for native build, icon generation, dependency auditing, signing and
notarization workflows. No macOS release command is implemented yet.
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
