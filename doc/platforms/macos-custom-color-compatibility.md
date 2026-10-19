# Custom clip-color compatibility contract

User requirement: removing Unbound must never remove a clip from a timeline or
lose its media, timing, edits, transforms, effects or other existing properties.
This is an acceptance requirement, not a claim that custom-color storage works.

## Required representation

- Keep the real Resolve clip and its identity. Do not replace a clip, wrap it in
  a compound/nested clip, create a generator or modify media to represent color.
- A closest supported native Clip Color must be stored on the real clip when
  the custom hue is applied. It must remain meaningful without Unbound. Do not
  depend on an uninstall hook, startup repair or background service to revert it.
- The exact custom hue/name may be additional metadata only after the storage
  path, round trip and compatibility have been verified on disposable projects.
  Do not write undocumented project database fields or invalid palette values.
- Determine fallback from verified native palette colors using a documented
  perceptual distance. Do not assume that a particular named hue is nearest.
- Custom metadata is optional for rendering the native project. A missing plugin,
  missing metadata or unsupported command must leave the native fallback usable.
- If exact hue information cannot safely travel with a shared project, document
  that limitation rather than promising lossless custom-color interchange.
- Desired `.drp` interchange: use a verified, namespaced, versioned project
  metadata representation for custom hue definitions and their clip associations.
  A second Unbound installation should recover the exact hue/name; Resolve
  without Unbound should use the native fallback. Do not assume `.drp` supports
  an arbitrary custom section or that a local sidecar travels with it.
- Validate imported color metadata as data only, including bounds and versions.
  Do not carry executable content, machine-specific paths or unverified clip
  references. Match clips using verified stable identifiers, never timeline row
  positions or media filenames alone.

## Acceptance tests before exposing custom colors

1. Record clip identity, count, media references, source/timeline ranges, edit
   positions and existing properties in a disposable project.
2. Apply custom hues. Verify the native fallback immediately, independently of
   Unbound's display, and verify unchanged clip/edit/property records.
3. Save, close, disable/uninstall Unbound, launch a fresh Resolve and reopen.
   Every clip and edit must remain; fallback colors must render without errors.
4. Repeat project interchange/reopen without Unbound on the receiving system.
5. Reinstall and verify exact hues can return when supported metadata survives.
6. Verify undo/redo, duplication, relinking and unsupported versions preserve
   clips. Partial color-write failure must never trigger structural edits.

No custom-color operation may call timeline/clip deletion or replacement as a
fallback. If compatibility cannot be demonstrated, leave the feature unavailable
and preserve Resolve's existing native palette.

## Executable probe

`infrastructure/macos/probe-red-clip-color.py --output <directory>` checks connectivity only. `--run` additionally requires **no existing project open**, generates silent audio in a uniquely named disposable project, tries Red/hex/lowercase names, saves/reopens each attempt, compares clip identity/ranges/media identity/count, restores a native Orange control and exports the scratch DRP. It never deletes or replaces clips and refuses to switch away from a working project. Calls have a process deadline; a timeout is not a successful storage test.

Native executable inventory contains `Sm2ClipColor`, `SM2_CLIP_COLOR_LAST` bounds assertions and 16 named clip palette resources, with no Red resource among them. That supports an enum-backed palette hypothesis, not proof of every possible internal storage path. No Red option is exposed until a storage/rendering implementation is verified. The current live read-only probe stalled and was stopped; no project writes were requested.
