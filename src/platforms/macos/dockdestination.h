// SPDX-License-Identifier: MIT
#pragma once
#include <QApplication>
#include "nativechrome.h"
namespace Unbound {
struct DockDestination { QWidget *surface = nullptr; QRect frame; };
inline DockDestination resolveDockDestination(QWidget *source, QPoint pointer, const QList<QWidget *> &overlays)
{
    QWidget *destination = nullptr;
    const auto windows = QApplication::topLevelWidgets();
    for (auto number : nativeOrderedWindowNumbers()) {
        QWidget *hit = nullptr;
        for (auto candidate : windows) {
            if (candidate == source || overlays.contains(candidate) || !candidate->isVisible() ||
                candidate->isMinimized() || nativeWindowNumber(candidate) != number) continue;
            if (candidate->frameGeometry().contains(pointer)) { hit = candidate; break; }
        }
        if (!hit) continue;
        if (hit->objectName().startsWith("unboundFloating_")) destination = hit;
        break;
    }
    if (!destination) return {};
    QList<QWidget *> surfaces;
    for (auto child : destination->findChildren<QWidget *>())
        if (child->objectName().startsWith("unboundGroupCell_") && child->isVisible()) surfaces.append(child);
    if (surfaces.isEmpty()) return {destination, destination->frameGeometry()};
    const auto outer = destination->frameGeometry();
    if (outer.contains(pointer) && !outer.adjusted(10,10,-10,-10).contains(pointer)) return {destination, outer};
    DockDestination result;
    for (auto content : surfaces) {
        // Each cell has its own wrapper, including its drag header.
        auto leaf = content->parentWidget();
        const QRect frame(leaf->mapToGlobal(QPoint()), leaf->size());
        if (frame.contains(pointer) && (!result.surface || frame.width()*frame.height() < result.frame.width()*result.frame.height()))
            result = {content, frame};
    }
    return result;
}
}
