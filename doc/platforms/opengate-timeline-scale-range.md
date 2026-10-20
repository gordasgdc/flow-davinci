# Timeline scaling range

experiment/macos-timeline-scale-range raises the existing native font/control/layout
scale range from 70–150 to 70–300 percent. Presets include 175, 200, 225, 250, 275 and
300; the slider and session validation share the range. Native fonts render at their
requested size; repeated changes derive from original metrics. There is no screenshot
or input-coordinate transform. Native icon requests grow; sharp icon results still
depend on the source artwork exposing a matching or vector representation.

Native-metric fixture passes through 300%, exact restoration and original action
signals. This does NOT implement scaling of Resolve's custom-painted timeline tracks
or ruler. That separate requirement remains open. A raster enlargement is not an
acceptable substitute: it would compromise sharpness and editing hit tests.

The timeline body receives input as BmdDavUI::UiGraphicsViewportWidget. Our public
metric adapter changes Qt widgets, not that renderer's private drawing constants.
A supported per-timeline renderer scale API has not been established. Further native
capability inspection is required before applying a transform. Do not advertise
full-body scaling as finished or remove the existing scope tooltip. No installation.
