// SPDX-License-Identifier: MIT
#pragma once
#include <QApplication>
#include <QHash>
#include "nativechrome.h"
#include "docksurfaces.h"
#include "nativedocksurfaces.h"
namespace Unbound {
struct DockDestination { QWidget *surface = nullptr; QRect frame; };
inline DockDestination resolveDockDestination(QWidget *source, QPoint pointer, const QList<QWidget *> &overlays)
{
    QWidget *destination = nullptr;
    QHash<qint64, QWidget *> windows;
    QList<QWidget *> underPointer; bool possible = false;
    for (auto candidate : QApplication::topLevelWidgets()) {
        if (candidate == source || overlays.contains(candidate) || !candidate->isVisible() || candidate->isMinimized()) continue;
        if (!candidate->frameGeometry().contains(pointer)) continue;
        underPointer.append(candidate); possible |= candidate->objectName().startsWith("unboundFloating_") || !nativeDockSurfaces(candidate).isEmpty();
    }
    // Free dragging needs no WindowServer query. When a target is possible we
    // still use the current native stack, including non-dockable occluders.
    if (!possible) return {};
    for (auto candidate : underPointer) windows.insert(nativeWindowNumber(candidate),candidate);
    for (auto number : nativeOrderedWindowNumbers()) {
        auto hit = windows.value(number);
        if (!hit || !hit->frameGeometry().contains(pointer)) continue;
        if (hit->objectName().startsWith("unboundFloating_") || !nativeDockSurfaces(hit).isEmpty()) destination = hit;
        break;
    }
    if (!destination) return {};
    if (!destination->objectName().startsWith("unboundFloating_")) {
        DockDestination result;
        for (auto cell : nativeDockSurfaces(destination)) {
            const QRect frame(cell->mapToGlobal(QPoint()),cell->size());
            if (frame.contains(pointer) && (!result.surface || qint64(frame.width())*frame.height()<qint64(result.frame.width())*result.frame.height()))
                result={cell,frame};
        }
        return result;
    }
    const auto surfaces = dockSurfaces(destination);
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
