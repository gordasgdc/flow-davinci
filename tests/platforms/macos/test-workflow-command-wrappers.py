#!/usr/bin/env python3
import json,os,runpy,subprocess,tempfile,sys
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
        # Resolve's embedded Python may lack the optional ctypes extension.
        # The request must still be published for the filesystem wake-up path.
        had_ctypes='ctypes' in sys.modules;original_ctypes=sys.modules.get('ctypes')
        sys.modules['ctypes']=None
        try:
            runpy.run_path(str(output/manifest[0]['file']))
        finally:
            if had_ctypes:sys.modules['ctypes']=original_ctypes
            else:sys.modules.pop('ctypes',None)
        fallback=list((Path(directory)/'command-requests').glob('*.json'))
        assert len(fallback)==31
    finally:
        if old is None: os.environ.pop('UNBOUND_LAUNCH_DIRECTORY',None)
        else: os.environ['UNBOUND_LAUNCH_DIRECTORY']=old
print('PASS: 30 native-workflow command wrappers create isolated atomic requests with exact registry IDs')
