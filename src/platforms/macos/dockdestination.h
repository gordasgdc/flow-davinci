// SPDX-License-Identifier: MIT
#pragma once
#include <QApplication>
#include <QHash>
#include "nativechrome.h"
namespace Unbound {
struct DockDestination { QWidget *surface = nullptr; QRect frame; };
inline DockDestination resolveDockDestination(QWidget *source, QPoint pointer, const QList<QWidget *> &overlays)
{
    QWidget *destination = nullptr;
    QHash<qint64, QWidget *> windows;
    for (auto candidate : QApplication::topLevelWidgets()) {
        if (candidate == source || overlays.contains(candidate) || !candidate->isVisible() || candidate->isMinimized()) continue;
        windows.insert(nativeWindowNumber(candidate), candidate);
    }
    for (auto number : nativeOrderedWindowNumbers()) {
        auto hit = windows.value(number);
        if (!hit || !hit->frameGeometry().contains(pointer)) continue;
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
