# Native first-drop recovery guards

A missing first-drop target must fail before the native page is resized or a whole-page arrangement is created. Revalidate the original native layout slot immediately before acquiring it, including saved-anchor preparation. Keep the floating source intact when validation fails.

Verification: `test-native-workspace-ui.sh` passes, including rejection of an unavailable first-drop target without calling the native mutation hook or moving the source. Existing menu, save/reopen, door, original-slot and undock paths also pass. This does not establish live Resolve recovery acceptance.
