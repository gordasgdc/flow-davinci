#!/usr/bin/env python3
"""Romanian texts for the macOS installer/uninstaller/support/packaging scripts (exact-string replacements; fails on a missing source)."""
import pathlib
R = pathlib.Path(__file__).resolve().parents[2] / 'infrastructure/macos'
T = {
'installer/preinstall': [
 ("'Install on the running startup disk'", "'Instalează pe discul de pornire activ'"),
 ("'Unsupported processor architecture'", "'Arhitectură de procesor nesuportată'"),
 ("'This package requires macOS @MIN_MACOS@ or later'", "'Acest pachet necesită macOS @MIN_MACOS@ sau mai nou'"),
 ("'Save and quit DaVinci Resolve, then run this installer again'", "'Salvează și închide DaVinci Resolve, apoi rulează din nou instalatorul'"),
 ("'Could not verify whether Resolve is running'", "'Nu s-a putut verifica dacă Resolve rulează'"),
 ("'DaVinci Resolve was not found at its supported installation location'", "'DaVinci Resolve nu a fost găsit în locația de instalare suportată'"),
 ("'This prototype requires Resolve 21; tested with @RESOLVE_VERSION@'", "'Acest prototip necesită Resolve 21; testat cu @RESOLVE_VERSION@'"),
 ("'Cannot read installed settings compatibility metadata'", "'Nu pot citi metadatele de compatibilitate ale setărilor instalate'"),
 ("'Installed OpenGate uses a newer data schema; use its recovery or compatible installer'", "'Flow DaVinci instalat folosește o schemă de date mai nouă; folosește recuperarea sau un instalator compatibil'"),
 ("'Cannot read installed OpenGate version'", "'Nu pot citi versiunea Flow DaVinci instalată'"),
 ("'Installed version metadata is invalid'", "'Metadatele versiunii instalate sunt nevalide'"),
 ("'Another install or recovery is pending; use the recorded recovery action first'", "'O altă instalare sau recuperare este în curs; folosește mai întâi acțiunea de recuperare înregistrată'"),
 ("Restore Previous OpenGate.command", "Restaureaza versiunea anterioara Flow DaVinci.command"),
],
'installer/postinstall': [
 ("'OpenGate installed. Open Resolve. Fresh profiles start in Resolve Layout; existing profiles retain their arrangement.'", "'Flow DaVinci a fost instalat. Deschide Resolve. Profilurile noi pornesc în Resolve Layout; profilurile existente își păstrează aranjamentul.'"),
 ('"Recovery and diagnostic record: $transaction"', '"Înregistrare de recuperare și diagnostic: $transaction"'),
],
'installer/uninstall.command': [
 ("'Save and quit Resolve before uninstalling.'", "'Salvează și închide Resolve înainte de dezinstalare.'"),
 ("'Install or recovery is pending; complete recovery before uninstalling'", "'O instalare sau recuperare este în curs; finalizeaz-o înainte de dezinstalare'"),
 ("Restore Previous OpenGate.command", "Restaureaza versiunea anterioara Flow DaVinci.command"),
 ("'uninstalled; OpenGate settings and native Resolve data preserved'", "'uninstalled; setările Flow DaVinci și datele native Resolve au fost păstrate'"),
 ("'OpenGate removed. Reopen Resolve for its native UI. Your presets remain available for reinstall.'", "'Flow DaVinci a fost eliminat. Redeschide Resolve pentru interfața nativă. Presetările tale rămân disponibile la reinstalare.'"),
 ('"Recovery: $transaction/Restaureaza versiunea anterioara Flow DaVinci.command"', '"Recuperare: $transaction/Restaureaza versiunea anterioara Flow DaVinci.command"'),
],
'installer/rollback.command': [
 ("'Save and quit Resolve before recovery.'", "'Salvează și închide Resolve înainte de recuperare.'"),
 ("'Previous OpenGate files restored. User presets and Resolve data were not changed.'", "'Fișierele Flow DaVinci anterioare au fost restaurate. Presetările utilizatorului și datele Resolve nu au fost modificate.'"),
],
'installer/export-support.command': [
 ('OpenGate-Support-', 'FlowDaVinci-Suport-'),
 ("'OpenGate support report — no project/media contents or raw runtime logs included'", "'Raport de suport Flow DaVinci — fără conținut de proiect/media și fără jurnale brute'"),
 ('"Support report: $output.zip"', '"Raport de suport: $output.zip"'),
 ("'Review the report before attaching it. Nothing was uploaded.'", "'Verifică raportul înainte de a-l trimite. Nimic nu a fost încărcat.'"),
],
'installer/export-support.js': [
 ("for(const directory of ['Unbound','OpenGate'])", "for(const directory of ['FlowDaVinci'])"),
 ("product:'OpenGate'", "product:'Flow DaVinci'"),
 ("'Could not save support summary.'", "'Rezumatul de suport nu a putut fi salvat.'"),
],
'installer/transaction.sh': [
 ("Export OpenGate Support.command", "Exporta suport Flow DaVinci.command"),
 ("Uninstall OpenGate.command", "Dezinstaleaza Flow DaVinci.command"),
 ("'/Library/Application Support/Blackmagic Design/DaVinci Resolve/Workflow Integration Plugins/'OpenGate*.py)", "'/Library/Application Support/Blackmagic Design/DaVinci Resolve/Workflow Integration Plugins/'FlowDaVinci*.py|'/Library/Application Support/Blackmagic Design/DaVinci Resolve/Workflow Integration Plugins/'Flow\\ DaVinci\\ -\\ *.py)"),
 ('echo "OpenGate: $*. Diagnostic record: $transaction/events.log" >&2', 'echo "Flow DaVinci: $*. Înregistrare de diagnostic: $transaction/events.log" >&2'),
 ("'Invalid retired loader identity'", "'Identitate nevalidă a încărcătorului retras'"),
 ("'Inventory contains a path outside OpenGate managed files'", "'Inventarul conține o cale în afara fișierelor gestionate de Flow DaVinci'"),
 ("'Invalid package inventory'", "'Inventar de pachet nevalid'"),
],
'installer/legacy-loaders.sh': [
 ('[[ "$product_name" == OpenGate ]]', '[[ "$product_name" == "Flow DaVinci" ]]'),
 ('# Retired OpenGate loaders', '# Retired Flow DaVinci loaders (never OpenGate or Unbound)'),
],
'packagepayload.py': [
 ("'product': 'OpenGate', 'vendor': 'CineWaltz'", "'product': 'Flow DaVinci', 'vendor': 'GDC (Cristi Gordas)'"),
 ("'foundation': 'Unbound by Wolfgang Woehl (MIT)'", "'foundation': 'OpenGate by CineWaltz (MIT), based on Unbound by Wolfgang Woehl (MIT)'"),
 ("'OpenGate ' + version + ' prototype candidate (' + build + ')\\n\\n'", "'Flow DaVinci ' + version + ' candidat prototip (' + build + ')\\n\\n'"),
 ("'This candidate has not been live-tested inside Resolve on macOS or Windows.\\n'", "'Acest candidat nu a fost testat live în Resolve pe macOS sau Windows.\\n'"),
 ("'This package contains macOS binaries only; no Windows parity claim.\\n'", "'Acest pachet conține doar binare macOS; nu se susține paritatea cu Windows.\\n'"),
 ("'Compilation and synthetic fixtures do not establish native GPU rendering or real input acceptance.\\n'", "'Compilarea și testele sintetice nu dovedesc randarea GPU nativă sau acceptarea intrărilor reale.\\n'"),
 ("'Displayed fixture status: '", "'Starea testelor cu ferestre: '"),
 ("'Save and quit Resolve before installation, removal or recovery.\\n'", "'Salvează și închide Resolve înainte de instalare, eliminare sau recuperare.\\n'"),
 ("'Fresh profiles start in Resolve Layout. Jordan Tetewsky Preset is optional.\\n'", "'Profilurile noi pornesc în Resolve Layout. Presetul Jordan Tetewsky (din OpenGate) este opțional.\\n'"),
 ("'Existing profiles and user presets are preserved; installing a version does not reset them.\\n'", "'Profilurile și presetările existente sunt păstrate; instalarea unei versiuni nu le resetează.\\n'"),
 ("'Applications/Flow DaVinci Tools contains double-click uninstall and support export.\\n'", "'Applications/Flow DaVinci Tools conține dezinstalarea, exportul de suport și alegerea produsului activ (dublu clic).\\n'"),
 ("'Recovery copies and install diagnostics: /Library/Application Support/GDC/FlowDaVinci/Transactions/.\\n'", "'Copii de recuperare și diagnostic de instalare: /Library/Application Support/GDC/FlowDaVinci/Transactions/.\\n'"),
 ("'A failed/interrupted install leaves a Restore Previous OpenGate.command in its transaction directory.\\n'", "'O instalare eșuată sau întreruptă lasă „Restaureaza versiunea anterioara Flow DaVinci.command” în directorul tranzacției.\\n'"),
 ("'OpenGate by CineWaltz, based on Unbound by Wolfgang Woehl (MIT); see LICENSE.\\n'", "'Flow DaVinci: adaptare în limba română inițiată de Cristi Gordas (GDC), bazată pe OpenGate de CineWaltz, la rândul său bazat pe Unbound de Wolfgang Woehl (MIT); vezi LICENSE. Nu este afiliat cu Blackmagic Design.\\n'"),
 ("('Export OpenGate Support.command', 'export-support.command'),\n                           ('Uninstall OpenGate.command', 'uninstall.command')]", "('Exporta suport Flow DaVinci.command', 'export-support.command'),\n                           ('Dezinstaleaza Flow DaVinci.command', 'uninstall.command'),\n                           ('Alege produsul activ.command', 'choose-active-product.command')]"),
 ("'/Applications/Flow DaVinci Tools/Export OpenGate Support.command', '/Applications/Flow DaVinci Tools/Uninstall OpenGate.command']", "'/Applications/Flow DaVinci Tools/Exporta suport Flow DaVinci.command', '/Applications/Flow DaVinci Tools/Dezinstaleaza Flow DaVinci.command', '/Applications/Flow DaVinci Tools/Alege produsul activ.command']"),
],
'package-prototype.py': [
 ("filename = 'OpenGate-'", "filename = 'FlowDaVinci-'"),
 ("title = 'OpenGate ' + args.version + (' Local Candidate' if args.candidate else ' Prototype')", "title = 'Flow DaVinci ' + args.version + (' — candidat local' if args.candidate else ' — prototip')"),
 ("<p>Save and quit Resolve before installing. Requires macOS ' + metadata['minimum_macos'] + '+ and Resolve 21 at its standard location. Includes Apple Silicon and Intel binaries; live validation for this candidate is pending.</p><p>This candidate has not been tested inside Resolve on macOS or Windows. Displayed fixture status is recorded in the manifest; do not infer runtime acceptance from compilation.</p><p>Existing presets and arrangements are preserved. Signing and notarization status are recorded in the accompanying release manifest. Based on Unbound by Wolfgang Woehl (MIT).</p>", "<p>Salvează și închide Resolve înainte de instalare. Necesită macOS ' + metadata['minimum_macos'] + '+ și Resolve 21 în locația standard. Include binare Apple Silicon și Intel; validarea live a acestui candidat este în așteptare.</p><p>Acest candidat nu a fost testat în Resolve pe macOS sau Windows. Starea testelor este înregistrată în manifest; nu deduce acceptarea la rulare din compilare.</p><p>Presetările și aranjamentele existente sunt păstrate. OpenGate și Unbound nu sunt șterse sau dezactivate. Starea semnării și a notarizării este în manifestul de lansare. Flow DaVinci este o adaptare în limba română, bazată pe OpenGate de CineWaltz și pe Unbound de Wolfgang Woehl (MIT).</p>"),
 ("<h1>OpenGate installed</h1><p>Open Resolve. New users begin in Resolve Layout; Jordan Tetewsky Preset is optional. Existing users retain their last working state.</p><p>Double-click uninstall and support export in Applications/Flow DaVinci Tools. Recovery records: /Library/Application Support/GDC/FlowDaVinci/Transactions/. Projects and media are not installation targets.</p>", "<h1>Flow DaVinci a fost instalat</h1><p>Deschide Resolve. Utilizatorii noi încep în Resolve Layout; presetul Jordan Tetewsky (din OpenGate) este opțional. Utilizatorii existenți își păstrează ultima stare de lucru.</p><p>Dezinstalarea, exportul de suport și alegerea produsului activ (dublu clic) sunt în Applications/Flow DaVinci Tools. Înregistrări de recuperare: /Library/Application Support/GDC/FlowDaVinci/Transactions/. Proiectele și media nu sunt ținte de instalare.</p>"),
],
}
for rel, pairs in T.items():
    p = R / rel; t = p.read_text()
    for a, b in pairs:
        assert a in t, (rel, a[:70])
        t = t.replace(a, b)
    p.write_text(t)
print('ok')
