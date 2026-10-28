#!/bin/bash
# SPDX-License-Identifier: MIT
set -eu
scripts="$(cd "$(dirname "$0")" && pwd)"
if [[ -z "${OPENGATE_TEST_ROOT:-}" ]]; then
 if /usr/bin/pgrep -x Resolve >/dev/null; then echo 'Salvează și închide Resolve înainte de dezinstalare.'; exit 1; else code=$?; [[ "$code" == 1 ]] || exit 1; fi
 if [[ "$EUID" -ne 0 ]]; then exec /usr/bin/sudo /bin/bash "$0"; fi
fi
source "$scripts/transaction.sh"
new_transaction
if ! mkdir "$product/install.lock" 2>/dev/null; then fail 'O instalare sau recuperare este în curs; finalizeaz-o înainte de dezinstalare'; fi
trap 'rmdir "$product/install.lock" 2>/dev/null || true' EXIT
checkpoint "$scripts/paths.txt"
cp "$scripts/transaction.sh" "$transaction/transaction.sh"
cp "$scripts/rollback.command" "$transaction/Restaureaza versiunea anterioara Flow DaVinci.command"
chmod 755 "$transaction/Restaureaza versiunea anterioara Flow DaVinci.command"
while IFS= read -r path; do /bin/rm -rf "$(physical "$path")"; done < "$transaction/paths.txt"
log 'uninstalled; setările Flow DaVinci și datele native Resolve au fost păstrate'
/bin/rmdir "$product/install.lock"
trap - EXIT
echo 'Flow DaVinci a fost eliminat. Redeschide Resolve pentru interfața nativă. Presetările tale rămân disponibile la reinstalare.'
echo "Recuperare: $transaction/Restaureaza versiunea anterioara Flow DaVinci.command"
