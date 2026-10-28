# Flow DaVinci — STATUS

Actualizat: 2026-10-28. Adaptare în limba română a OpenGate (MIT), bazat pe Unbound (MIT). Inițiator și responsabil de adaptare: Cristi Gordas (GDC).

## Bază
- OpenGate `main` @ eeb884abd04e9a7096feec3c5e33e097c849366c (VERSION 0.0.3). Unbound `master` @ 4ea56989db06194cdcbedd8bc74a31ee74414129 (remote `unbound`).
- Depozit: https://github.com/gordasgdc/flow-davinci (public, fork redenumit). Ramură de lucru: `flow-davinci-ro` (commit-uri locale, nepuse). VERSION proprie: 0.1.0 (test).

## Stare (2026-10-28)
- Identitate proprie `dev.gordas.flowdavinci`, căi distincte (Application Support/GDC/FlowDaVinci, Library/Logs/FlowDaVinci, FlowDaVinci.dvcp.bundle), UUID de plugin propriu. Detalii: doc/flow/IDENTITY.md.
- Localizare RO prin catalog central (src/core/flowlocale*.h), instalator/dezinstalare/suport în română. Windows: doar nume, RO, identitate; netestat pe Windows real, necompilat aici.
- Coexistență: instalarea alături de OpenGate/Unbound e posibilă; încărcarea simultană în aceeași instanță Resolve NU. Protecție doar într-un sens: dacă OpenGate/Unbound e deja încărcat, Flow nu pornește (mesaj RO). Dacă OpenGate se încarcă DUPĂ Flow, Flow NU cedează (nu există mecanism) — soluția sigură este alegerea exclusivă înainte de pornirea Resolve: `Alege produsul activ.command` (reversibil).
- Nume comenzi Workflow: numele afișat în Resolve = numele fișierului `Flow DaVinci - <etichetă>.py`; nu se poate separa de identificator prin API. Excepție documentată; de urmărit în verificarea live.
- Matrice ascunsă (commit 7bf3a8a67a): 55 PASS, 2 FAIL din așteptări de test depășite de identitate (nu din implementare): settings-profile (calea `/Unbound/` → `/Flow DaVinci/`) și flow-locale (rulat cu LANG=en; testul își dezactivează acum variabila). Corectate; matricea se reia pe commit-ul nou.
- Semnare: identități Developer ID Application + Installer (Team 8AR6XP8MG7) și profil notary `gdc-notary` prezente; fără blocaj.
- Nepachetat, neinstalat în Resolve, nepublicat.

## Următoarea acțiune exactă
1. Commit corecții + docs → reia `verify-integration.py --hidden-only` (cu FLOW_DAVINCI_LANG=en) → receipt PASS.
2. Symlink `release` extern → `package-prototype.py --sign --notary-profile gdc-notary` (notarizare + stapling + spctl) + repetiție rollback în sandbox.
3. Întreabă Cristi dacă Resolve e liber → verificare live într-un context separat.
4. Pagina https://gordas.dev/flow-davinci (în depozitul site-ului, worktree curat din origin/main; vezi HANDOFF), logo SVG, apoi push repo + release + verificare URL-uri.
