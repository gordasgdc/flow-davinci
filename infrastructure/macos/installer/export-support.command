#!/bin/bash
# SPDX-License-Identifier: MIT
set -eu
resources="$(cd "$(dirname "$0")" && pwd)"
stamp=$(/bin/date -u +%Y%m%dT%H%M%SZ)
output="$HOME/Desktop/FlowDaVinci-Suport-$stamp"
/bin/mkdir -m 700 "$output"
/usr/bin/osascript -l JavaScript "$resources/export-support.js" "$HOME" "$output/events-summary.json"
{
 echo 'Raport de suport Flow DaVinci — fără conținut de proiect/media și fără jurnale brute'
 /usr/bin/sw_vers
 /usr/bin/uname -m
} > "$output/system.txt"
for name in release-manifest.json runtime-build-manifest.json; do
 if [[ -f "$resources/$name" ]]; then /bin/cp "$resources/$name" "$output/$name"; fi
done
/usr/bin/ditto -c -k --keepParent "$output" "$output.zip"
echo "Raport de suport: $output.zip"
echo 'Verifică raportul înainte de a-l trimite. Nimic nu a fost încărcat.'
