// SPDX-License-Identifier: MIT
#pragma once
#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QWidget>

#ifndef UNBOUND_BUILD_ID
#define UNBOUND_BUILD_ID "development"
#endif

namespace Unbound {
inline void logEvent(const QString &event, QJsonObject details = {})
{
    const QString directory = qEnvironmentVariable("UNBOUND_DIAGNOSTICS_DIR",
        QDir::homePath() + QStringLiteral("/Library/Logs/Unbound"));
    if (!QDir().mkpath(directory)) return;
    const QString path = directory + QStringLiteral("/events.jsonl");
    if (QFile(path).size() > 8 * 1024 * 1024) {
        QFile::remove(path + QStringLiteral(".1"));
        QFile::rename(path, path + QStringLiteral(".1"));
    }
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append)) return;
    details.insert(QStringLiteral("component"), QStringLiteral("Unbound"));
    details.insert(QStringLiteral("module"), QStringLiteral("UnboundWidgetProbe.dylib"));
    details.insert(QStringLiteral("event"), event);
    details.insert(QStringLiteral("time"), QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));
    details.insert(QStringLiteral("pid"), QCoreApplication::applicationPid());
    details.insert(QStringLiteral("build"), QStringLiteral(UNBOUND_BUILD_ID));
    details.insert(QStringLiteral("process"), QCoreApplication::applicationName());
    file.write(QJsonDocument(details).toJson(QJsonDocument::Compact) + '\n');
}

inline QJsonObject widgetState(QWidget *widget)
{
    if (!widget) return {{"exists", false}};
    const QRect r = widget->geometry();
    return {{"exists", true}, {"name", widget->objectName()},
        {"class", widget->metaObject()->className()},
        {"parent", widget->parentWidget() ? widget->parentWidget()->objectName() : QString()},
        {"window", widget->isWindow()}, {"visible", widget->isVisible()},
        {"hidden", widget->isHidden()}, {"enabled", widget->isEnabled()},
        {"focus_policy", int(widget->focusPolicy())}, {"window_type", int(widget->windowType())},
        {"focus_proxy", widget->focusProxy() ? widget->focusProxy()->objectName() : QString()},
        {"x", r.x()}, {"y", r.y()}, {"width", r.width()}, {"height", r.height()}};
}

class WindowDiagnostics : public QObject
{
public:
    explicit WindowDiagnostics(QObject *parent) : QObject(parent) {}
protected:
    bool eventFilter(QObject *object, QEvent *event) override
    {
        auto widget = qobject_cast<QWidget *>(object);
        if (!widget || !widget->isWindow()) return false;
        const QString name = widget->objectName();
        if (name != QLatin1String("UiMainWindow") && name != QLatin1String("unboundPanelTest") &&
            !name.startsWith(QLatin1String("unboundFloating_"))) return false;
        const char *action = nullptr;
        switch (event->type()) {
        case QEvent::Move: action = "window.move"; break;
        case QEvent::Resize: action = "window.resize"; break;
        case QEvent::Show: action = "window.show"; break;
        case QEvent::Hide: action = "window.hide"; break;
        case QEvent::Close: action = "window.close.request"; break;
        case QEvent::WindowStateChange: action = "window.state"; break;
        case QEvent::WindowActivate: action = "window.activate"; break;
        case QEvent::WindowDeactivate: action = "window.deactivate"; break;
        default: return false;
        }
        auto state = widgetState(widget);
        state.insert(QStringLiteral("state"), int(widget->windowState()));
        logEvent(QLatin1String(action), state);
        return false;
    }
};
}
