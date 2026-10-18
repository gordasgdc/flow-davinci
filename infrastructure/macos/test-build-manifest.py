#!/usr/bin/env python3
"""Source lock makes integration provenance reproducible without local branches."""
import json
import pathlib
import subprocess
import tempfile
root=pathlib.Path(__file__).resolve().parents[2]
with tempfile.TemporaryDirectory() as directory:
    subprocess.run(['python3',str(root/'infrastructure/macos/build-manifest.py'),directory],check=True)
    manifest=json.loads((pathlib.Path(directory)/'build-manifest.json').read_text())
    lock=json.loads((root/'infrastructure/macos/integration-features.json').read_text())
    assert len(manifest['features']) == len(lock['features']) > 0
    for actual,expected in zip(manifest['features'],lock['features']):
        assert actual['branch']==expected['branch'] and actual['commit']==expected['commit']
        assert actual['included'] and actual['ancestry_verified']
        assert actual['acceptance']=='pending'
    print('PASS: exact integrated feature tips, scope and acceptance states match source lock')
