#!/bin/bash
set -eu
root="$(cd "$(dirname "$0")/../.." && pwd)"
work="$(mktemp -d "$root/build/macos/about-test.XXXXXX")"
xcrun swiftc -O -D FLOW_UPDATE_TESTING "$root/src/platforms/macos/update.swift" -o "$work/flow-update-test"
printf '{"version":"0.1.0","build_number":42,"build":"abc1234"}' > "$work/release-manifest.json"
export FLOW_DAVINCI_TEST_MANIFEST="$work/release-manifest.json" FLOW_DAVINCI_TEST_UPDATE_HELPER="$work/flow-update-test"
export FLOW_UPDATE_TEST_PUBKEY="AAAA" FLOW_UPDATE_TEST_URL="http://127.0.0.1:1/update-manifest.json" FLOW_UPDATE_TEST_INSTALLED="$work/release-manifest.json" FLOW_UPDATE_TEST_STATE="$work" FLOW_UPDATE_TEST_ARTIFACT_PREFIX="http://127.0.0.1:1/"
UNBOUND_TEST_HEADLESS=1 UNBOUND_TEST_NAME=about-tests UNBOUND_TEST_SOURCE="$root/tests/core/about/about-tests.cpp" \
    bash "$root/infrastructure/macos/test-panel-sessions.sh" "$@"
