#!/bin/bash
set -eu
root="$(cd "$(dirname "$0")/../.." && pwd)"
source "$root/infrastructure/macos/build-target.sh"
sdk="${OPENFX_SDK:-/Library/Application Support/Blackmagic Design/DaVinci Resolve/Developer/OpenFX/OpenFX-1.4/include}"
out="$root/build/macos/product-conflict"; rm -rf "$out"; mkdir -p "$out/Unbound.dvcp.bundle/Contents/MacOS"
echo 'int fixture_symbol(void){return 1;}' > "$out/fixture.c"
xcrun clang "${opengate_macos_flags[@]}" -dynamiclib "$out/fixture.c" -o "$out/Unbound.dvcp.bundle/Contents/MacOS/Unbound.dvcp"
xcrun clang "${opengate_macos_flags[@]}" -Wall -Wno-unused-function -framework CoreFoundation -I "$sdk" -I "$root/tests/platforms/macos" \
    "$root/tests/platforms/macos/product-conflict-tests.c" -o "$out/tests"
"$out/tests" "$out/Unbound.dvcp.bundle/Contents/MacOS/Unbound.dvcp"
