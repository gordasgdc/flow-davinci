// SPDX-License-Identifier: MIT
#pragma once
#include <QRect>
#include <QList>
#include <QLine>
#include <cstdlib>

namespace Unbound {
struct DockZone { QRect target, preview; Qt::Edge edge = Qt::LeftEdge; bool active = false; };
inline DockZone localDockZone(const QRect &frame, QPoint pointer)
{
    DockZone result;
    if (!frame.adjusted(-18, -18, 18, 18).contains(pointer)) return result;
    const int distances[] = {std::abs(pointer.x() - frame.left()), std::abs(pointer.x() - frame.right()),
        std::abs(pointer.y() - frame.top()), std::abs(pointer.y() - frame.bottom())};
    int closest = 0; for (int i = 1; i < 4; ++i) if (distances[i] < distances[closest]) closest = i;
    if (distances[closest] > 18) return result;
    const QPoint center(closest == 0 ? frame.left() : closest == 1 ? frame.right() : qBound(frame.left() + 9, pointer.x(), qMax(frame.left() + 9, frame.right() - 9)),
        closest == 2 ? frame.top() : closest == 3 ? frame.bottom() : qBound(frame.top() + 9, pointer.y(), qMax(frame.top() + 9, frame.bottom() - 9)));
    result.target = QRect(center - QPoint(9, 9), QSize(18, 18));
    result.edge = closest == 0 ? Qt::LeftEdge : closest == 1 ? Qt::RightEdge : closest == 2 ? Qt::TopEdge : Qt::BottomEdge;
    result.preview = frame;
    if (closest < 2) { result.preview.setWidth(frame.width() / 2); if (closest == 1) result.preview.moveRight(frame.right()); }
    else { result.preview.setHeight(frame.height() / 2); if (closest == 3) result.preview.moveBottom(frame.bottom()); }
    result.active = result.target.contains(pointer); return result;
}
inline QRect alignWindowEdges(QRect frame, const QList<QRect> &panels, QList<QLine> &guides, int threshold = 6)
{
    int bestX = threshold + 1, bestY = threshold + 1, dx = 0, dy = 0;
    QLine vertical, horizontal;
    for (const auto &target : panels) {
        for (int source : {frame.left(), frame.right()}) for (int edge : {target.left(), target.right()}) {
            if ((source == frame.left()) != (edge == target.left()) && frame.bottom() >= target.top() && frame.top() <= target.bottom()) continue;
            const int distance = std::abs(edge - source);
            if (distance < bestX) { bestX = distance; dx = edge - source;
                vertical = QLine(edge, qMin(frame.top(), target.top()), edge, qMax(frame.bottom(), target.bottom())); }
        }
        for (int source : {frame.top(), frame.bottom()}) for (int edge : {target.top(), target.bottom()}) {
            if ((source == frame.top()) != (edge == target.top()) && frame.right() >= target.left() && frame.left() <= target.right()) continue;
            const int distance = std::abs(edge - source);
            if (distance < bestY) { bestY = distance; dy = edge - source;
                horizontal = QLine(qMin(frame.left(), target.left()), edge, qMax(frame.right(), target.right()), edge); }
        }
    }
    if (bestX <= threshold) guides.append(vertical);
    if (bestY <= threshold) guides.append(horizontal);
    return frame.translated(dx, dy);
}
inline QRect snapWindowFrame(QRect frame, const QList<QRect> &panels, const QList<QRect> &displays, int threshold = 12)
{
    int dx = 0, dy = 0, bestX = threshold + 1, bestY = threshold + 1;
    const auto x = [&](int offset) { if (std::abs(offset) < bestX) { bestX = std::abs(offset); dx = offset; } };
    const auto y = [&](int offset) { if (std::abs(offset) < bestY) { bestY = std::abs(offset); dy = offset; } };
    for (const auto &target : panels) {
        if (frame.bottom() >= target.top() - threshold && frame.top() <= target.bottom() + threshold) {
            x(target.right() + 1 - frame.left()); x(target.left() - frame.right() - 1);
            x(target.left() - frame.left()); x(target.right() - frame.right());
        }
        if (frame.right() >= target.left() - threshold && frame.left() <= target.right() + threshold) {
            y(target.bottom() + 1 - frame.top()); y(target.top() - frame.bottom() - 1);
            y(target.top() - frame.top()); y(target.bottom() - frame.bottom());
        }
    }
    for (const auto &display : displays) if (display.adjusted(-threshold, -threshold, threshold, threshold).intersects(frame)) {
        x(display.left() - frame.left()); x(display.right() - frame.right());
        y(display.top() - frame.top()); y(display.bottom() - frame.bottom());
    }
    return frame.translated(dx, dy);
}
}
