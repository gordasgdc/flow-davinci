#!/bin/bash
set -eu
root="$(cd "$(dirname "$0")/../.." && pwd)"
sdk='/Library/Application Support/Blackmagic Design/DaVinci Resolve/Developer'
out="$root/build/macos/io-helper"
bundle="$out/Unbound.dvcp.bundle/Contents/MacOS"
mkdir -p "$bundle" "$out/test-home/Library/Logs/Unbound"
xcrun clang -Wall -Wextra -Werror -fvisibility=hidden -c \
    -I "$sdk/OpenFX/OpenFX-1.4/include" "$root/tests/platforms/macos/ofx-loading-probe.c" -o "$out/bootstrap.o"
xcrun clang++ -std=c++17 -Wall -Wextra -Werror -fvisibility=hidden -dynamiclib \
    -I "$sdk/CodecPlugin/Examples/x264_encoder_plugin/include" \
    "$root/src/platforms/macos/io-bootstrap.cpp" "$out/bootstrap.o" -o "$bundle/Unbound.dvcp"
cp "$root/build/macos/qt-generated/build-manifest.json" "$bundle/../build-manifest.json"
cp "$root/build/macos/ofx-probe/UnboundLoadingProbe.ofx.bundle/Contents/MacOS/UnboundWidgetProbe.dylib" "$bundle/"
/usr/bin/codesign --force --sign - "$bundle/Unbound.dvcp"
xcrun clang++ -std=c++17 -Wall -Wextra -Werror \
    -I "$sdk/CodecPlugin/Examples/x264_encoder_plugin/include" \
    "$root/tests/platforms/macos/io-bootstrap-tests.cpp" -o "$out/bootstrap-tests"
HOME="$out/test-home" "$out/bootstrap-tests" "$bundle/Unbound.dvcp"
