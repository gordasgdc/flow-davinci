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
#include "viewerdiagnostics.h"
#include "editcontrolinspection.h"
#include "pagecontext.h"


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
            logEvent(QStringLiteral("focus.changed"), {{"before", widgetState(before,false)}, {"after", widgetState(after,false)},
                {"active_window", widgetState(QApplication::activeWindow(),false)}});
        });
        QTimer::singleShot(0, this, [this] { scanActions(); recordPage(); });
    }
    ~InteractionDiagnostics() override { qApp->removeEventFilter(this); }
protected:
    bool eventFilter(QObject *object, QEvent *event) override
    {
        const auto type = event->type();
        if (type != QEvent::ActionAdded && type != QEvent::Shortcut && type != QEvent::Show && type != QEvent::Polish &&
            type != QEvent::MouseButtonPress && type != QEvent::KeyPress && type != QEvent::ShortcutOverride && type != QEvent::WindowActivate &&
            type != QEvent::ChildAdded && type != QEvent::ParentChange && type != QEvent::Hide) return false;
        auto widget = qobject_cast<QWidget *>(object);
        if ((type == QEvent::ChildAdded || type == QEvent::ParentChange) && related(widget)) queueButtonScan(widget);
        if (widget && widget->objectName().startsWith("WStackPage_") && (type==QEvent::Show || type==QEvent::Hide) && !pageScanQueued) {
            pageScanQueued=true;
            QTimer::singleShot(0,this,[this] { pageScanQueued=false; recordPage(); });
        }
        if (type == QEvent::Show) inspectTimelineOptions(qobject_cast<QMenu *>(widget));
        if (event->type() == QEvent::ActionAdded && related(widget)) {
            observe(static_cast<QActionEvent *>(event)->action());
        }
        if (event->type() == QEvent::Shortcut && related(QApplication::focusWidget()) && !textEditor(QApplication::focusWidget())) {
            auto shortcut = static_cast<QShortcutEvent *>(event);
            logEvent(QStringLiteral("input.shortcut"), {{"receiver", object->objectName()},
                {"class", object->metaObject()->className()}, {"key", shortcut->key().toString()},
                {"owner", object->parent() ? object->parent()->objectName() : QString()},
                {"ambiguous", shortcut->isAmbiguous()}, {"focus", widgetState(QApplication::focusWidget(),false)}});
        }
        if (related(widget) && (event->type() == QEvent::Show || event->type() == QEvent::Polish ||
            event->type() == QEvent::MouseButtonPress || event->type() == QEvent::KeyPress))
            observeButton(qobject_cast<QAbstractButton *>(widget));
        if (!related(widget)) return false;
        if (event->type() == QEvent::MouseButtonPress) {
            QPointer<QWidget> receiver = widget;
            logEvent(QStringLiteral("input.click"), {{"receiver", widgetState(widget,false)}, {"focus_before", widgetState(QApplication::focusWidget(),false)}});
            QTimer::singleShot(0, this, [receiver] {
                logEvent(QStringLiteral("input.click.focus-result"), {{"receiver", widgetState(receiver,false)},
                    {"focus", widgetState(QApplication::focusWidget(),false)}, {"active_window", widgetState(QApplication::activeWindow(),false)}});
            });
            auto button = qobject_cast<QAbstractButton *>(widget);
            if (button && (button->text().trimmed() == "Stabilize" || button->text().trimmed() == "Stabilization")) stabilizationContext(button);
        }
        // Qt propagates one key through every ancestor. Log its input receiver
        // once, rather than rebuilding three widget snapshots at each parent.
        if ((event->type() == QEvent::KeyPress || event->type() == QEvent::ShortcutOverride) &&
            (widget==QApplication::focusWidget() || (!QApplication::focusWidget()&&widget==QApplication::activeWindow()))) {
            auto key = static_cast<QKeyEvent *>(event);
            const bool typing = textEditor(widget) || textEditor(QApplication::focusWidget());
            QJsonObject state{{"receiver", widgetState(widget,false)}, {"focus", widgetState(QApplication::focusWidget(),false)},
                {"active_window", widgetState(QApplication::activeWindow(),false)}, {"text_editing", typing},
                {"event_type", int(event->type())}};
            // Do not record text or key values while the user is editing text.
            if (!typing) { state["key"] = key->key(); state["modifiers"] = int(key->modifiers()); state["repeat"] = key->isAutoRepeat(); }
            logEvent(QStringLiteral("input.key.observed"), state);
            if (qEnvironmentVariableIntValue("OPENGATE_VERBOSE_INPUT_CONTEXT")==1 &&
                !typing && event->type() == QEvent::KeyPress && widget == QApplication::focusWidget())
                commandContext(QKeySequence(int(key->modifiers()) | key->key()));
        }
        if (widget->isWindow() && event->type() == QEvent::WindowActivate) {
            if (!activationScanQueued) {
                activationScanQueued = true;
                // Actions are observed at startup and ActionAdded; buttons at
                // startup and coalesced subtree creation. Activation is not a
                // reason to walk every native action/button again.
                QTimer::singleShot(0, this, [this] { activationScanQueued = false; activationState(); });
            }
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
                    {"context", int(shortcut->context())}, {"owner", widgetState(owner,false)}});
            }
            for (auto action : window->findChildren<QAction *>()) {
                if (!action->shortcuts().contains(key) && !action->text().contains(QStringLiteral("ripple delete"), Qt::CaseInsensitive)) continue;
                QJsonArray owners;
                for (auto owner : action->associatedWidgets()) owners.append(widgetState(owner,false));
                actions.append(QJsonObject{{"name", action->objectName()}, {"label", action->text()},
                    {"enabled", action->isEnabled()}, {"visible", action->isVisible()},
                    {"context", int(action->shortcutContext())}, {"owners", owners}});
            }
        }
        logEvent(QStringLiteral("input.command-context"), {{"key", key.toString()},
            {"shortcuts", shortcuts}, {"actions", actions}, {"focus", widgetState(QApplication::focusWidget(),false)}});
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
            logEvent(event, {{"button", widgetState(button,false)}, {"label", button->text()},
                {"checked", button->isChecked()}, {"tooltip", button->toolTip()},
                {"focus", widgetState(QApplication::focusWidget(),false)},
                {"page",currentWorkspacePage(resolveMainWindow())}, {"observation","qt-button-signal"}});
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
                {"label", action->text()}, {"focus", widgetState(QApplication::focusWidget(),false)},
                {"page",currentWorkspacePage(resolveMainWindow())}, {"observation","qt-action-signal"}});
            if (action->objectName() == "workspaceViewerModeCinemaViewer" || action->text().contains("Cinema Viewer", Qt::CaseInsensitive)) {
                viewerDiagnostics->capture("command-triggered", action->objectName());
                const QString name = action->objectName();
                QTimer::singleShot(0, this, [this, name] { viewerDiagnostics->capture("command-dispatched", name); });
                QTimer::singleShot(600, this, [this, name] { viewerDiagnostics->capture("command-settled", name); });
            }
        });
    }
    void observeButtonsIn(QWidget *root)
    {
        if(!root)return;
        observeButton(qobject_cast<QAbstractButton *>(root));
        for(auto button:root->findChildren<QAbstractButton *>())observeButton(button);
    }
    void queueButtonScan(QWidget *root)
    {
        // Lazy native panels can create many children in one event turn. Scan
        // their affected subtree once, rather than all Resolve windows per child.
        for(const auto &pending:controlsScanRoots)
            if(pending && (pending==root || pending->isAncestorOf(root)))return;
        for(int i=controlsScanRoots.size()-1;i>=0;--i)
            if(!controlsScanRoots[i] || root->isAncestorOf(controlsScanRoots[i]))controlsScanRoots.removeAt(i);
        controlsScanRoots.append(root);
        if(controlsScanQueued)return;
        controlsScanQueued=true;
        QTimer::singleShot(0,this,[this] {
            controlsScanQueued=false;const auto roots=controlsScanRoots;controlsScanRoots.clear();
            for(const auto &root:roots)if(root && related(root))observeButtonsIn(root);
        });
    }
    void scanButtons()
    {
        // Includes hidden controls: a command can invoke an existing native button
        // without showing its panel. Connections observe signals, never consume input.
        for (auto window : QApplication::topLevelWidgets()) if (related(window)) {
            observeButtonsIn(window);
        }
    }
    void recordPage()
    {
        const auto page=currentWorkspacePage(resolveMainWindow());
        if(page==lastPage)return;
        logEvent("workspace.page.changed",{{"before",lastPage},{"page",page},{"observation","native-page-visibility"}});
        lastPage=page;
    }
    void scanActions()
    {
        scanButtons();
        viewerDiagnostics->scan();
        QJsonArray viewerActions;
        for (auto window : QApplication::topLevelWidgets()) if (window->objectName() == QLatin1String("UiMainWindow"))
            for (auto action : window->findChildren<QAction *>()) {
                observe(action);
                const QString text = action->text().toLower();
                if (text.contains("full screen") || text.contains("fullscreen") || text.contains("cinema viewer") || text.contains("single viewer"))
                    viewerActions.append(QJsonObject{{"name", action->objectName()}, {"label", action->text()},
                        {"enabled", action->isEnabled()}, {"checkable", action->isCheckable()}, {"checked", action->isChecked()},
                        {"context", int(action->shortcutContext())}, {"shortcut", action->shortcut().toString()}});
            }
        if (!viewerActions.isEmpty() && viewerActions != lastViewerActions) {
            lastViewerActions = viewerActions;
            logEvent(QStringLiteral("input.viewer-actions"), {{"actions", viewerActions}});
        }
        for (auto window : QApplication::topLevelWidgets()) if (related(window))
            for (auto button : window->findChildren<QAbstractButton *>())
                if (button->isVisible() && (button->text().trimmed() == "Stabilize" || button->text().trimmed() == "Stabilization")) stabilizationContext(button);
    }
    static void stabilizationContext(QAbstractButton *button)
    {
        QJsonArray ancestors;
        for (auto parent = button->parentWidget(); parent && ancestors.size() < 8; parent = parent->parentWidget()) ancestors.append(widgetState(parent,false));
        QJsonArray actions;
        for (auto window : QApplication::topLevelWidgets()) if (related(window))
            for (auto action : window->findChildren<QAction *>()) if (action->text().contains("stabil", Qt::CaseInsensitive))
                actions.append(QJsonObject{{"name", action->objectName()}, {"label", action->text()}, {"enabled", action->isEnabled()},
                    {"shortcut", action->shortcut().toString()}, {"context", int(action->shortcutContext())}});
        logEvent(QStringLiteral("inspector.stabilization.candidate"), {{"button", widgetState(button,false)}, {"ancestors", ancestors}, {"actions", actions}});
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
        logEvent(QStringLiteral("activation.state"), {{"panels", panels}, {"focus", widgetState(QApplication::focusWidget(),false)},
            {"active_window", widgetState(QApplication::activeWindow(),false)}, {"native_key", diagnosticNativeKeyWindowNumber()}});
    }
    ViewerDiagnostics *viewerDiagnostics = nullptr;
    QSet<QAction *> tracked;
    QSet<QAbstractButton *> buttons;
    QJsonArray lastViewerActions;
    QList<QPointer<QWidget>> controlsScanRoots;
    QString lastPage;
    bool activationScanQueued = false,controlsScanQueued=false,pageScanQueued=false;
};
}
