# Flow DaVinci — STATUS

Actualizat: 2026-10-28. Adaptare în limba română a OpenGate (MIT), bazat pe Unbound (MIT). Inițiator și responsabil de adaptare: Cristi Gordas (GDC).

## Bază
- OpenGate `main` @ eeb884abd04e9a7096feec3c5e33e097c849366c (VERSION 0.0.3). Unbound `master` @ 4ea56989db06194cdcbedd8bc74a31ee74414129 (remote `unbound`).
- Depozit: https://github.com/gordasgdc/flow-davinci (public, fork redenumit). Ramură de lucru: `flow-davinci-ro` (pusă). Release: https://github.com/gordasgdc/flow-davinci/releases/tag/flow-v0.1.0 (tag `v0.1.0` din upstream e ocupat; folosit `flow-v0.1.0`). VERSION proprie: 0.1.0 (test).

## Stare (2026-10-28)
- Identitate proprie `dev.gordas.flowdavinci`, căi distincte (Application Support/GDC/FlowDaVinci, Library/Logs/FlowDaVinci, FlowDaVinci.dvcp.bundle), UUID de plugin propriu. Detalii: doc/flow/IDENTITY.md.
- Localizare RO prin catalog central (src/core/flowlocale*.h), instalator/dezinstalare/suport în română. Windows: doar nume, RO, identitate; netestat pe Windows real, necompilat aici.
- Coexistență: instalarea alături de OpenGate/Unbound e posibilă; încărcarea simultană în aceeași instanță Resolve NU. Protecție doar într-un sens: dacă OpenGate/Unbound e deja încărcat, Flow nu pornește (mesaj RO). Dacă OpenGate se încarcă DUPĂ Flow, Flow NU cedează (nu există mecanism) — soluția sigură este alegerea exclusivă înainte de pornirea Resolve: `Alege produsul activ.command` (reversibil).
- Nume comenzi Workflow: numele afișat în Resolve = numele fișierului `Flow DaVinci - <etichetă>.py`; nu se poate separa de identificator prin API. Excepție documentată; de urmărit în verificarea live.
- Matrice ascunsă (commit 7bf3a8a67a): 55 PASS, 2 FAIL din așteptări de test depășite de identitate (nu din implementare): settings-profile (calea `/Unbound/` → `/Flow DaVinci/`) și flow-locale (rulat cu LANG=en; testul își dezactivează acum variabila). Corectate; matricea se reia pe commit-ul nou.
- Semnare: identități Developer ID Application + Installer (Team 8AR6XP8MG7) și profil notary `gdc-notary` prezente; fără blocaj.
- Nepachetat, neinstalat în Resolve, nepublicat.

## Lot curent (2026-10-28): Despre + actualizări + pagină + Windows CI
- Implementat: `Despre Flow DaVinci…`, `Verifică actualizările…` (meniu Resolve și aplicație), helper `flow-update` (manifest semnat, descărcare ghidată), versiune/build dintr-un singur `release-manifest.json`, identitate Flow DaVinci peste tot (doc/flow/UPDATES.md). Teste noi: test-update-check.py, test-about.sh (în matrice). Cheie de release creată în Certificates/ (de salvat cu backup-ul cheilor de produs).
- Windows CI (GitHub Actions, ramura flow-davinci-ro): prima rulare a picat la auditul de dependențe (bootstrap se numea OpenGate.dvcp în CMake, acum FlowDaVinci.dvcp — corectat); de rulat din nou după push. Fără verificare pe Windows real.
- Pagina https://gordas.dev/flow-davinci: sursa în /Volumes/CristiGordas/TESTE/GDC_PROJECTS/FLOW_DAVINCI/site (release.json = sursa unică, build_page.py, logo/*.svg); worktree site: /Volumes/CristiGordas/TESTE/GDC_PROJECTS/FLOW_DAVINCI/site-worktree. Nepublicată.

## Următoarea acțiune exactă
1. Commit → rebuild runtime curat → `verify-integration.py --hidden-only` (receipt PASS) → pachet semnat/notarizat (`--sign --notary-profile gdc-notary`).
2. Întreabă Cristi dacă Resolve e liber → verificare live (context separat).
3. Pachet în site (downloads/), manifest semnat, pagină, push site, verificare URL publice; release.

## Publicat (2026-10-28) — 0.1.0 build 625, commit a0a6240
- Pagină: https://gordas.dev/flow-davinci (sursă: /Volumes/CristiGordas/TESTE/GDC_PROJECTS/FLOW_DAVINCI/site, publicat în gdc-plugin-manager docs/flow-davinci, commit bf303cb). Manifest semnat verificat live (tools/flow/verify_published_update.py): https://gordas.dev/flow-davinci/update-manifest.json.
- macOS: pkg semnat Developer ID, notarizat, stapled, Gatekeeper OK; SHA-256 29baaf5a68b90f3fb55e46730f78d1e47672dedb5a0c7518e9109790aa03c277. Poarta live în Resolve 21.1.2 (proiect FLOW_DAVINCI_GATE, păstrat): meniu RO, Despre (0.1.0 · build 625), verificare actualizări (rezultat „nu a reușit” înainte de publicare), salvare/restaurare aranjament, revenire la interfața originală — fără erori în jurnal. Găsite și reparate la poarta live: calea versiunii instalate (Current/), texte de aranjamente netraduse.
- Windows: CI verde (windows-2022); instalator NESEMNAT, SHA-256 942e69188cc9f716b5d158ea190aa96ee0d38c32ee3ee3e00f9c8ca587f5da96; neverificat în Resolve pe Windows real; fără canal de actualizare.
- Limite: protecția de coexistență e într-un sens; butoanele OK/Cancel din dialoguri sunt Qt implicite (EN); numele comenzilor Workflow = numele fișierului; markerul de crash scrie încă componenta „OpenGate” (doar diagnostic); logo-ul neaprobat vizual de proprietar; matricea are un test intermitent (startup-readiness), trecut 3/3 la rerulare; aranjamentul „Test Flow” rămas în profilul testat.
- Verificare finală actualizări (2026-10-28, după publicarea manifestului): din aplicația instalată `/Applications/Flow DaVinci.app`, butonul «Verifică actualizările...» → «Flow DaVinci este la zi.» (0.1.0 build 625 = manifest). Rezultatul «nu a reușit» din poarta Resolve era anterior publicării. Resolve nu era pornit; nicio altă sesiune întreruptă.
- Aprobare vizuală: pagina a fost revăzută în browser (telefon) și logo-ul trimis la 16–128 px (/Volumes/CristiGordas/TESTE/GDC_PROJECTS/FLOW_DAVINCI/site/preview/logo-preview.html); APROBAT de proprietar (D-16) — punct închis.
