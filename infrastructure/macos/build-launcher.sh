#!/bin/bash
set -eu
root="$(cd "$(dirname "$0")/../.." && pwd)"
out="$root/build/macos/OpenGate.app/Contents"
mkdir -p "$out/MacOS"
xcrun swiftc -O "$root/src/platforms/macos/launcher.swift" -o "$out/MacOS/OpenGate"
cat > "$out/Info.plist" <<'PLIST'
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0"><dict>
<key>CFBundleExecutable</key><string>OpenGate</string>
<key>CFBundleIdentifier</key><string>com.cinewaltz.opengate</string>
<key>CFBundleName</key><string>OpenGate</string>
<key>CFBundlePackageType</key><string>APPL</string>
<key>CFBundleShortVersionString</key><string>0.1.0</string>
<key>CFBundleVersion</key><string>1</string>
<key>NSHighResolutionCapable</key><true/>
</dict></plist>
PLIST
/usr/bin/codesign --force --sign - "$root/build/macos/OpenGate.app"
testdir="$(mktemp -d "$root/build/macos/launcher-test.XXXXXX")"
UNBOUND_LAUNCH_DIRECTORY="$testdir" "$out/MacOS/OpenGate" --self-test
mkdir -p "$testdir/workflow-home"
HOME="$testdir/workflow-home" python3 "$root/src/platforms/macos/OpenGate.py"
test -s "$testdir/workflow-home/Library/Logs/Unbound/show-controls.request"
echo 'PASS: Workflow Integration launch request'
