# Flow DaVinci — STATUS

Actualizat: 2026-10-28. Adaptare în limba română a OpenGate (MIT), bazat pe Unbound (MIT).

## Bază
- OpenGate `main` @ eeb884abd04e9a7096feec3c5e33e097c849366c (VERSION 0.0.3, tag v0.0.3-prototype.1 + 2).
- Unbound `main` @ 4ea56989db06194cdcbedd8bc74a31ee74414129 (urmărit prin remote `unbound`).
- Depozit: https://github.com/gordasgdc/flow-davinci (fork public al OpenGate, redenumit). Ramură de lucru: `flow-davinci-ro`.

## Stare
- Inventar făcut; fork creat; nimic din cod modificat; nimic instalat în Resolve; nimic publicat (release).
- Fără instalare existentă OpenGate/Unbound pe acest Mac (verificat). Qt 5.15.2 și Inno Setup absente local.

## Următoarea acțiune exactă
1. Antete Qt 5.15.2 pe extern (rețeta din tests/platforms/macos/README.md, în /Volumes/CristiGordas/TESTE/GDC_PROJECTS/FLOW_DAVINCI/builds).
2. Catalog central de traduceri RO + puncte de apel minime (vezi DECISIONS.md D-1..D-3).
3. Build macOS pe extern, teste țintite, apoi coordonare cu Cristi pentru instalare în Resolve.
