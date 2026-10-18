// SPDX-License-Identifier: MIT
#pragma once
#include <QApplication>
#include <QCursor>
#include <QPointer>
#include <QTimer>
#include "floatinghost.h"
namespace Unbound {
// Readiness comes from session completion, not a fixed wait before moving.
class NativePanelDrag : public QObject {
public:
    explicit NativePanelDrag(QObject *owner) : QObject(owner) {
        deadline.setSingleShot(true); deadline.setInterval(1000);
        connect(&deadline,&QTimer::timeout,this,[this] {
            logEvent("panel.header.drag.cancelled",{{"panel",pending},{"reason","host-not-ready"}}); pending.clear();
        });
    }
    void begin(const QString &id) {
        pending=id; ++generation; deadline.start();
        logEvent("panel.header.drag.request",{{"panel",id}});
    }
    void ready(const QString &id,MacFloatingHost *host) {
        if (id!=pending || !host) return;
        const auto token=generation; QPointer<MacFloatingHost> alive(host);
        // finishUndock must finish applying saved geometry before capture begins.
        QTimer::singleShot(0,this,[this,id,alive,token] {
            if (token!=generation || pending!=id || !alive) return;
            deadline.stop(); pending.clear();
            const auto pointer=QCursor::pos();
            alive->move(pointer-QPoint(80,12));
            if (!QApplication::mouseButtons().testFlag(Qt::LeftButton)) {
                logEvent("panel.header.drag.completed",{{"panel",id},{"result","floated-after-release"},
                    {"x",pointer.x()},{"y",pointer.y()}}); return;
            }
            alive->raise(); alive->activateWindow();
            alive->beginPointerDrag(pointer,true);
            logEvent("panel.header.drag.started",{{"panel",id},{"x",pointer.x()},{"y",pointer.y()}});
        });
    }
private:
    QString pending; QTimer deadline; unsigned generation=0;
};
}
