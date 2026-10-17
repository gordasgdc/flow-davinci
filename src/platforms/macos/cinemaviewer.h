// SPDX-License-Identifier: MIT
#pragma once
#include <QApplication>
#include <QPointer>
#include <QTimer>
#include "diagnostics.h"

namespace Unbound {
inline QWidget *visibleCinemaViewer()
{
    for (auto window : QApplication::topLevelWidgets())
        if (window->isVisible() && window->inherits("UiCinemaViewerDialog")) return window;
    return nullptr;
}

// Native Cinema Viewer owns foreground input until it closes. It is not panel expansion.
class CinemaViewerLifecycle : public QObject
{
public:
    explicit CinemaViewerLifecycle(QObject *owner) : QObject(owner) { qApp->installEventFilter(this); }
    ~CinemaViewerLifecycle() override { if (qApp) qApp->removeEventFilter(this); }
protected:
    bool eventFilter(QObject *object, QEvent *event) override
    {
        auto viewer = qobject_cast<QWidget *>(object);
        if (!viewer || !viewer->isWindow() || !viewer->inherits("UiCinemaViewerDialog")) return false;
        if (event->type() == QEvent::Show) {
            QPointer<QWidget> alive(viewer);
            QTimer::singleShot(0, this, [alive] {
                if (!alive || !alive->isVisible()) return;
                alive->raise(); alive->activateWindow();
                logEvent(QStringLiteral("cinema.foreground.request"), {{"window", alive->objectName()}});
            });
        }
        return false;
    }
};
}
