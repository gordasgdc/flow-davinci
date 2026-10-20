# Narrow-panel audit, 2026-10-19

Candidate changes only; not installed. Native controls must remain reachable at
narrow widths and increased interface scales. Preserve native list scrolling,
signals and layout constraints; scroll the presentation viewport instead of
forcing native controls below their minimum size.

| Surface | Candidate behavior | Acceptance still required |
| --- | --- | --- |
| Inspector | Existing reversible scroll viewport | Narrow, scale, group/tab, dock, page return |
| Media Pool, Index, Effects, Metadata, Mixer, Sound Library, Keyframes | Shared layout-transfer path now has a reversible scroll viewport; no native minimum-width relaxation | Real native controls, last control pointer hit, nested lists, group/tab, dock, page return |
| Media Pool whole-widget transfer | Scroll viewport also covers this separate path | Real native toolbar overflow |
| Detached Timeline Toolbar | Horizontal overflow, matching Workspace Toolbar | Narrow widths and increased scale |
| Workspace Toolbar | Existing horizontal overflow and compact height | Attached/floating, increased scale |
| Attached Timeline Toolbar | Still native within Timeline | Inspect native row separately; do not wrap custom-painted tracks just to scroll toolbar |
| Source/Program Monitor | Native rendering retained | Narrow header controls, Single/Cinema modes; no blanket scroll wrapper around GPU surface |
| Page Navigation | Existing accessory scrolling | Native F12 synchronization, full workspace width and page return |

Multipart sidebar regression cases cover all seven registered sidebars, the
rightmost button's viewport reachability, retained scroll offset on page hide/show,
and exact native layout, child ownership, minimum and size-policy restoration.
The existing sidebar fixture also checks native signals and all window chrome
modes. Compilation is not execution evidence.

`panel.overflow.range` logs the panel, axis and available range on range changes;
`panel.overflow.scroll` records movement. These establish whether a viewport was
available, not whether every custom native control rendered correctly.

macOS modular registrations currently target Edit. Color, Fairlight and other
pages remain native; validate Edit suspension/return across those pages rather
than claiming modular overflow support for unregistered panels. Resolve is open,
so displayed/native-window fixtures and live acceptance remain pending. Preserve
both saved user arrangements and the preview profile during installation.
