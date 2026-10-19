// SPDX-License-Identifier: MIT
#pragma once
#include <QApplication>
#include <QMenu>
#include <QPointer>
#include <QSet>
#include "diagnostics.h"
namespace Unbound {
// Relocate the original native actions. Preserve their identity, ownership,
// connections, labels, enabled state and assigned shortcuts.
class WorkflowCommandMenu : public QObject {
public:
    explicit WorkflowCommandMenu(QObject *owner):QObject(owner) {}
    void wire(QMenu *root) {
        auto host=root->parentWidget() ? root->parentWidget()->window() : nullptr;
        if (!host || qobject_cast<QMenu *>(host)) return;
        for (auto widget:QApplication::allWidgets()) if (auto menu=qobject_cast<QMenu *>(widget)) {
            if (menu->property("unboundOwnMenu").toBool()) continue;
            bool found=false;
            for (auto action:menu->actions()) if (isCommand(action)) {
                found=true;
                if (!host->actions().contains(action)) host->addAction(action);
                menu->removeAction(action);
                logEvent("command.menu.relocated",{{"action",action->objectName()},{"label",action->text()}});
            }
            if (found && !watched.contains(menu)) {
                watched.insert(menu);
                connect(menu,&QObject::destroyed,this,[this,menu] { watched.remove(menu); });
                connect(menu,&QMenu::aboutToShow,this,[this,root=QPointer<QMenu>(root)] { if (root) wire(root); });
            }
        }

    }
private:
    static bool isCommand(QAction *action) {
        return action->objectName().startsWith("workspaceWIPlugins_") && action->text().startsWith("OpenGate - ");
    }
    QSet<QMenu *> watched;
};
}
