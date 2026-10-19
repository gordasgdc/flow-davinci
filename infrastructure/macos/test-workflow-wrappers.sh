#!/bin/bash
set -eu
root="$(cd "$(dirname "$0")/../.." && pwd)"
python3 "$root/tests/platforms/macos/test-workflow-command-wrappers.py"
