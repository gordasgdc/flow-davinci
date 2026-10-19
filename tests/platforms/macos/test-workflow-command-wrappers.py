#!/usr/bin/env python3
import json,os,runpy,subprocess,tempfile
from pathlib import Path
root=Path(__file__).resolve().parents[3]
subprocess.run(['python3',str(root/'infrastructure/macos/build-workflow-commands.py')],check=True)
output=root/'build/macos/workflow-commands'
manifest=json.loads((output/'manifest.json').read_text())
assert len({entry['command'] for entry in manifest})==len(manifest)==30
with tempfile.TemporaryDirectory() as directory:
    old=os.environ.get('UNBOUND_LAUNCH_DIRECTORY'); os.environ['UNBOUND_LAUNCH_DIRECTORY']=directory
    try:
        for entry in manifest:
            runpy.run_path(str(output/entry['file']))
        requests=list((Path(directory)/'command-requests').glob('*.json'))
        assert len(requests)==30
        assert {json.loads(p.read_text())['command'] for p in requests}=={e['command'] for e in manifest}
        assert not list((Path(directory)/'command-requests').glob('*.tmp'))
    finally:
        if old is None: os.environ.pop('UNBOUND_LAUNCH_DIRECTORY',None)
        else: os.environ['UNBOUND_LAUNCH_DIRECTORY']=old
print('PASS: 30 native-workflow command wrappers create isolated atomic requests with exact registry IDs')
