#!/usr/bin/env python3
import hashlib
import importlib.util
import json
from pathlib import Path
import tempfile
root=Path(__file__).resolve().parents[2]
spec=importlib.util.spec_from_file_location('profile_setup',root/'infrastructure/macos/first-install-profile.py')
setup=importlib.util.module_from_spec(spec);spec.loader.exec_module(setup)
bundle=json.loads((root/'src/assets/presets/BundledArrangements.json').read_text())
with tempfile.TemporaryDirectory() as directory:
    base=Path(directory);setup.seed(base,bundle)
    before={p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in base.iterdir()}
    try:setup.seed(base,bundle)
    except RuntimeError:pass
    else:raise AssertionError('existing settings were overwritten')
    assert before=={p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in base.iterdir()}
    workspace=json.loads((base/'Workspace.json').read_text())
    assert workspace['selected_arrangement']==bundle['default']
    assert workspace['footer']['attachment']=='timeline' and not workspace['footer']['hidden']
    assert len(json.loads((base/'Arrangements.json').read_text())['arrangements'])==3
    group=workspace['groups'][0]
    assert len(group['panels'])==len(set(group['panels']))==7
    assert all(workspace['panels'][name]['floating'] for name in group['panels'])
print('PASS: isolated first-install seed, Native Emulator, two bundled presets and existing-settings preservation')
