#!/usr/bin/env python3
"""Record only feature tips actually contained in this build's Git ancestry."""
import json
import pathlib
import subprocess
import sys
root = pathlib.Path(__file__).resolve().parents[2]
def git(*args):
    return subprocess.check_output(['git', '-C', str(root), *args], text=True).strip()
lock = root/'infrastructure/macos/integration-features.json'
features = []
if lock.exists():
    declared = json.loads(lock.read_text())
    for entry in declared['features']:
        item = dict(entry)
        result = subprocess.run(['git', '-C', str(root), 'merge-base', '--is-ancestor', entry['commit'], 'HEAD'], capture_output=True)
        item['included'] = result.returncode == 0
        item['ancestry_verified'] = result.returncode in (0, 1)
        features.append(item)
manifest = {'schema': 1, 'product': 'OpenGate', 'vendor': 'CineWaltz', 'foundation': 'Unbound by Wolfgang Woehl (MIT)', 'branch': git('branch', '--show-current'), 'commit': git('rev-parse', 'HEAD'),
            'dirty': bool(git('status', '--porcelain')), 'features': features,
            'meaning': 'Included means present in source ancestry, not acceptance-tested or complete.'}
out = pathlib.Path(sys.argv[1]); out.mkdir(parents=True, exist_ok=True)
payload = json.dumps(manifest, separators=(',', ':'))
(out/'build-manifest.json').write_text(json.dumps(manifest, indent=2)+'\n')
(out/'unboundbuildmanifest.h').write_text('#pragma once\n#define UNBOUND_FEATURE_MANIFEST '+json.dumps(payload)+'\n')
print('Recorded integration feature ancestry in', out/'build-manifest.json')
