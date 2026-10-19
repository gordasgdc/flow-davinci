# Custom hue metadata probe

The installed scripting SDK declares `MediaPoolItem.GetThirdPartyMetadata(key)` and `SetThirdPartyMetadata(dict)`. This is a storage candidate, not an arbitrary-color rendering API.

`probe-red-clip-color.py --metadata --run --output <scratch-output>` uses the existing guarded disposable-project path and generated silence. It refuses to switch, save or mutate an already-open project. `clipcolormetadataprobe.py` owns the namespaced payload/key and readback. The caller owns project creation, save/close/reopen, clip-count/identity/start/end/duration checks, supported native control color and DRP export. A 30-second worker deadline and receipts retain failures. Do not run against an editing project.

The probe writes one additive `com.cinewaltz.opengate.clip-color-probe.v1` metadata key with Bright Red/#FF0000 and an Orange native control fallback. Orange is a test control, not a claimed nearest-color production mapping. Reopen verifies exact payload and unchanged clip structure. It never deletes or replaces clips.

Open-project refusal and Python compilation pass. Live storage execution has not run. DRP import/metadata portability, per-timeline-instance identity (metadata is on source media), rendering, undo and nearest native fallback remain unverified. No Red/custom-color UI is exposed by this branch. Users without OpenGate would still need ordinary supported native clip values; metadata alone cannot render the hue.
