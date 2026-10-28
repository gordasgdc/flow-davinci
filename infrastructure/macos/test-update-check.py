#!/usr/bin/env python3
"""Verifică helperul de actualizare: versiune la zi, versiune nouă, lipsa conexiunii, răspuns invalid, semnătură, rollback, descărcare."""
import base64, hashlib, http.server, json, os, pathlib, subprocess, sys, tempfile, threading, datetime
root = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(root / 'tools/flow'))
import sign_update_manifest as S
from cryptography.hazmat.primitives.asymmetric.ed25519 import Ed25519PrivateKey
from cryptography.hazmat.primitives import serialization

work = pathlib.Path(tempfile.mkdtemp(prefix='update-test.', dir=root / 'build/macos'))
binary = work / 'flow-update-test'
subprocess.run(['xcrun', 'swiftc', '-O', '-D', 'FLOW_UPDATE_TESTING', str(root / 'src/platforms/macos/update.swift'), '-o', str(binary)], check=True)
key = Ed25519PrivateKey.generate(); other = Ed25519PrivateKey.generate()
pub = base64.b64encode(key.public_key().public_bytes(serialization.Encoding.Raw, serialization.PublicFormat.Raw)).decode()
web = work / 'web'; web.mkdir()
pkg = b'pachet-fals-de-test' * 100
(web / 'FlowDaVinci-0.2.0-macOS.pkg').write_bytes(pkg)
class Quiet(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *a, **k): super().__init__(*a, directory=str(web), **k)
    def log_message(self, *a): pass
server = http.server.ThreadingHTTPServer(('127.0.0.1', 0), Quiet); port = server.server_address[1]
threading.Thread(target=server.serve_forever, daemon=True).start()
base = f'http://127.0.0.1:{port}'
installed = work / 'installed.json'; installed.write_text(json.dumps({'version': '0.1.0', 'build_number': 5}))
state = work / 'state'; state.mkdir()
home = work / 'home'; home.mkdir()
def payload(**kw):
    d = dict(version='0.2.0', build=7, sequence=1, notes_ro='Noutăți de test.', url=f'{base}/FlowDaVinci-0.2.0-macOS.pkg',
             size=len(pkg), sha256=hashlib.sha256(pkg).hexdigest()); d.update(kw)
    return S.make_payload(**d)
def publish(obj): (web / 'update-manifest.json').write_text(obj if isinstance(obj, str) else json.dumps(obj))
def run(command='check', url=None):
    env = dict(os.environ, HOME=str(home), FLOW_UPDATE_TEST_PUBKEY=pub, FLOW_UPDATE_TEST_URL=url or f'{base}/update-manifest.json',
               FLOW_UPDATE_TEST_INSTALLED=str(installed), FLOW_UPDATE_TEST_STATE=str(state), FLOW_UPDATE_TEST_ARTIFACT_PREFIX=base + '/')
    out = subprocess.run([str(binary), command], env=env, capture_output=True, text=True, timeout=60)
    return json.loads(out.stdout)
failures = []
def check(name, condition, detail=''):
    print(('PASS' if condition else 'FAIL') + ': ' + name + ('' if condition else ' ' + str(detail)))
    if not condition: failures.append(name)

def reset_state(): (state / 'update-state.json').unlink(missing_ok=True)
S_KEY = S.KEY_ID
publish(S.sign(payload(version='0.1.0', build=5), key)); r = run()
check('versiune la zi', r['status'] == 'up_to_date', r)
reset_state(); publish(S.sign(payload(), key)); r = run()
check('versiune nouă disponibilă', r['status'] == 'available' and r['version'] == '0.2.0' and r['notes'] == 'Noutăți de test.', r)
reset_state(); publish(S.sign(payload(version='0.1.0', build=9), key)); r = run()
check('același număr de versiune, build mai nou', r['status'] == 'available' and r['build'] == 9, r)
reset_state(); r = run(url='http://127.0.0.1:1/update-manifest.json')
check('lipsa conexiunii', r['status'] == 'failed' and r['reason'] == 'offline', r)
publish('nu este json'); r = run()
check('răspuns invalid (text)', r['status'] == 'failed' and r['reason'] == 'invalid-response', r)
publish(json.dumps({'format': 'gdc.signed/1'})); r = run()
check('răspuns invalid (câmpuri lipsă)', r['status'] == 'failed' and r['reason'] == 'invalid-response', r)
publish(S.sign(payload(), other)); r = run()
check('semnătură de la altă cheie respinsă', r['status'] == 'failed' and r['reason'] == 'bad-signature', r)
tampered = S.sign(payload(), key); p = json.loads(base64.b64decode(tampered['payload'])); p['version'] = '9.9.9'
tampered['payload'] = base64.b64encode(json.dumps(p).encode()).decode(); publish(tampered); r = run()
check('manifest modificat după semnare respins', r['status'] == 'failed' and r['reason'] == 'bad-signature', r)
publish(S.sign(payload(product='gdc-photo-assist'), key)); r = run()
check('alt produs respins', r['status'] == 'failed' and r['reason'] == 'invalid-response', r)
now = datetime.datetime.now(datetime.timezone.utc)
publish(S.sign(payload(issued=now - datetime.timedelta(days=900), expires=now - datetime.timedelta(days=1)), key)); r = run()
check('manifest expirat respins', r['status'] == 'failed' and r['reason'] == 'expired', r)
reset_state(); publish(S.sign(payload(sequence=5), key)); run()
publish(S.sign(payload(sequence=3), key)); r = run()
check('revenire la un manifest mai vechi respinsă', r['status'] == 'failed' and r['reason'] == 'rollback', r)
reset_state(); publish(S.sign(payload(url='https://exemplu.invalid/x.pkg'), key)); r = run()
check('pachet din afara gordas.dev/flow-davinci respins', r['status'] == 'failed' and r['reason'] == 'bad-artifact-url', r)
reset_state(); publish(S.sign(payload(), key)); r = run('download')
cached = home / 'Library/Caches/dev.gordas.flowdavinci/updates/FlowDaVinci-0.2.0-macOS.pkg'
check('descărcare + verificare SHA-256', r['status'] == 'downloaded' and cached.exists() and cached.read_bytes() == pkg, r)
cached.unlink(); reset_state(); publish(S.sign(payload(sha256='0' * 64), key)); r = run('download')
check('SHA-256 greșit: fișier șters', r['status'] == 'failed' and r['reason'] == 'hash-mismatch' and not cached.exists(), r)
reset_state(); publish(S.sign(payload(version='0.1.0', build=5), key)); r = run('download')
check('fără actualizare nu se descarcă nimic', r['status'] == 'up_to_date' and not cached.exists(), r)
# produsul livrat nu acceptă injecții din mediu și are cheia reală
src = (root / 'src/platforms/macos/update.swift').read_text()
check('cheia publică de producție prezentă', 'StnroDRQYwQTvPHHfon+u/x/daxaK0/6vCmL9FhFlE4=' in src)
check('manifestul de producție sub gordas.dev/flow-davinci', 'https://gordas.dev/flow-davinci/update-manifest.json' in src)
pay = (root / 'infrastructure/macos/packagepayload.py').read_text()
ident = (root / 'src/core/productidentity.h').read_text()
check('calea versiunii instalate = locul din pachet (Current/)', 'GDC/FlowDaVinci/Current/release-manifest.json' in src and 'GDC/FlowDaVinci/Current/release-manifest.json' in ident and "FlowDaVinci/Current')" in pay)
server.shutdown()
print('PASS: actualizări (toate cazurile)' if not failures else 'FAIL: ' + ', '.join(failures))
sys.exit(1 if failures else 0)
