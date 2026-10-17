// SPDX-License-Identifier: MIT
#pragma once
#include <QElapsedTimer>
#include "diagnostics.h"
namespace Unbound {
class DragTiming
{
public:
    class Sample {
    public:
        explicit Sample(DragTiming &owner) : owner(owner) { timer.start(); }
        ~Sample() { const auto ns = timer.nsecsElapsed(); ++owner.count; owner.total += ns; owner.maximum = qMax(owner.maximum, ns); }
    private:
        DragTiming &owner;
        QElapsedTimer timer;
    };
    void begin() { count = 0; total = maximum = 0; }
    void report(const QString &window) const
    {
        logEvent(QStringLiteral("drag.timing"), {{"window", window}, {"preview_updates", count},
            {"preview_mean_ms", count ? double(total)/count/1000000 : 0}, {"preview_max_ms", double(maximum)/1000000}});
    }
private:
    int count = 0;
    qint64 total = 0, maximum = 0;
};
}
