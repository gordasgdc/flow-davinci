// SPDX-License-Identifier: MIT
#pragma once
#include <QApplication>
#include <QMetaMethod>
#include <QSignalMapper>
#include <QSet>
#include <QTimer>
#include "diagnostics.h"
#include "../platform/diagnosticplatform.h"

namespace Unbound {
// Observe only runtime-confirmed signals. Never invoke a private player method.
class ViewerDiagnostics : public QObject
{
public:
    explicit ViewerDiagnostics(QObject *parent) : QObject(parent)
    {
        focusSnapshot.setSingleShot(true); focusSnapshot.setInterval(100);
        focusSettled.setSingleShot(true); focusSettled.setInterval(120);
        connect(&focusSnapshot,&QTimer::timeout,this,[this] { capture("native-focus-coalesced",lastFocusSignal); });
        connect(&focusSettled,&QTimer::timeout,this,[this] { capture("native-focus-settled",lastFocusSignal); });
    }
    void scan()
    {
        for (auto widget : QApplication::allWidgets()) {
            if ((widget->objectName() != "widgetEditSourcePlayer" && widget->objectName() != "widgetEditTimelinePlayer") || players.contains(widget)) continue;
            players.insert(widget);
            connect(widget, &QObject::destroyed, this, [this, widget] { players.remove(widget); });
            QJsonArray inventory;
            const auto meta = widget->metaObject();
            const QSet<QByteArray> observedSignals{"ChangePlayerCinemaMode", "ReplaceCinemaViewer", "IsActiveChanged", "FocusReceived"};
            for (int index = 0; index < meta->methodCount(); ++index) {
                const auto method = meta->method(index);
                if (!observedSignals.contains(method.name()) || method.methodType() != QMetaMethod::Signal || (method.attributes() & QMetaMethod::Cloned)) continue;
                auto mapper = new QSignalMapper(widget);
                const QString identity = widget->objectName() + ':' + QString::fromLatin1(method.methodSignature());
                mapper->setMapping(widget, identity);
                const auto slot = mapper->metaObject()->method(mapper->metaObject()->indexOfSlot("map()"));
                const bool connected = bool(QObject::connect(widget, method, mapper, slot, Qt::AutoConnection));
                connect(mapper, &QSignalMapper::mappedString, this,
                    [this](const QString &signal) {
                        if (!signal.contains("CinemaViewer") && !signal.contains("CinemaMode")) {
                            // Keep every signal identity, but do not serialize the entire
                            // window/player tree repeatedly while focus is changing.
                            logEvent("viewer.native-signal",{{"signal",signal}});
                            lastFocusSignal = signal;
                            if (!focusSnapshot.isActive()) focusSnapshot.start();
                            focusSettled.start(); return;
                        }
                        capture("native-player-signal", signal);
                        QTimer::singleShot(120, this, [this, signal] { capture("native-player-settled", signal); });
                    });
                inventory.append(QJsonObject{{"signal", QString::fromLatin1(method.methodSignature())}, {"connected", connected}});
            }
            logEvent(QStringLiteral("viewer.native-signals"), {{"player", widgetState(widget)}, {"signals", inventory}});
        }
    }
    void capture(const QString &phase, const QString &command)
    {
        QJsonArray windows, viewers;
        for (auto window : QApplication::topLevelWidgets()) if (window->isVisible()) {
            auto state = widgetState(window); state["state"] = int(window->windowState());
            state["native_window"] = diagnosticNativeWindowNumber(window); windows.append(state);
        }
        for (auto widget : players) {
            QJsonArray ancestry;
            for (auto parent = widget->parentWidget(); parent && ancestry.size() < 10; parent = parent->parentWidget()) ancestry.append(widgetState(parent));
            viewers.append(QJsonObject{{"player", widgetState(widget)}, {"ancestry", ancestry}});
        }
        logEvent(QStringLiteral("viewer.cinema.context"), {{"phase", phase}, {"command", command}, {"windows", windows},
            {"players", viewers}, {"focus", widgetState(QApplication::focusWidget())}, {"native_key", diagnosticNativeKeyWindowNumber()}});
    }
private:
    QSet<QWidget *> players;
    QTimer focusSnapshot,focusSettled;
    QString lastFocusSignal;
};
}
