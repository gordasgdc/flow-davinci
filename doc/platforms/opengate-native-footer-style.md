# Native footer appearance

Branch fix/macos-native-footer-style builds on the pinned-header branch. Native main
Page Navigation has no overlay; its real controls and adjacent Home remain untouched.
Custom surfaces use NativePageButton, which paints through each original native
button at its logical size and destination device scale. There is no replacement
icon artwork, red-underlined custom style or scaled screenshot. Each proxy continues
to invoke the existing original button once, so page state remains native.

The copies preserve native label/icon presentation and use horizontal scrolling
when that native presentation does not fit. They do not invent a separate compact
icon style when Resolve does not expose one. Tooltips retain full page names.

Hidden navigation, state synchronization, migration, widths/font change, reachability
and native-ownership tests pass. Actual Resolve visual acceptance is still pending.
No install, merge or publication.
