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
            logEvent("panel.header.drag.cancelled",{{"panel",pending},{"reason","host-not-ready"}}); pending.clear(); delete releaseWatch.data();
        });
    }
    void begin(const QString &id) {
        pending=id; ++generation; deadline.start(); released=false;
        delete releaseWatch.data();
        releaseWatch=watchNativePointerRelease(this,[this](QPoint point,bool) { released=true; releasePoint=point; });
        logEvent("panel.header.drag.request",{{"panel",id}});
    }
    void ready(const QString &id,MacFloatingHost *host) {
        if (id!=pending || !host) return;
        const auto token=generation; QPointer<MacFloatingHost> alive(host);
        // finishUndock must finish applying saved geometry before capture begins.
        QTimer::singleShot(0,this,[this,id,alive,token] {
            if (token!=generation || pending!=id || !alive) return;
            deadline.stop(); pending.clear(); delete releaseWatch.data();
            if (QGuiApplication::applicationState()!=Qt::ApplicationActive) {
                logEvent("panel.header.drag.cancelled",{{"panel",id},{"reason","application-inactive"}}); return;
            }
            const auto pointer=released ? releasePoint : QCursor::pos();
            alive->move(pointer-QPoint(80,12));
            if (released || !QApplication::mouseButtons().testFlag(Qt::LeftButton)) {
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
    QPointer<QObject> releaseWatch; QPoint releasePoint; bool released=false;
};
}
