#!/usr/bin/env python3
"""Verifica ca fiecare FD("...") din surse are cheie in catalogul RO si raporteaza chei nefolosite.
Iesire 1 daca lipsesc chei (texte noi din upstream netraduse)."""
import re, sys, pathlib
root = pathlib.Path(__file__).resolve().parents[2]
lit = r'"((?:[^"\\]|\\.)*)"'
catalog = set(m.group(1) for m in re.finditer(r'\{' + lit + r',\s*' + lit + r'\}', (root / 'src/core/flowlocale_ro.h').read_text()))
used = {}
for p in (root / 'src').rglob('*'):
    if p.suffix not in ('.h', '.cpp', '.mm') or p.name == 'flowlocale_ro.h': continue
    for m in re.finditer(r'\bFD\((?:[^"()]*\?\s*)?' + lit + r'(?:\s*:\s*FD\(' + lit + r'\))?', p.read_text()):
        for g in m.groups():
            if g: used[g] = str(p.relative_to(root))
missing = sorted(set(used) - catalog)
for s in missing: print('LIPSA in catalog:', used[s], '->', s)
print(len(used), 'chei folosite;', len(missing), 'lipsa;', len(catalog - set(used)), 'in catalog dar nefolosite direct (etichete dinamice sau variante).')
sys.exit(1 if missing else 0)
