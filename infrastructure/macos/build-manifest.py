#!/usr/bin/env python3
"""Record only feature tips actually contained in this build's Git ancestry."""
import json
import pathlib
import subprocess
import sys
root = pathlib.Path(__file__).resolve().parents[2]
def git(*args):
    return subprocess.check_output(['git', '-C', str(root), *args], text=True).strip()
features = []
for branch in git('for-each-ref', '--format=%(refname:short)', 'refs/heads/feature/macos-', 'refs/heads/experiment/macos-').splitlines():
    commit = git('rev-parse', branch)
    feature_commits = int(git('rev-list', '--count', '8828868..'+commit))
    included = feature_commits > 0 and subprocess.run(['git', '-C', str(root), 'merge-base', '--is-ancestor', commit, 'HEAD']).returncode == 0
    features.append({'branch': branch, 'commit': commit, 'included': included, 'feature_commits': feature_commits})
manifest = {'schema': 1, 'branch': git('branch', '--show-current'), 'commit': git('rev-parse', 'HEAD'),
            'dirty': bool(git('status', '--porcelain')), 'features': features,
            'meaning': 'Included means present in source ancestry, not acceptance-tested or complete.'}
out = pathlib.Path(sys.argv[1]); out.mkdir(parents=True, exist_ok=True)
payload = json.dumps(manifest, separators=(',', ':'))
(out/'build-manifest.json').write_text(json.dumps(manifest, indent=2)+'\n')
(out/'unboundbuildmanifest.h').write_text('#pragma once\n#define UNBOUND_FEATURE_MANIFEST '+json.dumps(payload)+'\n')
print('Recorded integration feature ancestry in', out/'build-manifest.json')
