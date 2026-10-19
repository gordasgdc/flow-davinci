#!/usr/bin/env python3
"""Package the verified installed 0.0.1 runtime, without rebuilding its binaries."""
import hashlib,html,json,os,pathlib,plistlib,shlex,shutil,subprocess,tempfile
root=pathlib.Path(__file__).resolve().parents[2]
def run(*args,**kw): return subprocess.run(args,check=True,**kw)
def git(*args): return subprocess.check_output(['git','-C',str(root),*args],text=True).strip()
def sha(p): return hashlib.sha256(p.read_bytes()).hexdigest()
def copy(a,b): b.parent.mkdir(parents=True,exist_ok=True);run('/usr/bin/ditto',str(a),str(b))
receipt=json.loads((root/'build/macos/install-receipt.json').read_text());runtime=receipt['commit']
if git('status','--porcelain'): raise SystemExit('Commit release packaging before creating the immutable package.')
run('git','-C',str(root),'merge-base','--is-ancestor',runtime,'HEAD')
if git('diff',runtime,'HEAD','--','src'): raise SystemExit('Runtime source changed since installed build.')
verified=json.loads(pathlib.Path(receipt['functional_receipt']).read_text())
if not verified['all_passed'] or not verified['source_unchanged'] or verified['dirty_at_start'] or verified['commit']!=runtime:
 raise SystemExit('Runtime verification does not match installed source.')
source=root/'build/macos/io-helper/Unbound.dvcp.bundle'
for name,expected in receipt['sha256'].items():
 for folder in [source,pathlib.Path(receipt['plugin'])]:
  p=folder/'Contents/MacOS'/name
  if sha(p)!=expected: raise SystemExit('Artifact differs from installed runtime: '+str(p))
  run('/usr/bin/codesign','--verify','--strict',str(p))
commands=root/'build/macos/workflow-commands';entries=json.loads((commands/'manifest.json').read_text())
for entry in entries:
 name=entry['file']
 if pathlib.Path(name).name!=name or sha(commands/name)!=receipt['workflow_command_sha256'][name]: raise SystemExit('Workflow command mismatch.')
