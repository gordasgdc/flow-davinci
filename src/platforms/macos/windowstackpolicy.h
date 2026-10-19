// SPDX-License-Identifier: MIT
#pragma once
#include <QWidget>
namespace Unbound {
inline int workspaceWindowTier(QWidget *window) {
    if (!window) return 0;
    if (window->property("unboundPinned").toBool()) return 4;
    // Group names describe their creation, not their current membership.
    // A Timeline workspace remains below utility windows after merges/tear-offs.
    if (window->objectName()=="unboundFloating_frameConformTimelines" ||
        window->findChild<QWidget *>("frameConformTimelines")) return 0;
    if (window->objectName()=="unboundFloating_widgetEditInspector" ||
        window->findChild<QWidget *>("widgetEditInspector")) return 2;
    if (window->objectName().startsWith("unboundFloating_group_") ||
        window->objectName()=="unboundFloating_widgetEditSourcePlayer" ||
        window->objectName()=="unboundFloating_widgetEditTimelinePlayer") return 1;
    return 2;
}
}
