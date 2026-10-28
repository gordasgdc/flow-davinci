# Flow DaVinci — STATUS

Actualizat: 2026-10-28. Adaptare în limba română a OpenGate (MIT), bazat pe Unbound (MIT).

## Bază
- OpenGate `main` @ eeb884abd04e9a7096feec3c5e33e097c849366c (VERSION 0.0.3, tag v0.0.3-prototype.1 + 2).
- Unbound `main` @ 4ea56989db06194cdcbedd8bc74a31ee74414129 (urmărit prin remote `unbound`).
- Depozit: https://github.com/gordasgdc/flow-davinci (fork public al OpenGate, redenumit). Ramură de lucru: `flow-davinci-ro`.

## Stare (2026-10-28, după lotul de localizare)
- Antete Qt 5.15.2 și build de bază macOS pe extern: /Volumes/CristiGordas/TESTE/GDC_PROJECTS/FLOW_DAVINCI/builds (symlink explicit `build` în repo, ignorat de git).
- Localizare: src/core/flowlocale.h + flowlocale_ro.h (catalog, FD("...") la ~75 de puncte UI). FLOW_DAVINCI_LANG=en dezactivează traducerea (testele upstream compară engleza).
- Verificat: build-io-helper, build-launcher, test-layout-plans, test-diagnostics, test-flow-locale (PASS); fixturile cu ferestre native doar compilate (Resolve era deschis).
- Nepachetat, neinstalat, nepublicat. Neacoperit încă: jurnale/mesaje non-UI, textele scripturilor de instalare/dezinstalare/suport, etichetele integration-features.json, Windows (rc/Inno), README RO.

## Poartă deschisă: identitate și coexistență la instalare
Pachetul actual folosește aceleași căi ca OpenGate (Unbound.dvcp.bundle, OpenGate.py, /Applications/OpenGate*, com.cinewaltz.opengate). Vezi mesajul către Cristi.

## Următoarea acțiune exactă
Decizia de identitate (mai sus) → redenumire pachet/instalator → README RO → build candidat + rollback → coordonare instalare în Resolve.
