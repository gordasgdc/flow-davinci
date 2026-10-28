#!/bin/bash
set -eu
root="$(cd "$(dirname "$0")/../.." && pwd)"
UNBOUND_TEST_HEADLESS=1 UNBOUND_TEST_NAME=flowlocale-tests UNBOUND_TEST_SOURCE="$root/tests/core/flowlocale/flowlocale-tests.cpp" \
    bash "$root/infrastructure/macos/test-panel-sessions.sh" "$@"
