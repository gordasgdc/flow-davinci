# SPDX-License-Identifier: MIT
"""Construct a Mac payload and its exact installation/recovery inventory."""
import json
import pathlib
import plistlib
import shutil
import subprocess
from packageinputs import sha
from packagesigning import sign_code

SUPPORT = pathlib.Path('Library/Application Support/GDC/FlowDaVinci/Current')
RESOLVE = pathlib.Path('Library/Application Support/Blackmagic Design/DaVinci Resolve')


def copy(source, target):
    target.parent.mkdir(parents=True, exist_ok=True)
    subprocess.run(['/usr/bin/ditto', str(source), str(target)], check=True)


def prepare(root, artifacts, inputs, payload, scripts):
    version = inputs['version']; build = inputs['commit'][:7]
    runtime = inputs['runtime_manifest']
    packaging_commit = subprocess.check_output(['git', '-C', str(root), 'rev-parse', 'HEAD'], text=True).strip()
    build_number = subprocess.check_output(['git', '-C', str(root), 'rev-list', '--count', inputs['commit']], text=True).strip()
    minimum = max((runtime['macos_deployment_target'], runtime['resolve_build_host']['minimum_macos']),
                  key=lambda s: tuple(map(int, s.split('.'))))
    metadata = {'schema': 1, 'product': 'Flow DaVinci', 'vendor': 'GDC (Cristi Gordas)', 'version': version,
                'build': build, 'packaging_commit': packaging_commit, 'package_build': build + '-pkg' + packaging_commit[:7], 'build_number': int(build_number), 'runtime_commit': inputs['commit'], 'platform': 'macos',
                'architecture': runtime['macos_architectures'], 'minimum_macos': minimum,
                'tested_resolve': runtime['resolve_build_host']['version'], 'settings_schema_written': 1,
                'settings_schema_read_max': 1, 'signed': 'Developer ID' if inputs.get('signing') else 'ad-hoc',
                'notarization_requested': bool(inputs.get('notary_profile')),
                'validation': 'Local candidate; runtime acceptance pending on both platforms.' if inputs.get('candidate') else 'Hidden and displayed fixtures; actual Resolve acceptance remains separate.',
                'displayed_verification': 'pending' if inputs.get('candidate') else 'passed',
                'live_macos_verified': False, 'live_windows_verified': False, 'windows_artifact_built': False,
                'foundation': 'OpenGate by CineWaltz (MIT), based on Unbound by Wolfgang Woehl (MIT)'}
    copy(inputs['bundle'], payload / RESOLVE / 'IOPlugins/FlowDaVinci.dvcp.bundle')
    copy(artifacts / 'Flow DaVinci.app', payload / 'Applications/Flow DaVinci.app')
    info = payload / 'Applications/Flow DaVinci.app/Contents/Info.plist'
    app_info = plistlib.loads(info.read_bytes())
    app_info.update(CFBundleShortVersionString=version, CFBundleVersion=build_number, LSMinimumSystemVersion=minimum)
    info.write_bytes(plistlib.dumps(app_info))
    subprocess.run(['/usr/bin/codesign', '--force', '--sign', '-', str(info.parents[1])], check=True)
    workflow = payload / RESOLVE / 'Workflow Integration Plugins'
    copy(root / 'src/platforms/macos/FlowDaVinci.py', workflow / 'FlowDaVinci.py')
    commands = json.loads((artifacts / 'workflow-commands/manifest.json').read_text())
    for entry in commands:
        name = entry['file']
        if pathlib.Path(name).name != name:
            raise ValueError('Invalid workflow filename.')
        copy(artifacts / 'workflow-commands' / name, workflow / name)
    support = payload / SUPPORT; support.mkdir(parents=True)
    copy(root / 'LICENSE', support / 'LICENSE')
    copy(inputs['bundle'] / 'Contents/Resources/build-manifest.json', support / 'runtime-build-manifest.json')
    (support / 'release-manifest.json').write_text(json.dumps(metadata, indent=2) + '\n')
    (support / 'README.txt').write_text(
        'Flow DaVinci ' + version + ' candidat prototip (' + build + ')\n\n'
        'Acest candidat nu a fost testat live în Resolve pe macOS sau Windows.\n'
        'Acest pachet conține doar binare macOS; nu se susține paritatea cu Windows.\n'
        'Compilarea și testele sintetice nu dovedesc randarea GPU nativă sau acceptarea intrărilor reale.\n'
        'Starea testelor cu ferestre: ' + metadata['displayed_verification'] + '.\n\n'
        'Salvează și închide Resolve înainte de instalare, eliminare sau recuperare.\n'
        'Profilurile noi pornesc în Resolve Layout. Presetul Jordan Tetewsky (din OpenGate) este opțional.\n'
        'Profilurile și presetările existente sunt păstrate; instalarea unei versiuni nu le resetează.\n'
        'Applications/Flow DaVinci Tools conține dezinstalarea, exportul de suport și alegerea produsului activ (dublu clic).\n'
        'Copii de recuperare și diagnostic de instalare: /Library/Application Support/GDC/FlowDaVinci/Transactions/.\n'
        'O instalare eșuată sau întreruptă lasă „Restaureaza versiunea anterioara Flow DaVinci.command” în directorul tranzacției.\n'
        'Support export includes selected event fields and install phases, not raw logs, project/media contents or arbitrary error text. Review before sharing. Nothing is uploaded automatically.\n\n'
        'Remaining live checks include Mixer custom-title dragging/control lifetime, Index acquisition readiness, native painting, physical/remapped keys and full Native/OpenGate transitions.\n'
        + ('Code and installer use Developer ID signing. Check the accompanying release manifest for notarization acceptance.\n' if inputs.get('signing') else 'The package is ad-hoc signed, unsigned as an installer, and not notarized. Downloaded copies may receive macOS security prompts.\n')
        + 'Flow DaVinci: adaptare în limba română inițiată de Cristi Gordas (GDC), bazată pe OpenGate de CineWaltz, la rândul său bazat pe Unbound de Wolfgang Woehl (MIT); vezi LICENSE. Nu este afiliat cu Blackmagic Design.\n')

    bundle = json.loads((root / 'src/assets/presets/BundledArrangements.json').read_text())
    chosen = next(e for e in bundle['arrangements'] if e['id'] == bundle['default'])
    state = dict(chosen['workspace']); state['selected_arrangement'] = chosen['id']
    presets = support / 'Presets'; presets.mkdir()
    (presets / 'Workspace.json').write_text(json.dumps(state, indent=2) + '\n')
    (presets / 'Arrangements.json').write_text(json.dumps({'version': 1, 'arrangements': [e for e in bundle['arrangements'] if not e.get('read_only')]}, indent=2) + '\n')
    replacements = {'VERSION': version, 'BUILD': build, 'MIN_MACOS': minimum,
                    'MIN_MAJOR': minimum.split('.')[0], 'RESOLVE_VERSION': metadata['tested_resolve']}
    scripts.mkdir()
    for path in (root / 'infrastructure/macos/installer').iterdir():
        if not path.is_file(): continue
        text = path.read_text()
        for key, value in replacements.items(): text = text.replace('@' + key + '@', value)
        target = scripts / path.name; target.write_text(text); target.chmod(0o755)
        if path.name not in ('preinstall', 'postinstall'):
            copy(target, support / path.name)
    tools = payload / 'Applications/Flow DaVinci Tools'; tools.mkdir(parents=True)
    for label, command in [('Exporta suport Flow DaVinci.command', 'export-support.command'),
                           ('Dezinstaleaza Flow DaVinci.command', 'uninstall.command'),
                           ('Alege produsul activ.command', 'choose-active-product.command')]:
        wrapper = tools / label
        wrapper.write_text('#!/bin/bash\nexec /bin/bash "/' + str(SUPPORT / command) + '"\n')
        wrapper.chmod(0o755)
    # One inventory for replacement, uninstall and recovery. No user settings.
    paths = ['/Applications/Flow DaVinci.app', '/' + str(RESOLVE / 'IOPlugins/FlowDaVinci.dvcp.bundle'), '/' + str(SUPPORT)]
    paths += ['/Applications/Flow DaVinci Tools/Exporta suport Flow DaVinci.command', '/Applications/Flow DaVinci Tools/Dezinstaleaza Flow DaVinci.command', '/Applications/Flow DaVinci Tools/Alege produsul activ.command']
    paths += ['/' + str(RESOLVE / 'Workflow Integration Plugins' / name) for name in ['FlowDaVinci.py'] + [e['file'] for e in commands]]
    inventory = '\n'.join(paths) + '\n'
    (scripts / 'paths.txt').write_text(inventory); (support / 'paths.txt').write_text(inventory)
    if inputs.get('signing'):
        sign_code(payload, inputs['signing'])
        metadata['signing_team'] = inputs['signing']['application']['team']
    helper = payload / RESOLVE / 'IOPlugins/FlowDaVinci.dvcp.bundle/Contents/MacOS'
    metadata['source_runtime_sha256'] = inputs.get('sha256', {})
    metadata['distributed_runtime_sha256'] = {name: sha(helper / name) for name in ('FlowDaVinci.dvcp', 'FlowDaVinciWorkspace.dylib')}
    (support / 'release-manifest.json').write_text(json.dumps(metadata, indent=2) + '\n')
    # Hash every regular payload file, including metadata and maintenance tools.
    hashes = ''.join(sha(path) + '\t/' + str(path.relative_to(payload)) + '\n'
                     for path in sorted(payload.rglob('*')) if path.is_file())
    (scripts / 'hashes.tsv').write_text(hashes)
    keep = {'preinstall', 'postinstall', 'transaction.sh', 'rollback.command', 'legacy-loaders.sh', 'paths.txt', 'hashes.tsv'}
    for path in scripts.iterdir():
        if path.name not in keep: path.unlink()
    return metadata
