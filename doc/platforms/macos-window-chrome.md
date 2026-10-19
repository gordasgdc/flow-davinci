# Avoid redundant native window writes

Native normalization now writes level/deactivation properties only when values differ. A group cell's clipping flag is written once. Reapplying hidden-background mode no longer orders an already-hidden background out or calls orderFront on every already-visible panel; explicit subsequent hierarchy ordering remains authoritative. Real native state is checked each time, so external changes can still be repaired.

This removes avoidable window-server/style operations without turning off rendering, diagnostics or recovery. It is not proof that all blinking is fixed: the current live trace also records native content Hide events and recovery, which need an observed reproduction. Existing native visibility/stack/focus fixtures must pass before install.
