# Flat Panels menu

fix/macos-flat-panels-menu removes This Window plus All Panels duplication. Every
workspace title-bar Panels menu lists the canonical registry's panels directly, once.
Checks and deferred Show/Hide use the same owner callbacks on every surface; no
separate local approximation of visibility remains. Workspace Toolbar is presented
as Workspace Header. Page Navigation is a root visibility toggle; its separate
placement submenu controls only pinning. Existing Show Panel in Workspace remains
a deliberately show-only command surface and uses the same visibility model.

Regression fixture checks direct entries, no nested All Panels, actual hide/show,
state synchronization between menus and preserved show-only command behavior.
Installed baseline remains 091268b; no installation or release.
