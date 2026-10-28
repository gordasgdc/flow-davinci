#!/usr/bin/env python3
"""Verifică manifestul de actualizare PUBLICAT (după deploy): semnătură, câmpuri, pachet descărcabil, dimensiune și SHA-256.
Utilizare: verify_published_update.py [URL_MANIFEST]  (implicit https://gordas.dev/flow-davinci/update-manifest.json)"""
import base64, hashlib, json, sys, urllib.request
from cryptography.hazmat.primitives.asymmetric.ed25519 import Ed25519PublicKey
PUBLIC = 'StnroDRQYwQTvPHHfon+u/x/daxaK0/6vCmL9FhFlE4='
url = sys.argv[1] if len(sys.argv) > 1 else 'https://gordas.dev/flow-davinci/update-manifest.json'
def get(u):
    with urllib.request.urlopen(urllib.request.Request(u, headers={'Cache-Control': 'no-cache'}), timeout=60) as r:
        assert r.status == 200, (u, r.status); return r.read()
env = json.loads(get(url))
assert env['format'] == 'gdc.signed/1' and env['context'] == 'release-manifest' and env['key_id'] == 'gdc-flow-davinci-release-20261028'
payload = base64.b64decode(env['payload'])
Ed25519PublicKey.from_public_bytes(base64.b64decode(PUBLIC)).verify(base64.b64decode(env['signature']), b'gdc.signed/1\nrelease-manifest\n' + payload)
m = json.loads(payload); a = m['artifact']
assert m['product_id'] == 'gdc-flow-davinci' and m['platform'] == 'macos' and a['url'].startswith('https://gordas.dev/flow-davinci/downloads/')
data = get(a['url'])
assert len(data) == a['size'] and hashlib.sha256(data).hexdigest() == a['sha256']
print('OK: manifest semnat valid; versiunea', m['version'], 'build', m['build'], 'secvența', m['sequence'], '; pachet', a['url'], len(data), 'octeți, SHA-256 corect')
