# Integrarea actualizărilor OpenGate (principal) și Unbound (secundar)

Principiu: **OpenGate este baza**. Unbound aduce doar schimbări relevante și compatibile, după verificare. Nimic nu se descarcă și nu se execută automat pe calculatoarele utilizatorilor; sincronizarea se face doar în depozitul de dezvoltare.

Branding și traducere sunt **derivate reproductibil** din trei piese separate de schimbările funcționale:
`tools/flow/apply_identity.py` (identificatori/căi), `tools/flow/wrap_ui_text.py` (învelește textele UI în `FD("…")`), `src/core/flowlocale_ro.h` (traducerile). După orice merge se re-rulează; sunt idempotente.

## OpenGate (principal)
1. `tools/flow/sync_upstream.sh status` — commit-uri noi și fișiere cu risc de conflict.
2. `tools/flow/sync_upstream.sh merge-opengate` — creează `sync/opengate-AAAALLZZ`, face merge fără commit, re-aplică identitatea și învelirea textelor, verifică catalogul.
3. Conflicte: pentru liniile modificate de ambele părți ia varianta upstream, apoi re-rulează `apply_identity.py --apply` și `wrap_ui_text.py --apply`; acestea refac adaptarea Flow DaVinci. Adaugă traducerile care lipsesc (`check_catalog.py` le listează).
4. Verificare țintită: `FLOW_DAVINCI_LANG=en bash infrastructure/macos/test-*.sh` (+ `test-*.py`) pentru zonele atinse, `test-flow-locale.sh`, `test-product-conflict.sh`, `test-active-product.sh`. Apoi o verificare consolidată a candidatului.
5. Actualizează `doc/flow/SOURCES.md` (commit-ul preluat) și `STATUS.md`; commit separat pentru merge, separat pentru adaptare.

## Unbound (secundar)
1. `tools/flow/sync_upstream.sh review-unbound` — commit-urile Unbound absente din istoricul nostru (ramura implicită este `master`).
2. Citește fiecare schimbare; alege doar ce e compatibil cu OpenGate/Flow DaVinci; `git cherry-pick -x <sha>` (păstrează proveniența), nu merge orb.
3. Re-aplică adaptarea și verifică la fel ca mai sus. Nu suprascrie adaptările OpenGate.

## Reguli
- Păstrează licența MIT și notificările de copyright moștenite.
- Nu redenumi global identificatori interni (vezi `IDENTITY.md`).
- Dacă upstream schimbă formatul datelor, schema sau numele comenzilor, oprește-te și decide explicit compatibilitatea.
