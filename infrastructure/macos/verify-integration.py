#!/usr/bin/env python3
"""Run the complete fixture matrix; retain failures and exact source provenance."""
import datetime
import hashlib
import json
from pathlib import Path
import subprocess
import sys
import time

root = Path(__file__).resolve().parents[2]
def git(*args):
    return subprocess.check_output(['git', '-C', str(root), *args], text=True).strip()

resolve = subprocess.run(['pgrep', '-x', 'Resolve'], capture_output=True, text=True)
if resolve.returncode != 1:
    print('NOT RUN: Resolve must be closed and its process state verifiable.', flush=True)
    sys.exit(2)

commit = git('rev-parse', 'HEAD')
stamp = datetime.datetime.now(datetime.timezone.utc).strftime('%Y%m%dT%H%M%SZ')
output = root / 'build/macos/verification' / (commit[:10] + '-' + stamp)
output.mkdir(parents=True, exist_ok=False)
tests = ['build-manifest', 'panel-placement', 'diagnostics', 'layout-plans',
         'workspace-pages', 'workspace-menu', 'panel-sessions', 'panel-groups',
         'native-panel-positions', 'native-arrangements', 'native-workspace-ui', 'group-signal-lifetimes', 'controls-hidden', 'reliability', 'instrumentation', 'responsiveness']
receipt = {'commit': commit, 'branch': git('branch', '--show-current'),
           'dirty_at_start': bool(git('status', '--porcelain')),
           'source_lock_sha256': hashlib.sha256((root / 'infrastructure/macos/integration-features.json').read_bytes()).hexdigest(),
           'scope': 'Fixtures only; does not establish live Resolve or GPU/playback acceptance',
           'tests': [], 'all_passed': False}
def save():
    temporary = output / 'receipt.tmp'
    temporary.write_text(json.dumps(receipt, indent=2) + '\n')
    temporary.replace(output / 'receipt.json')

save()
for name in tests:
    script = root / 'infrastructure/macos' / ('test-' + name + ('.py' if name == 'build-manifest' else '.sh'))
    command = [sys.executable, str(script)] if script.suffix == '.py' else ['bash', str(script)]
    print('RUN: ' + name, flush=True)
    start = time.monotonic()
    log = output / (name + '.log')
    with log.open('w') as stream:
        try:
            result = subprocess.run(command, cwd=root, stdout=stream, stderr=subprocess.STDOUT, timeout=180)
            code = result.returncode
        except subprocess.TimeoutExpired:
            stream.write('\nFixture exceeded its 180-second deadline.\n')
            code = 124
    status = 'PASS' if code == 0 else 'NOT RUN' if code == 2 else 'FAIL'
    receipt['tests'].append({'name': name, 'status': status, 'exit_code': code,
                             'elapsed_seconds': round(time.monotonic()-start, 3), 'log': log.name})
    save()
    print(status + ': ' + name + ' (' + str(log) + ')', flush=True)

receipt['all_passed'] = all(item['status'] == 'PASS' for item in receipt['tests'])
receipt['source_unchanged'] = git('rev-parse', 'HEAD') == commit and not git('status', '--porcelain')
save()
print('Receipt: ' + str(output / 'receipt.json'), flush=True)
sys.exit(0 if receipt['all_passed'] and receipt['source_unchanged'] and not receipt['dirty_at_start'] else 1)
