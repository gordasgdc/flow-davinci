#!/usr/bin/env python3
"""Build individual Resolve Workflow Integration shortcut targets (no install)."""
import json
from pathlib import Path
root=Path(__file__).resolve().parents[2]
output=root/'build/macos/workflow-commands'
output.mkdir(parents=True,exist_ok=True)
panels=[('frameConformTimelines','Timeline'),('widgetEditInspector','Inspector'),('widgetEditSourcePlayer','Source Monitor'),
        ('widgetEditTimelinePlayer','Program Monitor'),('frameEditPageMediaPool','Media Pool'),('frameEditPageEditIndex','Index'),
        ('frameEditPageEffectsLibrary','Effects'),('frameEditPageMetadata','Metadata'),('frameEditAudioMeter','Mixer'),
        ('frameEditPageSoundLibrary','Sound Library'),('frameEditPageKeyframeEditor','Keyframes'),
        ('frameEditToolbarTop','Timeline Toolbar'),('unboundWorkspaceToolbar','Workspace Toolbar')]
commands=[('panel.toggle.'+id,'Show-Hide '+label) for id,label in panels]
commands += [('panel.expand.'+id,'Maximize-Restore '+label) for id,label in panels]
commands += [('panel.expand.hovered','Maximize-Restore Hovered Panel'),('panel.toggle.page-navigation','Show-Hide Page Navigation'),
             ('edit.timeline.fixed-playhead','Edit - Toggle Fixed Playhead'),('edit.inspector.stabilize','Edit - Stabilize')]
manifest=[]
for command,label in commands:
    filename='OpenGate - '+label+'.py'
    source='''# OpenGate by CineWaltz; native Workflow Integration command bridge.
import ctypes, json, os, pathlib, time, uuid
base=pathlib.Path(os.environ.get("UNBOUND_LAUNCH_DIRECTORY",str(pathlib.Path.home()/"Library/Logs/Unbound")))
directory=base/"command-requests"
directory.mkdir(parents=True,exist_ok=True)
token=str(uuid.uuid4())
temporary=directory/(token+".tmp")
temporary.write_text(json.dumps({"command":COMMAND,"issued_ms":int(time.time()*1000)}))
os.replace(temporary,directory/(token+".json"))
# Wake the GUI main queue immediately; directory watching remains a fallback.
try:
    post=ctypes.CDLL(None).notify_post
    post.argtypes=[ctypes.c_char_p]
    post.restype=ctypes.c_uint32
    post(b"com.cinewaltz.opengate.command-request")
except (AttributeError,OSError):
    pass
'''.replace('COMMAND',repr(command))
    (output/filename).write_text(source)
    manifest.append(dict(command=command,label='OpenGate - '+label,file=filename))
(output/'manifest.json').write_text(json.dumps(manifest,indent=2)+'\n')
print('Built',len(manifest),'individual OpenGate workflow command targets:',output)
