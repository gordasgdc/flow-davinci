# Decizii

- D-1 (2026-10-28, Cristi): fork public redenumit `gordasgdc/flow-davinci`; istoricul upstream păstrat.
- D-2 (revizuit 2026-10-28, Cristi): presetul lui Jordan Tetewsky RĂMÂNE, cu atribuire corectă; nu se exclude fără motiv concret explicat în prealabil.
- D-3: localizare printr-un catalog central RO (textul original = cheie), nu înlocuire directă de literale, pentru sincronizare upstream ușoară.
- D-4: identificatorii interni (Unbound*, chei persistente, căi de log) se păstrează pentru compatibilitate; excepțiile se documentează în doc/flow/.
- D-5: sincronizare doar prin merge controlat din remote-urile `upstream` (OpenGate) și `unbound`; fără descărcare/executare automată de cod upstream la utilizatori.
- Atribuire: fără nume Claude în commit-uri/materiale; istoricul moștenit verificat — 0 mențiuni Claude/Co-Authored-By în OpenGate și Unbound.
- D-6 (2026-10-28, Cristi): identitate proprie dev.gordas, căi distincte de OpenGate/Unbound; coexistența instalărilor și încărcarea simultană se tratează separat; protecție + alegere reversibilă a produsului activ; OpenGate/Unbound nu se șterg/dezactivează automat.
- D-7 (2026-10-28): testele upstream nu se modifică pentru PASS; doar așteptările depășite de identitatea intenționată (calea de profil) și testul propriu flow-locale (își dezactivează FLOW_DAVINCI_LANG).
- D-8 (2026-10-28): la ordinea inversă de încărcare nu există „cedare”; limita se documentează, iar garanția este alegerea exclusivă înainte de pornirea Resolve. Detecția la încărcare târzie = funcție nouă, doar cu acordul lui Cristi.
- D-9 (2026-10-28): numele comenzilor Workflow = numele fișierului (Resolve nu separă afișarea de identificator); excepție documentată.
- D-10 (2026-10-28, Cristi): distribuția macOS = pachet semnat Developer ID, notarizat și cu stapling; ordinea: verificare candidat → împachetare/semnare → notarizare/stapling → spctl → publicarea exactă a artefactului verificat. Formulare publică: „Semnat și notarizat de Apple” doar după verificare; notarizarea nu e certificare a funcționalității.
- D-11 (2026-10-28, Cristi): pagină publică https://gordas.dev/flow-davinci pe procedura GitHub Pages existentă (depozit `gordasgdc/gdc-plugin-manager`, folder `docs/`, ca `foto-asist-gc`); stare „Versiune pentru testare”; buton Windows dezactivat „Windows — în pregătire” cât timp nu există instalator verificat; logo SVG original, fără raster.
