#!/bin/bash
# SPDX-License-Identifier: MIT
# Alege produsul activ in Resolve: Flow DaVinci sau OpenGate/Unbound. Reversibil; nu sterge nimic.
# Muta (nu sterge) pachetul celuilalt produs din IOPlugins intr-un dosar inactiv din afara cautarii Resolve.
set -eu
scripts="$(cd "$(dirname "$0")" && pwd)"
physical() { printf '%s%s' "${OPENGATE_TEST_ROOT:-}" "$1"; }
plugins=$(physical '/Library/Application Support/Blackmagic Design/DaVinci Resolve/IOPlugins')
inactive=$(physical '/Library/Application Support/GDC/FlowDaVinci/Inactive')
flow="$plugins/FlowDaVinci.dvcp.bundle"
others=("$plugins/Unbound.dvcp.bundle" "$plugins/OpenGate.dvcp.bundle")
choice=${1:-}
if [[ -z "${OPENGATE_TEST_ROOT:-}" ]]; then
 if /usr/bin/pgrep -x Resolve >/dev/null; then echo 'Salvează și închide Resolve înainte de a schimba produsul activ.'; exit 1; else code=$?; [[ "$code" == 1 ]] || exit 1; fi
 if [[ "$EUID" -ne 0 ]]; then exec /usr/bin/sudo /bin/bash "$0" "$choice"; fi
fi
describe() {
 [[ -d "$flow" ]] && echo '  Flow DaVinci: activ (instalat în Resolve)' || { [[ -d "$inactive/FlowDaVinci.dvcp.bundle" ]] && echo '  Flow DaVinci: dezactivat temporar' || echo '  Flow DaVinci: neinstalat'; }
 for o in "${others[@]}"; do n=${o##*/}
  if [[ -d "$o" ]]; then echo "  ${n%.dvcp.bundle}: activ (instalat în Resolve)"; fi
  if [[ -d "$inactive/$n" ]]; then echo "  ${n%.dvcp.bundle}: dezactivat temporar de Flow DaVinci"; fi
 done
 return 0
}
echo 'Stare curentă:'; describe
if [[ -z "$choice" ]]; then
 echo; echo 'Alege: [1] Flow DaVinci activ   [2] OpenGate/Unbound activ   [altceva] anulează'
 read -r -p '> ' choice
 case "$choice" in 1) choice=flow ;; 2) choice=other ;; *) echo 'Nicio modificare.'; exit 0 ;; esac
fi
log() { /bin/mkdir -p "$inactive"; printf '%s %s\n' "$(/bin/date -u +%Y-%m-%dT%H:%M:%SZ)" "$*" >> "$inactive/alegere.log"; }
move() { # sursa destinatia: nu suprascrie niciodata
 [[ -e "$2" ]] && { echo "Refuz să suprascriu $2. Nicio modificare."; exit 1; }
 /bin/mkdir -p "$(/usr/bin/dirname "$2")"; /bin/mv "$1" "$2"; log "mutat $1 -> $2"
}
case "$choice" in
 flow)
  for o in "${others[@]}"; do [[ -d "$o" ]] && move "$o" "$inactive/${o##*/}"; done
  [[ -d "$inactive/FlowDaVinci.dvcp.bundle" && ! -e "$flow" ]] && move "$inactive/FlowDaVinci.dvcp.bundle" "$flow"
  echo 'Flow DaVinci este produsul activ. OpenGate/Unbound nu au fost șterse; pot fi reactivate cu aceeași unealtă. Pornește Resolve.' ;;
 other)
  [[ -d "$flow" ]] && move "$flow" "$inactive/FlowDaVinci.dvcp.bundle"
  for o in "${others[@]}"; do [[ -d "$inactive/${o##*/}" && ! -e "$o" ]] && move "$inactive/${o##*/}" "$o"; done
  echo 'OpenGate/Unbound sunt produsul activ. Flow DaVinci nu a fost șters; poate fi reactivat cu aceeași unealtă. Pornește Resolve.' ;;
 *) echo 'Opțiune necunoscută. Nicio modificare.'; exit 1 ;;
esac
echo; echo 'Stare nouă:'; describe
