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
    QMenu *wire(QMenu *root) {
        destination=root->findChild<QMenu *>("openGateNativeCommands",Qt::FindDirectChildrenOnly);
        if (!destination) {
            destination=new QMenu("Commands",root); destination->setObjectName("openGateNativeCommands");
            destination->setProperty("unboundOwnMenu",true);
        }
        for (auto widget:QApplication::allWidgets()) if (auto menu=qobject_cast<QMenu *>(widget)) {
            if (menu==destination || menu->property("unboundOwnMenu").toBool()) continue;
            bool found=false;
            for (auto action:menu->actions()) if (isCommand(action)) {
                found=true; menu->removeAction(action);
                if (!destination->actions().contains(action)) destination->addAction(action);
                logEvent("command.menu.relocated",{{"action",action->objectName()},{"label",action->text()}});
            }
            if (found && !watched.contains(menu)) {
                watched.insert(menu);
                connect(menu,&QObject::destroyed,this,[this,menu] { watched.remove(menu); });
                connect(menu,&QMenu::aboutToShow,this,[this,root=QPointer<QMenu>(root)] { if (root) wire(root); });
            }
        }
        destination->menuAction()->setVisible(!destination->actions().isEmpty());
        return destination;
    }
private:
    static bool isCommand(QAction *action) {
        return action->objectName().startsWith("workspaceWIPlugins_") && action->text().startsWith("OpenGate - ");
    }
    QPointer<QMenu> destination; QSet<QMenu *> watched;
};
}
