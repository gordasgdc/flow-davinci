#!/bin/bash
# SPDX-License-Identifier: MIT
# Shared by install, rollback and uninstall. Settings are never a removal target.
set -eu
physical() { printf '%s%s' "${OPENGATE_TEST_ROOT:-}" "$1"; }
product=$(physical '/Library/Application Support/GDC/FlowDaVinci')
mkdir -p "$product/Transactions"
log() { printf '%s %s\n' "$(/bin/date -u +%Y-%m-%dT%H:%M:%SZ)" "$*" >> "$transaction/events.log"; }
fail() { log "failed $*"; echo "Flow DaVinci: $*. Înregistrare de diagnostic: $transaction/events.log" >&2; exit 1; }
new_transaction() {
 transaction="$product/Transactions/$(/usr/bin/uuidgen)"
 mkdir -m 755 "$transaction"
}
version_relation() {
 local -a installed offered
 local i left right
 IFS=. read -r -a installed <<< "$1"
 IFS=. read -r -a offered <<< "$2"
 for i in 0 1 2; do
  left=$((10#${installed[$i]})); right=$((10#${offered[$i]}))
  if [[ "$right" -gt "$left" ]]; then echo upgrade; return; fi
  if [[ "$right" -lt "$left" ]]; then echo downgrade; return; fi
 done
 echo reinstall
}
validate_path() {
 case "$1" in
  '/Applications/Flow DaVinci Tools/Exporta suport Flow DaVinci.command'|'/Applications/Flow DaVinci Tools/Dezinstaleaza Flow DaVinci.command'|'/Applications/Flow DaVinci Tools/Alege produsul activ.command'|'/Applications/Flow DaVinci.app'|'/Library/Application Support/GDC/FlowDaVinci/Current'|'/Library/Application Support/Blackmagic Design/DaVinci Resolve/IOPlugins/FlowDaVinci.dvcp.bundle') ;;
  '/Library/Application Support/Blackmagic Design/DaVinci Resolve/Workflow Integration Plugins/'FlowDaVinci*.py|'/Library/Application Support/Blackmagic Design/DaVinci Resolve/Workflow Integration Plugins/'Flow\ DaVinci\ -\ *.py) ;;
  '/Library/Application Support/Blackmagic Design/DaVinci Resolve/IOPlugins/'FlowDaVinci-retired-*.backup)
   [[ "${1##*/}" =~ ^FlowDaVinci-retired-[0-9]{8}T[0-9]{6}Z\.backup$ ]] || fail 'Identitate nevalidă a încărcătorului retras' ;;
  *) fail 'Inventarul conține o cale în afara fișierelor gestionate de Flow DaVinci' ;;
 esac
 [[ "$1" != *'/../'* ]] || fail 'Inventar de pachet nevalid'
}
checkpoint() {
 local inventory=$1 index=0 path target
 cp "$inventory" "$transaction/paths.txt"
 mkdir "$transaction/files"
 : > "$transaction/present.txt"
 while IFS= read -r path; do
  [[ -n "$path" && "$path" == /* && "$path" != *'/../'* ]] || fail 'Inventar de pachet nevalid'
  validate_path "$path"
  target=$(physical "$path")
  if [[ -e "$target" || -L "$target" ]]; then
   /usr/bin/ditto "$target" "$transaction/files/$index"
   printf '%s\n' "$index" >> "$transaction/present.txt"
  fi
  index=$((index+1))
 done < "$inventory"
 log 'checkpoint complete'
}
restore_checkpoint() {
 local index=0 path target
 while IFS= read -r path; do
  validate_path "$path"
  target=$(physical "$path")
  # Inventory comes from this package, never from user settings or a wildcard.
  /bin/rm -rf "$target"
  if /usr/bin/grep -qx "$index" "$transaction/present.txt"; then
   /bin/mkdir -p "$(/usr/bin/dirname "$target")"
   /usr/bin/ditto "$transaction/files/$index" "$target"
  fi
  index=$((index+1))
 done < "$transaction/paths.txt"
 log 'rollback complete'
}
