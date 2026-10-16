// SPDX-License-Identifier: MIT
#pragma once
#include <QAbstractSpinBox>
#include <QAction>
#include <QActionEvent>
#include <QApplication>
#include <QComboBox>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QJsonArray>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QSet>
#include <QShortcutEvent>
#include <QTextEdit>
#include <QTimer>
#include "diagnostics.h"
#include "nativechrome.h"

namespace Unbound {
inline bool textEditor(QWidget *widget)
{
    for (auto parent = widget; parent; parent = parent->parentWidget()) {
        if (qobject_cast<QLineEdit *>(parent) || qobject_cast<QTextEdit *>(parent) ||
            qobject_cast<QPlainTextEdit *>(parent) || qobject_cast<QAbstractSpinBox *>(parent)) return true;
        auto combo = qobject_cast<QComboBox *>(parent);
        if (combo && combo->isEditable()) return true;
        if (parent->isWindow()) break;
    }
    return false;
}

class InteractionDiagnostics : public QObject
{
public:
    explicit InteractionDiagnostics(QObject *parent) : QObject(parent)
    {
        qApp->installEventFilter(this);
        connect(qApp, &QApplication::focusChanged, this, [](QWidget *before, QWidget *after) {
            if (!related(before) && !related(after)) return;
            logEvent(QStringLiteral("focus.changed"), {{"before", widgetState(before)}, {"after", widgetState(after)},
                {"active_window", widgetState(QApplication::activeWindow())}});
        });
        QTimer::singleShot(0, this, [this] { scanActions(); });
    }
    ~InteractionDiagnostics() override { qApp->removeEventFilter(this); }
protected:
    bool eventFilter(QObject *object, QEvent *event) override
    {
        auto widget = qobject_cast<QWidget *>(object);
        if (event->type() == QEvent::ActionAdded && related(widget)) {
            observe(static_cast<QActionEvent *>(event)->action());
        }
        if (event->type() == QEvent::Shortcut && related(QApplication::focusWidget()) && !textEditor(QApplication::focusWidget())) {
            auto shortcut = static_cast<QShortcutEvent *>(event);
            logEvent(QStringLiteral("input.shortcut"), {{"receiver", object->objectName()},
                {"class", object->metaObject()->className()}, {"key", shortcut->key().toString()},
                {"ambiguous", shortcut->isAmbiguous()}, {"focus", widgetState(QApplication::focusWidget())}});
        }
        if (!related(widget)) return false;
        if (event->type() == QEvent::MouseButtonPress) {
            QPointer<QWidget> receiver = widget;
            logEvent(QStringLiteral("input.click"), {{"receiver", widgetState(widget)}, {"focus_before", widgetState(QApplication::focusWidget())}});
            QTimer::singleShot(0, this, [receiver] {
                logEvent(QStringLiteral("input.click.focus-result"), {{"receiver", widgetState(receiver)},
                    {"focus", widgetState(QApplication::focusWidget())}, {"active_window", widgetState(QApplication::activeWindow())}});
            });
        }
        if (event->type() == QEvent::KeyPress || event->type() == QEvent::ShortcutOverride) {
            auto key = static_cast<QKeyEvent *>(event);
            const bool typing = textEditor(widget) || textEditor(QApplication::focusWidget());
            QJsonObject state{{"receiver", widgetState(widget)}, {"focus", widgetState(QApplication::focusWidget())},
                {"active_window", widgetState(QApplication::activeWindow())}, {"text_editing", typing},
                {"event_type", int(event->type())}};
            // Do not record text or key values while the user is editing text.
            if (!typing) { state["key"] = key->key(); state["modifiers"] = int(key->modifiers()); state["repeat"] = key->isAutoRepeat(); }
            logEvent(QStringLiteral("input.key.observed"), state);
        }
        if (widget->isWindow() && event->type() == QEvent::WindowActivate) {
            QTimer::singleShot(0, this, [this] { activationState(); scanActions(); });
        }
        if (widget->isWindow() && event->type() == QEvent::Show && widget->objectName() == QLatin1String("UiMainWindow")) scanActions();
        return false;
    }
private:
    static bool related(QWidget *widget)
    {
        if (!widget) return false;
        const QString name = widget->window()->objectName();
        return name == QLatin1String("UiMainWindow") || name.startsWith(QLatin1String("unboundFloating_"));
    }
    void observe(QAction *action)
    {
        if (!action || tracked.contains(action) || action->shortcuts().isEmpty()) return;
        tracked.insert(action);
        connect(action, &QObject::destroyed, this, [this, action] { tracked.remove(action); });
        connect(action, &QAction::triggered, this, [action] {
            logEvent(QStringLiteral("input.action.triggered"), {{"action", action->objectName()},
                {"label", action->text()}, {"focus", widgetState(QApplication::focusWidget())}});
        });
    }
    void scanActions()
    {
        QJsonArray viewerActions;
        for (auto window : QApplication::topLevelWidgets()) if (window->objectName() == QLatin1String("UiMainWindow"))
            for (auto action : window->findChildren<QAction *>()) {
                observe(action);
                const QString text = action->text().toLower();
                if (text.contains("full screen") || text.contains("fullscreen") || text.contains("cinema viewer"))
                    viewerActions.append(QJsonObject{{"name", action->objectName()}, {"label", action->text()},
                        {"enabled", action->isEnabled()}, {"shortcut", action->shortcut().toString()}});
            }
        if (!viewerActions.isEmpty()) logEvent(QStringLiteral("input.viewer-actions"), {{"actions", viewerActions}});
    }
    void activationState()
    {
        QJsonArray panels;
        for (auto window : QApplication::topLevelWidgets()) {
            if (!window->objectName().startsWith(QLatin1String("unboundFloating_"))) continue;
            const QString name = window->objectName().mid(QStringLiteral("unboundFloating_").size());
            QWidget *content = window->findChild<QWidget *>(name);
            QJsonArray children;
            if (content && (name == "widgetEditSourcePlayer" || name == "widgetEditTimelinePlayer"))
                for (const auto &child : {"frameViewerWidget", "framePlayerControl", "framePlayerTime"})
                    children.append(widgetState(content->findChild<QWidget *>(QLatin1String(child))));
            panels.append(QJsonObject{{"host", widgetState(window)}, {"content", widgetState(content)}, {"viewer_parts", children}});
        }
        logEvent(QStringLiteral("activation.state"), {{"panels", panels}, {"focus", widgetState(QApplication::focusWidget())},
            {"active_window", widgetState(QApplication::activeWindow())}, {"native_key", nativeKeyWindowNumber()}});
    }
    QSet<QAction *> tracked;
};
}
