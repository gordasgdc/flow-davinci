// SPDX-License-Identifier: MIT
#pragma once
#include <QRect>
#include <QList>
#include <cstdlib>

namespace Unbound {
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
