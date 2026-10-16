// SPDX-License-Identifier: MIT
#pragma once
#include <QAbstractSpinBox>
#include <QAbstractButton>
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
#include <QShortcut>
#include <QTextEdit>
#include <QTimer>
#include "diagnostics.h"
#include "nativechrome.h"
#include "viewerdiagnostics.h"

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
        viewerDiagnostics = new ViewerDiagnostics(this);
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
                {"owner", object->parent() ? object->parent()->objectName() : QString()},
                {"ambiguous", shortcut->isAmbiguous()}, {"focus", widgetState(QApplication::focusWidget())}});
        }
        if (related(widget) && (event->type() == QEvent::Show || event->type() == QEvent::Polish ||
            event->type() == QEvent::MouseButtonPress || event->type() == QEvent::KeyPress))
            observeButton(qobject_cast<QAbstractButton *>(widget));
        if (!related(widget)) return false;
        if (event->type() == QEvent::MouseButtonPress) {
            QPointer<QWidget> receiver = widget;
            logEvent(QStringLiteral("input.click"), {{"receiver", widgetState(widget)}, {"focus_before", widgetState(QApplication::focusWidget())}});
            QTimer::singleShot(0, this, [receiver] {
                logEvent(QStringLiteral("input.click.focus-result"), {{"receiver", widgetState(receiver)},
                    {"focus", widgetState(QApplication::focusWidget())}, {"active_window", widgetState(QApplication::activeWindow())}});
            });
            auto button = qobject_cast<QAbstractButton *>(widget);
            if (button && (button->text().trimmed() == "Stabilize" || button->text().trimmed() == "Stabilization")) stabilizationContext(button);
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
            if (!typing && event->type() == QEvent::KeyPress && widget == QApplication::focusWidget())
                commandContext(QKeySequence(int(key->modifiers()) | key->key()));
        }
        if (widget->isWindow() && event->type() == QEvent::WindowActivate) {
            QTimer::singleShot(0, this, [this] { activationState(); scanActions(); });
        }
        if (widget->isWindow() && event->type() == QEvent::Show && widget->objectName() == QLatin1String("UiMainWindow")) scanActions();
        return false;
    }
