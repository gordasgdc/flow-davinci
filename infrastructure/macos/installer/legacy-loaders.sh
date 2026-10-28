#!/bin/bash
# SPDX-License-Identifier: MIT
# Retired Flow DaVinci loaders (never OpenGate or Unbound) must be backed up outside Resolve's plugin search tree.
legacy_loaders() {
 local folder name metadata product_name
 local directory='/Library/Application Support/Blackmagic Design/DaVinci Resolve/IOPlugins'
 for folder in "$(physical "$directory")"/FlowDaVinci-retired-*.backup; do
  [[ -d "$folder" ]] || continue
  name=${folder##*/}
  [[ "$name" =~ ^FlowDaVinci-retired-[0-9]{8}T[0-9]{6}Z\.backup$ ]] || continue
  metadata="$folder/Contents/Resources/build-manifest.json"
  [[ -f "$metadata" && -f "$folder/Contents/MacOS/FlowDaVinci.dvcp" && -f "$folder/Contents/MacOS/FlowDaVinciWorkspace.dylib" ]] || continue
  product_name=$(/usr/bin/plutil -extract product raw -o - "$metadata" 2>/dev/null) || continue
  [[ "$product_name" == "Flow DaVinci" ]] || continue
  printf '%s/%s\n' "$directory" "$name"
 done
}
