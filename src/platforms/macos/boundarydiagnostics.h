// SPDX-License-Identifier: MIT
#pragma once
#include <QApplication>
#include <QPointer>
#include <QSet>
#include <QTimer>
#include "diagnostics.h"
namespace Unbound {
// Inspect only registered group cells after layout settles. No global widget scan.
class BoundaryDiagnostics : public QObject {
public:
    explicit BoundaryDiagnostics(QObject *owner) : QObject(owner) { qApp->installEventFilter(this); }
    ~BoundaryDiagnostics() override { if (qApp) qApp->removeEventFilter(this); }
protected:
    bool eventFilter(QObject *object,QEvent *event) override {
        if (event->type() != QEvent::Resize && event->type() != QEvent::Show && event->type() != QEvent::LayoutRequest) return false;
        auto widget = qobject_cast<QWidget *>(object);
        if (!widget || !widget->objectName().startsWith("unboundGroupCell_") || pending.contains(widget)) return false;
        pending.insert(widget); QPointer<QWidget> alive(widget);
        QTimer::singleShot(50,this,[this,alive,widget] {
            pending.remove(widget); if (!alive || !alive->isVisible()) return;
            QJsonArray overflow; int checked=0;
            for (auto child : alive->findChildren<QWidget *>()) {
                if (!child->isVisible() || child->isWindow()) continue;
                ++checked;
                const QRect bounds(child->mapTo(alive,QPoint()),child->size());
                if (alive->rect().contains(bounds)) continue;
                if (overflow.size() < 16) overflow.append(QJsonObject{{"object",child->objectName()},
                    {"class",child->metaObject()->className()},{"x",bounds.x()},{"y",bounds.y()},
                    {"width",bounds.width()},{"height",bounds.height()},
                    {"native",child->testAttribute(Qt::WA_NativeWindow)},
                    {"paint_on_screen",child->testAttribute(Qt::WA_PaintOnScreen)}});
            }
            logEvent("panel.boundary.audit",{{"cell",alive->objectName()},{"width",alive->width()},
                {"height",alive->height()},{"descendants_checked",checked},{"overflow",overflow},
                {"scope","Geometry outside cell; does not by itself prove visible paint leakage"}});
        });
        return false;
    }
private:
    QSet<QWidget *> pending;
};
}
