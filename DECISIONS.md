# Decizii

- D-1 (2026-10-28, Cristi): fork public redenumit `gordasgdc/flow-davinci`; istoricul upstream păstrat.
- D-2: presetul opțional al lui Jordan Tetewsky se scoate din distribuția Flow DaVinci (rămâne în istoric/licență).
- D-3: localizare printr-un catalog central RO (textul original = cheie), nu înlocuire directă de literale, pentru sincronizare upstream ușoară.
- D-4: identificatorii interni (Unbound*, chei persistente, căi de log) se păstrează pentru compatibilitate; excepțiile se documentează în doc/flow/.
- D-5: sincronizare doar prin merge controlat din remote-urile `upstream` (OpenGate) și `unbound`; fără descărcare/executare automată de cod upstream la utilizatori.
- Atribuire: fără nume Claude în commit-uri/materiale; istoricul moștenit verificat — 0 mențiuni Claude/Co-Authored-By în OpenGate și Unbound.
