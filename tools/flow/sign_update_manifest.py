#!/usr/bin/env python3
"""Semnează manifestul de actualizare Flow DaVinci (gdc.signed/1, Ed25519), la fiecare release.
Cheia privată (PKCS8 în base64) rămâne în afara depozitului: /Users/gordasgdc/Developer/Certificates/gdc-flow-davinci-release-20261028.ed25519.pk8.
Utilizare: sign_update_manifest.py --pkg FISIER.pkg --version 0.1.0 --build 12 --sequence 1 --notes-ro "..." --url https://gordas.dev/flow-davinci/downloads/FlowDaVinci-0.1.0-macOS.pkg --out update-manifest.json"""
import argparse, base64, hashlib, json, pathlib, datetime, sys
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric.ed25519 import Ed25519PrivateKey

KEY_ID = 'gdc-flow-davinci-release-20261028'
DEFAULT_KEY = '/Users/gordasgdc/Developer/Certificates/gdc-flow-davinci-release-20261028.ed25519.pk8'

def load_key(path):
    return serialization.load_der_private_key(base64.b64decode(pathlib.Path(path).read_text().strip()), password=None)

def make_payload(*, version, build, sequence, notes_ro, url, size, sha256, product='gdc-flow-davinci', platform='macos',
                 channel='test', mandatory=False, issued=None, expires=None, team='8AR6XP8MG7'):
    now = issued or datetime.datetime.now(datetime.timezone.utc).replace(microsecond=0)
    fmt = lambda d: d.strftime('%Y-%m-%dT%H:%M:%SZ')
    return {'schema': 'gdc.release-manifest/1', 'product_id': product, 'platform': platform, 'channel': channel,
            'version': version, 'build': build, 'sequence': sequence, 'issued_at': fmt(now),
            'expires_at': fmt(expires or now + datetime.timedelta(days=730)), 'mandatory': mandatory,
            'notes': {'ro': notes_ro}, 'artifact': {'kind': 'pkg', 'url': url, 'size': size, 'sha256': sha256,
                                                   'bundle_id': 'dev.gordas.flowdavinci', 'team_id': team}}

def sign(payload_obj, key, key_id=KEY_ID):
    payload = json.dumps(payload_obj, ensure_ascii=False, separators=(',', ':')).encode()
    signature = key.sign(b'gdc.signed/1\nrelease-manifest\n' + payload)
    return {'format': 'gdc.signed/1', 'context': 'release-manifest', 'key_id': key_id,
            'payload': base64.b64encode(payload).decode(), 'signature': base64.b64encode(signature).decode()}

def main():
    a = argparse.ArgumentParser(description=__doc__)
    a.add_argument('--pkg', type=pathlib.Path, required=True); a.add_argument('--version', required=True)
    a.add_argument('--build', type=int, required=True); a.add_argument('--sequence', type=int, required=True)
    a.add_argument('--notes-ro', required=True); a.add_argument('--url', required=True)
    a.add_argument('--out', type=pathlib.Path, required=True); a.add_argument('--key', default=DEFAULT_KEY)
    a.add_argument('--mandatory', action='store_true')
    args = a.parse_args()
    data = args.pkg.read_bytes()
    if not args.url.startswith('https://gordas.dev/flow-davinci/downloads/'):
        sys.exit('URL-ul pachetului trebuie să fie sub https://gordas.dev/flow-davinci/downloads/')
    payload = make_payload(version=args.version, build=args.build, sequence=args.sequence, notes_ro=args.notes_ro, url=args.url,
                           size=len(data), sha256=hashlib.sha256(data).hexdigest(), mandatory=args.mandatory)
    args.out.write_text(json.dumps(sign(payload, load_key(args.key)), ensure_ascii=False, indent=1) + '\n')
    print('scris', args.out, 'secvența', args.sequence)

if __name__ == '__main__':
    main()
