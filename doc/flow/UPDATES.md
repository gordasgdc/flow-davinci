# Identificare și actualizare (macOS)

**Mecanism:** descărcare ghidată. Flow DaVinci nu înlocuiește nimic cât rulează Resolve și nu descarcă sau execută cod din OpenGate/Unbound; ofertele vin doar din lansările Flow DaVinci.

## Sursa unică de versiune
- `VERSION` (repo) → `release-manifest.json` scris de pachet în `/Library/Application Support/GDC/FlowDaVinci/` (`version`, `build_number` = număr de commit-uri, `build` = commit scurt) → afișat de «Despre Flow DaVinci» și comparat de actualizare.
- Același `VERSION` intră în `Info.plist` al aplicației (`CFBundleShortVersionString`/`CFBundleVersion`), în numele pachetului și în manifestul public. Windows: `VERSION` → `UNBOUND_VERSION`/`FLOW_DAVINCI_VERSION` la compilare.

## Interfață
- În Resolve: meniul Flow DaVinci → «Despre Flow DaVinci…» și «Verifică actualizările…» (apare numai dacă helperul există).
- Aplicația `/Applications/Flow DaVinci.app`: butoanele «Despre Flow DaVinci» și «Verifică actualizările…» (calea fără Resolve pornit, necesară pentru instalare).
- Rezultate: la zi / versiune nouă (versiune, build, noutăți, «Descarcă și verifică») / verificare nereușită (fără conexiune sau răspuns invalid).

## Helper și verificări
- `src/platforms/macos/update.swift` → `Flow DaVinci.app/Contents/MacOS/flow-update` (`check`, `download`). Manifest HTTPS semnat Ed25519 (`gdc.signed/1`, cheia `gdc-flow-davinci-release-20261028`, publică în cod), cu produs, platformă, expirare, secvență anti-revenire (stare în `~/Library/Application Support/GDC/FlowDaVinci/update-state.json`) și prefix obligatoriu `https://gordas.dev/flow-davinci/downloads/`.
- Descărcarea verifică dimensiunea, SHA-256, `pkgutil --check-signature` (Developer ID Installer, Team 8AR6XP8MG7) și `spctl --assess --type install` (notarizare); un fișier care pică se șterge. Pachetul se salvează în `~/Library/Caches/dev.gordas.flowdavinci/updates/` și se deschide la cererea utilizatorului, cu Resolve închis. Instalatorul tranzacțional păstrează setările și presetările.
- Teste: `infrastructure/macos/test-update-check.py` (la zi, versiune nouă, fără conexiune, răspuns invalid, semnătură greșită/modificată, alt produs, expirat, revenire, URL străin, descărcare, SHA greșit) și `infrastructure/macos/test-about.sh` (identitate, versiune/build, texte, punte UI↔helper). Ambele rulează în matricea ascunsă.

## Locația metadatelor
- Manifest: `https://gordas.dev/flow-davinci/update-manifest.json` = `docs/flow-davinci/update-manifest.json` în depozitul site-ului (`gordasgdc/gdc-plugin-manager`, GitHub Pages). Pachete: `https://gordas.dev/flow-davinci/downloads/FlowDaVinci-<versiune>-macOS.pkg`.
- Cheia privată: `/Users/gordasgdc/Developer/Certificates/gdc-flow-davinci-release-20261028.ed25519.pk8` (în afara depozitului; de salvat cu procedura de backup a cheilor de produs). Nu se pune în repo, CI sau loguri.
- Windows: fără canal de actualizare (nu există artefact Windows verificat); meniul de actualizare nu apare. Fără verificare pe Windows real.

## Pași la fiecare release
1. Crește `VERSION` (+ `CHANGELOG`), commit curat, rulează `verify-integration.py --hidden-only` (receipt PASS), apoi `package-prototype.py --sign --notary-profile gdc-notary` (semnare, notarizare, stapling, spctl).
2. Copiază pachetul verificat în site: `docs/flow-davinci/downloads/FlowDaVinci-<v>-macOS.pkg`.
3. Semnează manifestul: `python3 tools/flow/sign_update_manifest.py --pkg … --version … --build … --sequence <N+1> --notes-ro "…" --url https://gordas.dev/flow-davinci/downloads/<fișier> --out docs/flow-davinci/update-manifest.json`. Secvența crește mereu.
4. Actualizează `release.json` (starea, URL, SHA-256, dimensiunea) și regenerează pagina (`build_page.py`); commit + push în depozitul site-ului.
5. După deploy: `python3 tools/flow/verify_published_update.py` (semnătură + pachet + SHA-256 pe URL-ul public) și verifică pagina. Nu publica manifestul înaintea pachetului.
