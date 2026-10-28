# Identitate Flow DaVinci: ce s-a schimbat și ce rămâne

## Schimbat (distinct de OpenGate/Unbound)
| Element | OpenGate/Unbound | Flow DaVinci |
|---|---|---|
| Bundle ID aplicație | `com.cinewaltz.opengate` | `dev.gordas.flowdavinci` |
| Plugin IO Resolve (macOS) | `IOPlugins/Unbound.dvcp.bundle` | `IOPlugins/FlowDaVinci.dvcp.bundle` (alt UUID de plugin) |
| Bibliotecă Qt | `UnboundWidgetProbe.dylib` | `FlowDaVinciWorkspace.dylib` |
| Aplicație | `/Applications/OpenGate.app` | `/Applications/Flow DaVinci.app` |
| Unelte | `OpenGate Tools` | `Flow DaVinci Tools` (dezinstalare, suport, alegerea produsului activ) |
| Workflow Integration | `OpenGate*.py`, prefix `OpenGate - ` | `FlowDaVinci.py`, prefix `Flow DaVinci - ` |
| Suport instalator | `/Library/Application Support/CineWaltz/OpenGate` | `/Library/Application Support/GDC/FlowDaVinci` |
| Setări utilizator (macOS) | `~/Library/Application Support/Unbound` | `~/Library/Application Support/Flow DaVinci` |
| Jurnale (macOS) | `~/Library/Logs/OpenGate` | `~/Library/Logs/FlowDaVinci` |
| Notificare între procese | `com.cinewaltz.opengate.command-request` | `dev.gordas.flowdavinci.command-request` |
| Windows: setări / jurnale | `%LOCALAPPDATA%\Unbound`, `…\OpenGate\Logs` | `%LOCALAPPDATA%\FlowDaVinci`, `…\FlowDaVinci\Logs` |
| Windows: plugin IO, instalator, mesaj | `OpenGate.dvcp`, AppId OpenGate…, `OpenGate.WorkflowCommandReady.v1` | `FlowDaVinci.dvcp`, AppId `FlowDaVinci-GDC`, `FlowDaVinci.WorkflowCommandReady.v1` |

## Păstrat intenționat (compatibilitate / sincronizare upstream)
- Spațiul de nume C++ `Unbound`, variabile `UNBOUND_*` și `OPENGATE_*`, numele fișierelor de test și țintelor CMake.
- Windows: `Unbound.exe`/`Unbound.dll` în dosarul de instalare propriu (`{autopf}\FlowDaVinci`); protecția existentă din launcher/IO refuză să încarce un al doilea asistent în același Resolve.
- Valoarea `component` din jurnale (`OpenGate`) și numele comenzilor native după prefix (identificatori stabili pentru scurtături).
- Fișiere `Workspace.json`/`Arrangements.json` (schemă 1).

## Coexistență
- **Instalare pe același calculator:** posibilă (căi/ID-uri distincte; instalatorul schimbă doar căile din propriul inventar).
- **Încărcare simultană în aceeași instanță Resolve:** neacceptată. macOS: `ofx-loading-probe.c` detectează imaginile OpenGate/Unbound deja încărcate și Flow DaVinci nu pornește (mesaj în română, jurnal `other-product-loaded`). Windows: protecția existentă pe `Unbound.dll`, cu mesaj tradus. Alegerea reversibilă a produsului activ: `choose-active-product.command`. Nimic nu este șters sau dezactivat automat.
- Limită: dacă OpenGate se încarcă după Flow DaVinci, OpenGate (neschimbat) pornește oricum; Flow DaVinci cedează.