private:
    void commandContext(const QKeySequence &key)
    {
        QJsonArray shortcuts, actions;
        for (auto window : QApplication::topLevelWidgets()) {
            if (!related(window)) continue;
            for (auto shortcut : window->findChildren<QShortcut *>()) {
                if (shortcut->key() != key) continue;
                auto owner = qobject_cast<QWidget *>(shortcut->parent());
                shortcuts.append(QJsonObject{{"key", key.toString()}, {"enabled", shortcut->isEnabled()},
                    {"context", int(shortcut->context())}, {"owner", widgetState(owner)}});
            }
            for (auto action : window->findChildren<QAction *>()) {
                if (!action->shortcuts().contains(key) && !action->text().contains(QStringLiteral("ripple delete"), Qt::CaseInsensitive)) continue;
                QJsonArray owners;
                for (auto owner : action->associatedWidgets()) owners.append(widgetState(owner));
                actions.append(QJsonObject{{"name", action->objectName()}, {"label", action->text()},
                    {"enabled", action->isEnabled()}, {"visible", action->isVisible()},
                    {"context", int(action->shortcutContext())}, {"owners", owners}});
            }
        }
        logEvent(QStringLiteral("input.command-context"), {{"key", key.toString()},
            {"shortcuts", shortcuts}, {"actions", actions}, {"focus", widgetState(QApplication::focusWidget())}});
    }
    static bool related(QWidget *widget)
    {
        for (auto ancestor = widget; ancestor; ancestor = ancestor->parentWidget()) {
            const QString name = ancestor->objectName();
            if (name == QLatin1String("UiMainWindow") || name == QLatin1String("unboundPanelTest") ||
                name.startsWith(QLatin1String("unboundFloating_"))) return true;
        }
        return false;
    }

    void observeButton(QAbstractButton *button)
    {
        if (!button || buttons.contains(button)) return;
        buttons.insert(button);
        connect(button, &QObject::destroyed, this, [this, button] { buttons.remove(button); });
        auto record = [button](const QString &event) {
            logEvent(event, {{"button", widgetState(button)}, {"label", button->text()},
                {"checked", button->isChecked()}, {"tooltip", button->toolTip()},
                {"focus", widgetState(QApplication::focusWidget())}});
        };
        // Press precedes the action; clicked also covers keyboard and programmatic activation.
        connect(button, &QAbstractButton::pressed, this, [record] { record(QStringLiteral("input.button.pressed")); });
        connect(button, &QAbstractButton::clicked, this, [record] { record(QStringLiteral("input.button.clicked")); });
    }
    void observe(QAction *action)
    {
        if (!action || tracked.contains(action)) return;
        tracked.insert(action);
        connect(action, &QObject::destroyed, this, [this, action] { tracked.remove(action); });
        connect(action, &QAction::triggered, this, [this, action] {
            logEvent(QStringLiteral("input.action.triggered"), {{"action", action->objectName()},
                {"label", action->text()}, {"focus", widgetState(QApplication::focusWidget())}});
            if (action->objectName() == "workspaceViewerModeCinemaViewer" || action->text().contains("Cinema Viewer", Qt::CaseInsensitive)) {
                viewerDiagnostics->capture("command-triggered", action->objectName());
                const QString name = action->objectName();
                QTimer::singleShot(0, this, [this, name] { viewerDiagnostics->capture("command-dispatched", name); });
                QTimer::singleShot(600, this, [this, name] { viewerDiagnostics->capture("command-settled", name); });
            }
        });
    }
    void scanActions()
    {
        viewerDiagnostics->scan();
        QJsonArray viewerActions;
        for (auto window : QApplication::topLevelWidgets()) if (window->objectName() == QLatin1String("UiMainWindow"))
            for (auto action : window->findChildren<QAction *>()) {
                observe(action);
                const QString text = action->text().toLower();
                if (text.contains("full screen") || text.contains("fullscreen") || text.contains("cinema viewer"))
                    viewerActions.append(QJsonObject{{"name", action->objectName()}, {"label", action->text()},
                        {"enabled", action->isEnabled()}, {"checkable", action->isCheckable()}, {"checked", action->isChecked()},
                        {"context", int(action->shortcutContext())}, {"shortcut", action->shortcut().toString()}});
            }
        if (!viewerActions.isEmpty()) logEvent(QStringLiteral("input.viewer-actions"), {{"actions", viewerActions}});
        for (auto window : QApplication::topLevelWidgets()) if (related(window))
            for (auto button : window->findChildren<QAbstractButton *>())
                if (button->isVisible() && (button->text().trimmed() == "Stabilize" || button->text().trimmed() == "Stabilization")) stabilizationContext(button);
    }
    static void stabilizationContext(QAbstractButton *button)
    {
        QJsonArray ancestors;
        for (auto parent = button->parentWidget(); parent && ancestors.size() < 8; parent = parent->parentWidget()) ancestors.append(widgetState(parent));
        QJsonArray actions;
        for (auto window : QApplication::topLevelWidgets()) if (related(window))
            for (auto action : window->findChildren<QAction *>()) if (action->text().contains("stabil", Qt::CaseInsensitive))
                actions.append(QJsonObject{{"name", action->objectName()}, {"label", action->text()}, {"enabled", action->isEnabled()},
                    {"shortcut", action->shortcut().toString()}, {"context", int(action->shortcutContext())}});
        logEvent(QStringLiteral("inspector.stabilization.candidate"), {{"button", widgetState(button)}, {"ancestors", ancestors}, {"actions", actions}});
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
    ViewerDiagnostics *viewerDiagnostics = nullptr;
    QSet<QAction *> tracked;
    QSet<QAbstractButton *> buttons;
};
}
