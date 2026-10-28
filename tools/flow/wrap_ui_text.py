#!/usr/bin/env python3
"""Wrap user-visible literals of recognised Qt sinks with FD("...") (Flow DaVinci catalog lookup).
Idempotent. Usage: wrap_ui_text.py [--apply] ; prints the unique wrapped strings (JSON) to stdout."""
import re, sys, json, pathlib
root = pathlib.Path(__file__).resolve().parents[2] / 'src'
LIT = r'"((?:[^"\\]|\\.)*)"'
SINK = r'(?:setToolTip|setText|setWindowTitle|setAccessibleName|setTitle|setPlaceholderText|addAction|addMenu)'
pats = [
    re.compile(r'(\b' + SINK + r'\()' + LIT + r'(?=[\s,)+])'),
    re.compile(r'(\bnew (?:QLabel|QPushButton|QCheckBox|QMenu)\()' + LIT + r'(?=\s*[,)+])'),
    re.compile(r'(\bsetToolTip\([^"\n]*\?\s*)' + LIT),
    re.compile(r'(status->setText\()QStringLiteral\(' + LIT + r'\)'),
    re.compile(r'(\bsetText\(QStringLiteral\()' + LIT + r'(?=\))'),
]
SKIP = re.compile(r'^[^A-Za-z]*$|^%\d$')
found = {}
apply = '--apply' in sys.argv
for p in sorted(root.rglob('*')):
    if p.suffix not in ('.h', '.cpp', '.mm'): continue
    t = p.read_text(); o = t
    def sub(m):
        s = m.group(2)
        if SKIP.match(s) or m.group(0).startswith('FD('): return m.group(0)
        found[s] = found.get(s, 0) + 1
        if m.group(0).count('QStringLiteral('): return m.group(1) + 'FD(' + '"' + s + '")' if m.re.pattern.startswith('(status') else m.group(0)
        return m.group(1) + 'FD("' + s + '")'
    for pat in pats:
        t = pat.sub(sub, t)
    if apply and t != o:
        if '"flowlocale.h"' not in t and 'flowlocale.h' not in t:
            t = t.replace('#pragma once', '#pragma once\n#include "flowlocale.h"', 1) if '#pragma once' in t else t
        p.write_text(t)
print(json.dumps(sorted(found), ensure_ascii=False, indent=0))
