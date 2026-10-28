#!/bin/bash
set -eu
root="$(cd "$(dirname "$0")/../.." && pwd)"
tool="$root/infrastructure/macos/installer/choose-active-product.command"
fixture="$root/build/macos/active-product-test"; rm -rf "$fixture"
plugins="$fixture/Library/Application Support/Blackmagic Design/DaVinci Resolve/IOPlugins"
mkdir -p "$plugins/Unbound.dvcp.bundle" "$plugins/FlowDaVinci.dvcp.bundle"; echo marker > "$plugins/Unbound.dvcp.bundle/marker"
run() { OPENGATE_TEST_ROOT="$fixture" bash "$tool" "$1" >/dev/null; }
run other; [[ -d "$plugins/Unbound.dvcp.bundle" && ! -e "$plugins/FlowDaVinci.dvcp.bundle" ]] || { echo 'FAIL: other not active'; exit 1; }
run flow;  [[ -f "$fixture/Library/Application Support/GDC/FlowDaVinci/Inactive/Unbound.dvcp.bundle/marker" && -d "$plugins/FlowDaVinci.dvcp.bundle" && ! -e "$plugins/Unbound.dvcp.bundle" ]] || { echo 'FAIL: flow not active'; exit 1; }
run other; [[ -f "$plugins/Unbound.dvcp.bundle/marker" && ! -e "$plugins/FlowDaVinci.dvcp.bundle" ]] || { echo 'FAIL: not restored'; exit 1; }
mkdir -p "$plugins/FlowDaVinci.dvcp.bundle"; mkdir -p "$fixture/Library/Application Support/GDC/FlowDaVinci/Inactive/FlowDaVinci.dvcp.bundle"
if OPENGATE_TEST_ROOT="$fixture" bash "$tool" other >/dev/null 2>&1; then echo 'FAIL: overwrote existing'; exit 1; fi
[[ -f "$plugins/Unbound.dvcp.bundle/marker" ]] || { echo 'FAIL: data lost'; exit 1; }
echo 'PASS: alegere produs activ (reversibila, fara stergere, fara suprascriere)'
