# Decizii

- D-1 (2026-10-28, Cristi): fork public redenumit `gordasgdc/flow-davinci`; istoricul upstream păstrat.
- D-2 (revizuit 2026-10-28, Cristi): presetul lui Jordan Tetewsky RĂMÂNE, cu atribuire corectă; nu se exclude fără motiv concret explicat în prealabil.
- D-3: localizare printr-un catalog central RO (textul original = cheie), nu înlocuire directă de literale, pentru sincronizare upstream ușoară.
- D-4: identificatorii interni (Unbound*, chei persistente, căi de log) se păstrează pentru compatibilitate; excepțiile se documentează în doc/flow/.
- D-5: sincronizare doar prin merge controlat din remote-urile `upstream` (OpenGate) și `unbound`; fără descărcare/executare automată de cod upstream la utilizatori.
- Atribuire: fără nume Claude în commit-uri/materiale; istoricul moștenit verificat — 0 mențiuni Claude/Co-Authored-By în OpenGate și Unbound.
- D-6 (2026-10-28, Cristi): identitate proprie dev.gordas, căi distincte de OpenGate/Unbound; coexistența instalărilor și încărcarea simultană se tratează separat; protecție + alegere reversibilă a produsului activ; OpenGate/Unbound nu se șterg/dezactivează automat.
