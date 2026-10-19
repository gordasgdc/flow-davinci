// SPDX-License-Identifier: MIT
#pragma once
#include <QEvent>
#include <algorithm>
#include "nativechrome.h"
namespace Unbound {
// A drag-scoped snapshot, invalidated by ordering/lifetime events and changes to
// the actual windows under the pointer. Release always requests fresh order.
class DockOrderCache {
public:
    void invalidate() { valid=false; }
    void event(QEvent::Type type) {
        switch (type) {
        case QEvent::WindowActivate: case QEvent::WindowDeactivate:
        case QEvent::ApplicationActivate: case QEvent::ApplicationDeactivate:
        case QEvent::ApplicationStateChange: case QEvent::ZOrderChange:
        case QEvent::Show: case QEvent::Hide: case QEvent::WindowStateChange:
        case QEvent::ParentChange: case QEvent::WinIdChange:
            invalidate(); break;
        default: break;
        }
    }
    qint64 front(QList<qint64> windows) {
        std::sort(windows.begin(),windows.end());
        const auto current=nativeWindowOrderRevision();
        if (!valid || windows!=candidates || current!=revision) {
            result=nativeFrontmostWindowNumber(windows); candidates=windows; revision=current; valid=true;
        }
        return result;
    }
private:
    QList<qint64> candidates;
    quint64 revision=0;
    qint64 result=0;
    bool valid=false;
};
}
