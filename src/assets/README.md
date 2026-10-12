# Application icon

Filmhaus logo and derived Unbound icons: Copyright (c) 2026 Wolfgang Woehl.
The original artwork is `filmhaus.png` in this directory.
See the root NOTICE for artwork rights.

The Windows release build regenerates the icons automatically through
`infrastructure/windows/generate-icons.ps1`. The generated Windows resource
`src/platforms/windows/resources/unbound.ico` contains transparent frames at 16, 20, 24, 32, 40, 48, 64, 96,
128 and 256 pixels. It is embedded in the EXE. Individual PNGs and a preview
are included for inspection; they are not required alongside the executable.
