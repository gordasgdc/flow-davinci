#!/usr/bin/env python3
"""Flow DaVinci identity map (D-6): explicit, ordered substitutions on src/, infrastructure/, tests/.
Internal tokens not listed here (component "OpenGate", Unbound namespace, UNBOUND_* env, Unbound.exe/.dll) stay for compatibility."""
import pathlib, sys
root = pathlib.Path(__file__).resolve().parents[2]
MAP = [
 ('Library/Application Support/CineWaltz/OpenGate', 'Library/Application Support/GDC/FlowDaVinci'),
 ('Application Support/CineWaltz/OpenGate', 'Application Support/GDC/FlowDaVinci'),
 ('Library/Application Support/Unbound', 'Library/Application Support/Flow DaVinci'),
 ('Unbound.dvcp.bundle', 'FlowDaVinci.dvcp.bundle'),
 ('OpenGate.dvcp.bundle', 'FlowDaVinci.dvcp.bundle'),
 ('Unbound.dvcp', 'FlowDaVinci.dvcp'),
 ('OpenGate.dvcp', 'FlowDaVinci.dvcp'),
 ('UnboundWidgetProbe', 'FlowDaVinciWorkspace'),
 ('UnboundLoadingProbe', 'FlowDaVinciLoadingProbe'),
 ('Unbound-retired-', 'FlowDaVinci-retired-'),
 ('Library/Logs/OpenGate', 'Library/Logs/FlowDaVinci'),
 ('OpenGate/Logs', 'FlowDaVinci/Logs'),
 ('com.cinewaltz.opengate', 'dev.gordas.flowdavinci'),
 ('OpenGate Tools', 'Flow DaVinci Tools'),
 ('OpenGate.app', 'Flow DaVinci.app'),
 ('OpenGate.py', 'FlowDaVinci.py'),
 ('OpenGate - ', 'Flow DaVinci - '),
 ('OpenGate-CineWaltz-Prototype', 'FlowDaVinci-GDC'),
]
changed = []
for base in ('src', 'infrastructure', 'tests'):
    for p in sorted((root / base).rglob('*')):
        if not p.is_file() or p.name in ('flowlocale_ro.h',): continue
        try: t = p.read_text()
        except UnicodeDecodeError: continue
        o = t
        for a, b in MAP: t = t.replace(a, b)
        if t != o:
            changed.append(str(p.relative_to(root)))
            if '--apply' in sys.argv: p.write_text(t)
print(len(changed), 'files'); print('\n'.join(changed))
