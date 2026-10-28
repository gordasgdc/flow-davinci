# Flow DaVinci

Flow DaVinci oferă DaVinci Resolve panouri mobile, grupuri împărțite/pe file și spații de lucru salvate, păstrând conținutul nativ real al Resolve. Este **gratuit**, cu **surse publice** și **deschis contribuțiilor**.

Este o **adaptare în limba română** a [OpenGate](https://github.com/jordantetewsky/OpenGate) (CineWaltz, licență MIT), care la rândul său se bazează pe [Unbound](https://github.com/wolfgangw/Unbound) de Wolfgang Woehl (licență MIT). Adaptarea Flow DaVinci este inițiată și susținută de Cristi Gordas (GDC). Nu este afiliat cu Blackmagic Design.

> **Stare: prototip.** macOS: construit și verificat parțial pe Mac (vezi „Verificare”). **Windows: sursă adaptată (nume, traducere, identitate), netestat pe Windows real.**

## Ce este tradus și ce nu
- **Tradus:** textele proprii ale Flow DaVinci — meniuri, comenzi, dialoguri, tooltip-uri, mesaje de stare și de eroare, instalator, dezinstalare, export de suport, mesaje de coexistență.
- **Netradus (controlat de Resolve):** interfața nativă DaVinci Resolve și numele comenzilor din *Keyboard Customization → Workspace → Workflow Integrations*, care rămân `Flow DaVinci - <nume englezesc>` (identificatori stabili pentru scurtăturile salvate).
- Setarea `FLOW_DAVINCI_LANG=en` afișează textele originale englezești (folosită de teste).

## Instalare (macOS)
1. Descarcă pachetul `FlowDaVinci-…-macOS-….pkg` din secțiunea *Releases* a depozitului.
2. Salvează lucrul și închide DaVinci Resolve, apoi deschide `.pkg`-ul.
3. Deschide Resolve normal. Flow DaVinci se încarcă automat. Cerințe: macOS 15+, Resolve 21 în locația standard.

Instalatorul scrie doar în căile proprii (`/Applications/Flow DaVinci.app`, `…/IOPlugins/FlowDaVinci.dvcp.bundle`, `/Library/Application Support/GDC/FlowDaVinci`). **Nu suprascrie, nu șterge și nu dezactivează OpenGate sau Unbound**, nici presetările ori configurațiile lor. Setările Flow DaVinci sunt în `~/Library/Application Support/Flow DaVinci`.

## Dezinstalare și recuperare
Închide Resolve, apoi deschide **Applications → Flow DaVinci Tools → Dezinstaleaza Flow DaVinci**. Presetările și aranjamentele rămân. Recuperarea unei instalări eșuate: `Restaureaza versiunea anterioara Flow DaVinci.command` din dosarul tranzacției (`/Library/Application Support/GDC/FlowDaVinci/Transactions/`).

## Coexistență cu OpenGate/Unbound
- **Instalate pe același calculator:** da, au căi, identificatori și setări distincte.
- **Încărcate simultan în aceeași instanță Resolve:** nu. Ambele ar gestiona aceleași widgeturi Qt. Dacă Flow DaVinci detectează OpenGate/Unbound deja încărcat, **nu pornește** și afișează un mesaj în română; nimic nu este șters sau modificat.
- **Alegerea produsului activ (reversibilă):** **Applications → Flow DaVinci Tools → Alege produsul activ**. Mută (nu șterge) pachetul celuilalt produs într-un dosar inactiv și îl poate readuce. Se rulează doar cu Resolve închis și numai la cererea ta.

## Exportul de suport
Reproduce problema, apoi **Applications → Flow DaVinci Tools → Exporta suport Flow DaVinci**. Se creează un ZIP pe Desktop, cu câmpuri selectate din evenimente și fazele instalării; fără proiecte, media sau jurnale brute. Nimic nu se încarcă automat; verifică ZIP-ul înainte de a-l trimite (deschide un *issue* în depozit).

## Limite cunoscute
- Prototip moștenit din OpenGate 0.0.3: limitele lui rămân valabile (vezi [README-ul OpenGate](doc/opengate-readme.md) și `doc/platforms/`).
- Verificarea în Resolve real, pe Windows, lipsește; instalatorul Windows (dacă este publicat) este etichetat *netestat pe Windows real* și nesemnat.
- Dosarele de setări ale OpenGate/Unbound nu sunt importate automat în Flow DaVinci.
- Traducerea acoperă textele proprii; mesajele de jurnal tehnic rămân în engleză.

## Proveniență, licențe, credite
Vezi [LICENSE](LICENSE) (MIT, Copyright (c) 2026 Wolfgang Woehl; notificările moștenite rămân) și [registrul surselor](doc/flow/SOURCES.md). Presetul opțional „Jordan Tetewsky” provine din OpenGate și rămâne cu atribuire.

## Contribuții
Vezi [CONTRIBUTING.md](CONTRIBUTING.md). Integrarea schimbărilor din OpenGate/Unbound: [doc/flow/SYNC.md](doc/flow/SYNC.md).
