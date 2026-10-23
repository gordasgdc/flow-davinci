#!/bin/bash
set -eu
root="$(cd "$(dirname "$0")/../.." && pwd)"
QT_MAC_DISABLE_FOREGROUND_APPLICATION_TRANSFORM=1 UNBOUND_TEST_HEADLESS=1 OPENGATE_TEST_NATIVE_EDIT_CONTROLLER=1 \
    UNBOUND_TEST_NAME=native-controller-flow-tests UNBOUND_TEST_SOURCE="$root/tests/platforms/macos/opengate-first-open-tests.cpp" \
    bash "$root/infrastructure/macos/test-panel-sessions.sh" "$@"
