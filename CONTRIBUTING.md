# Contribuții la Flow DaVinci

Mulțumim pentru ajutor! Reguli scurte:

1. **Licență:** contribuțiile sunt sub licența MIT a proiectului. Nu copia cod sub altă licență.
2. **Localizare:** textele vizibile se scriu cu `FD("text englezesc")` (vezi `src/core/flowlocale.h`) și se adaugă traducerea în `src/core/flowlocale_ro.h`. Rulează `python3 tools/flow/check_catalog.py`.
3. **Separare:** păstrează branding/traducerea separate de schimbările funcționale (commit-uri distincte), ca sincronizarea cu OpenGate să rămână simplă.
4. **Identificatori interni:** nu redenumi global `Unbound*`/`OpenGate*` (vezi [doc/flow/IDENTITY.md](doc/flow/IDENTITY.md)).
5. **Teste:** rulează testele afectate din `infrastructure/macos/test-*.sh|py` cu `FLOW_DAVINCI_LANG=en` plus `infrastructure/macos/test-flow-locale.sh`.
6. **Windows:** menționează explicit dacă schimbarea nu a fost verificată pe Windows real.
7. **Atribuire:** commit-urile poartă autorul real; fără atribuiri automate de instrumente.
8. **Artefacte generate:** nu intră în git (build, instalatoare, loguri).
