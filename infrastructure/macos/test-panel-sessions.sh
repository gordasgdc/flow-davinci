#!/bin/bash
set -eu
root="$(cd "$(dirname "$0")/../.." && pwd)"
qt="$root/build/macos/qtbase-source"
generated="$root/build/macos/qt-generated"
app="${1:-/Applications/DaVinci Resolve/DaVinci Resolve.app}"
out="$root/build/macos/${UNBOUND_TEST_NAME:-panel-session-tests}"
source_file="${UNBOUND_TEST_SOURCE:-$root/tests/platforms/macos/panel-session-tests.cpp}"
xcrun clang++ -std=c++17 -Wall -Wextra -Werror \
    -I "$generated" -I "$qt/include" -I "$qt/include/QtCore" -I "$qt/include/QtGui" -I "$qt/include/QtWidgets" \
    -I "$root/src/platforms/macos" "$source_file" "$root/src/platforms/macos/nativechrome.mm" \
    -L "$app/Contents/Frameworks" -lQt5Widgets.5 -lQt5Gui.5 -lQt5Core.5 \
    -framework AppKit -Wl,-rpath,"$app/Contents/Frameworks" -o "$out"
QT_PLUGIN_PATH="$app/Contents/PlugIns" "$out"