output=root/'release/mac/0.0.1-prototype'
if output.exists(): raise SystemExit('Refusing to overwrite immutable release directory: '+str(output))
filename='CineWaltz-OpenGate-0.0.1-Prototype-macOS-arm64.pkg'
with tempfile.TemporaryDirectory(prefix='opengate-package-',dir=root/'build/macos') as temporary:
 work=pathlib.Path(temporary);payload=work/'payload';scripts=work/'scripts';scripts.mkdir();resources=work/'installer-resources';resources.mkdir()
 resolve=payload/'Library/Application Support/Blackmagic Design/DaVinci Resolve'
 copy(source,resolve/'IOPlugins/Unbound.dvcp.bundle')
 app=payload/'Applications/OpenGate.app';copy(root/'build/macos/OpenGate.app',app)
 info=app/'Contents/Info.plist';value=plistlib.loads(info.read_bytes());value.update(CFBundleShortVersionString='0.0.1',CFBundleVersion='1',LSMinimumSystemVersion='27.0');info.write_bytes(plistlib.dumps(value));run('/usr/bin/codesign','--force','--sign','-',str(app));run('/usr/bin/codesign','--verify','--strict',str(app))
 workflow=resolve/'Workflow Integration Plugins';copy(root/'src/platforms/macos/OpenGate.py',workflow/'OpenGate.py')
 for entry in entries: copy(commands/entry['file'],workflow/entry['file'])
 support=payload/'Library/Application Support/CineWaltz/OpenGate';support.mkdir(parents=True)
 bundle=json.loads((root/'src/assets/presets/BundledArrangements.json').read_text());chosen=next(e for e in bundle['arrangements'] if e['id']==bundle['default']);state=dict(chosen['workspace']);state['selected_arrangement']=chosen['id'];presets=support/'Presets';presets.mkdir()
 for name,data in [('Workspace.json',state),('Arrangements.json',{'version':1,'arrangements':bundle['arrangements']})]: (presets/name).write_text(json.dumps(data,indent=2)+'\n')
 copy(root/'LICENSE',support/'LICENSE');copy(root/'doc/releases/opengate-0.0.1-prototype.md',support/'README.md')
 copy(root/'infrastructure/macos/installer/seed-profile.sh',support/'seed-profile.sh')
 for name in ['preinstall','postinstall']: copy(root/'infrastructure/macos/installer'/name,scripts/name);(scripts/name).chmod(0o755)
 paths=['/Applications/OpenGate.app','/Library/Application Support/Blackmagic Design/DaVinci Resolve/IOPlugins/Unbound.dvcp.bundle','/Library/Application Support/Blackmagic Design/DaVinci Resolve/Workflow Integration Plugins/OpenGate.py']+['/Library/Application Support/Blackmagic Design/DaVinci Resolve/Workflow Integration Plugins/'+e['file'] for e in entries]
 uninstall='''#!/bin/bash
set -eu
if /usr/bin/pgrep -x Resolve >/dev/null; then echo 'Save and quit Resolve first.'; exit 1; fi
if [[ "$EUID" -ne 0 ]]; then exec /usr/bin/sudo /bin/bash "$0"; fi
backup="/Library/Application Support/CineWaltz/OpenGate/UninstallBackups/$(/bin/date -u +%Y%m%dT%H%M%SZ)"
/bin/mkdir -p "$backup"
'''
 for i,path in enumerate(paths): uninstall+=f'if [[ -e {shlex.quote(path)} ]]; then /bin/mv {shlex.quote(path)} "$backup/{i}-$(/usr/bin/basename {shlex.quote(path)})"; fi\n'
 uninstall+='''/usr/sbin/pkgutil --forget com.cinewaltz.opengate.prototype >/dev/null 2>&1 || true
echo "OpenGate removed. Files retained in $backup. User settings, projects and native shortcuts were preserved."
'''
 (support/'Uninstall OpenGate.command').write_text(uninstall);(support/'Uninstall OpenGate.command').chmod(0o755)
 meta={'product':'CineWaltz OpenGate','version':'0.0.1 Prototype','runtime_commit':runtime,'packaging_commit':git('rev-parse','HEAD'),'architecture':'arm64','minimum_macos':'27.0','tested_resolve':'21.1.1','runtime_sha256':receipt['sha256'],'fixture_suites':len(verified['tests']),'functional_fixtures_passed':True,'performance_gate':'open; not rerun','notarized':False,'foundation':'Unbound by Wolfgang Woehl (MIT)'}
 (support/'release-manifest.json').write_text(json.dumps(meta,indent=2)+'\n');copy(source/'Contents/Resources/build-manifest.json',support/'runtime-build-manifest.json')
 for script in list(scripts.iterdir())+[support/'seed-profile.sh',support/'Uninstall OpenGate.command']:run('/bin/bash','-n',str(script))
 component=work/'component.plist';run('/usr/bin/pkgbuild','--analyze','--root',str(payload),str(component));items=plistlib.loads(component.read_bytes())
 for item in items:item.update(BundleIsRelocatable=False,BundleIsVersionChecked=False)
 component.write_bytes(plistlib.dumps(items));run('/usr/bin/pkgbuild','--root',str(payload),'--component-plist',str(component),'--scripts',str(scripts),'--ownership','recommended','--identifier','com.cinewaltz.opengate.prototype','--version','0.0.1','--install-location','/',str(work/'component.pkg'))
 (resources/'welcome.html').write_text('<html><body><h1>CineWaltz OpenGate 0.0.1 Prototype</h1><p>Quit Resolve before continuing. Requires Apple Silicon, macOS 27+, Resolve 21 at its standard location. Tested with Studio 21.1.1.</p><p>Includes Native Emulator and two Jordan presets on a fresh profile. Existing settings are preserved and backed up. Custom hotkeys and some arranging behavior remain experimental.</p><p>OpenGate by CineWaltz, based on Unbound by Wolfgang Woehl (MIT). Unsigned and not notarized.</p></body></html>')
 (resources/'conclusion.html').write_text('<html><body><h1>Ready to try OpenGate</h1><p>Open Resolve and an Edit project. Use the OpenGate UI menu. Fresh profiles start with Native Emulator; existing profiles retain their settings.</p><p>Read the included release notes in /Library/Application Support/CineWaltz/OpenGate/. No projects, media or native keyboard assignments were changed.</p></body></html>');copy(root/'LICENSE',resources/'LICENSE.txt')
 distribution=work/'distribution.xml';distribution.write_text('''<?xml version="1.0" encoding="utf-8"?>
<installer-gui-script minSpecVersion="1"><title>CineWaltz OpenGate 0.0.1 Prototype</title>
<options customize="never" require-scripts="true" hostArchitectures="arm64"/>
<domains enable_localSystem="true" enable_currentUserHome="false" enable_anywhere="false"/>
<welcome file="welcome.html"/><license file="LICENSE.txt"/><conclusion file="conclusion.html"/>
<volume-check><allowed-os-versions><os-version min="27.0"/></allowed-os-versions></volume-check>
<choices-outline><line choice="default"/></choices-outline><choice id="default" visible="false"><pkg-ref id="com.cinewaltz.opengate.prototype"/></choice>
<pkg-ref id="com.cinewaltz.opengate.prototype" version="0.0.1" auth="Root">component.pkg</pkg-ref></installer-gui-script>''')
 run('/usr/bin/productbuild','--distribution',str(distribution),'--package-path',str(work),'--resources',str(resources),str(work/filename))
 # Inspect the actual archive before publishing, without changing this installation.
 expanded=work/'expanded';run('/usr/sbin/pkgutil','--expand-full',str(work/filename),str(expanded))
 binary=list(expanded.rglob('UnboundWidgetProbe.dylib'));assert len(binary)==1 and sha(binary[0])==receipt['sha256']['UnboundWidgetProbe.dylib']
 archived=list(expanded.rglob('Unbound.dvcp'));assert len(archived)==1 and sha(archived[0])==receipt['sha256']['Unbound.dvcp']
 assert len(list(expanded.rglob('OpenGate - *.py')))==len(entries)
 archives=list(expanded.rglob('Presets/Arrangements.json'));assert len(archives)==1 and json.loads(archives[0].read_text())['arrangements']==bundle['arrangements']
 output.mkdir(parents=True);copy(work/filename,output/filename);copy(root/'doc/releases/opengate-0.0.1-prototype.md',output/'README.md');(output/'release-manifest.json').write_text(json.dumps(meta,indent=2)+'\n')
 (output/'SHA256SUMS.txt').write_text(''.join(sha(output/name)+'  '+name+'\n' for name in [filename,'README.md','release-manifest.json']))
 print('PACKAGE VERIFIED:',output/filename)
