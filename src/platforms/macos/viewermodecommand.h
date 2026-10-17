// SPDX-License-Identifier: MIT
#pragma once
#include <QAction>
#include <QApplication>
#include <QMenu>
#include <QPointer>
#include <QSet>
#include <QHash>
#include <functional>
#include <QTimer>
#include "diagnostics.h"
namespace Unbound {
// Follow the native command identity and retain its user-assigned shortcut.
class ViewerModeCommand : public QObject
{
public:
    ViewerModeCommand(QObject *owner, std::function<bool()> managed, std::function<void(bool)> apply)
        : QObject(owner), managed(std::move(managed)), apply(std::move(apply)) {}
    void refresh(QWidget *main)
    {
        if (!main) return;
        QList<QAction *> actions = main->findChildren<QAction *>();
        for (auto widget : QApplication::allWidgets()) if (auto menu = qobject_cast<QMenu *>(widget))
            if (menu->objectName().startsWith("workspace")) actions.append(menu->actions());
        for (auto action : actions) {
            QString text = action->text(); text.remove('&');
            if (text.trimmed() != QLatin1String("Single Viewer Mode") || !action->isCheckable()) continue;
            if (!observed.contains(action)) {
                observed.insert(action);
                connect(action, &QObject::destroyed, this, [this, action] { observed.remove(action); priorEnabled.remove(action); });
                connect(action, &QAction::triggered, this, [this, action] {
                    if (!managed()) return;
                    const bool single = action->isChecked();
                    QTimer::singleShot(0, this, [this, single] {
                        if (!managed()) return;
                        apply(single);
                        logEvent(QStringLiteral("viewer.mode.command"), {{"single_viewer", single}});
                    });
                });
            }
            if (managed() && !QApplication::activeModalWidget()) {
                if (!priorEnabled.contains(action)) priorEnabled[action] = action->isEnabled();
                action->setEnabled(true);
            } else if (priorEnabled.contains(action)) action->setEnabled(priorEnabled.take(action));
        }
    }
private:
    std::function<bool()> managed;
    std::function<void(bool)> apply;
    QSet<QAction *> observed;
    QHash<QAction *, bool> priorEnabled;
};
}
