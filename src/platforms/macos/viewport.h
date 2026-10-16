// SPDX-License-Identifier: MIT
#pragma once
#include <QGuiApplication>
#include <QScreen>
#include <QWidget>
#include <limits>

namespace Unbound {
inline QRect fitToDisplays(QRect desired, const QList<QRect> &displays)
{
    if (displays.isEmpty()) return desired;
    QRect best = displays.first();
    qint64 bestArea = -1, bestDistance = std::numeric_limits<qint64>::max();
    for (const QRect &display : displays) {
        const QRect intersection = desired.intersected(display);
        const qint64 area = qint64(intersection.width()) * intersection.height();
        const QPoint delta = desired.center() - display.center();
        const qint64 distance = qint64(delta.x()) * delta.x() + qint64(delta.y()) * delta.y();
        if (area > bestArea || (area == bestArea && distance < bestDistance)) {
            best = display; bestArea = area; bestDistance = distance;
        }
    }
    desired.setSize(desired.size().boundedTo(best.size()).expandedTo(QSize(1, 1)));
    desired.moveLeft(qBound(best.left(), desired.left(), best.right() - desired.width() + 1));
    desired.moveTop(qBound(best.top(), desired.top(), best.bottom() - desired.height() + 1));
    return desired;
}

inline void keepOnDisplay(QWidget *window)
{
    if (!window) return;
    QList<QRect> areas;
    for (auto screen : QGuiApplication::screens()) areas.append(screen->availableGeometry());
    const QRect frame = window->frameGeometry();
    const QRect fitted = fitToDisplays(frame, areas);
    if (frame == fitted) return;
    const QSize decoration = frame.size() - window->size();
    window->resize((fitted.size() - decoration).expandedTo(QSize(1, 1)));
    window->move(fitted.topLeft());
}
}
