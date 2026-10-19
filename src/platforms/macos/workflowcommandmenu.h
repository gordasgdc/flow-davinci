// SPDX-License-Identifier: MIT
#pragma once
#include <QApplication>
#include <QMenu>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPointer>
#include <QSet>
#include <QTimer>
#include "diagnostics.h"
namespace Unbound {
// Keep native action identity and ownership. Restore the original menu structure
// before Resolve enumerates keyboard commands; regroup only after its editor closes.
class WorkflowCommandMenu : public QObject {
public:
    explicit WorkflowCommandMenu(QObject *owner):QObject(owner) { instances().append(this); qApp->installEventFilter(this); }
    ~WorkflowCommandMenu() override { instances().removeAll(this); if (qApp) qApp->removeEventFilter(this); }
    static void prepareKeyboardEditor() { for (auto instance:instances()) instance->restoreForEditor(); }
    void restoreForEditor() {
        editing=true; int count=0;
        for (const auto &entry:entries) if (entry.action && entry.origin) {
            if (entry.group) entry.group->removeAction(entry.action);
            if (!entry.origin->actions().contains(entry.action)) entry.origin->addAction(entry.action);
            ++count;
        }
        logEvent("keyboard.commands.restored",{{"count",count},{"reason","native-editor-enumeration"}});
    }
    void wire(QMenu *root) {
        lastRoot=root; if (editing) return;
        const auto widgets=QApplication::allWidgets();
        for (auto widget:widgets) if (auto menu=qobject_cast<QMenu *>(widget)) {
            if (menu->property("unboundOwnMenu").toBool()) continue;
            bool found=false;
            for (auto action:menu->actions()) {
                if (action->objectName()=="resolveCustomizedActions" && !keyboardActions.contains(action)) {
                    keyboardActions.insert(action);
                    connect(action,&QObject::destroyed,this,[this,action] { keyboardActions.remove(action); });
                }
                if (!isCommand(action)) continue;
                auto group=menu->findChild<QMenu *>("openGateWorkflowCommands",Qt::FindDirectChildrenOnly);
                if (!group) {
                    group=new QMenu("OpenGate Commands",menu); group->setObjectName("openGateWorkflowCommands");
                    group->setProperty("unboundOwnMenu",true); menu->addMenu(group);
                }
                bool known=false;
                for (const auto &entry:entries) known |= entry.action==action;
                if (!known) entries.append({action,menu,group});
                group->addAction(action); menu->removeAction(action); found=true;
                logEvent("command.menu.grouped",{{"action",action->objectName()},{"label",action->text()}});
            }
            if (found && !watched.contains(menu)) {
                watched.insert(menu);
                connect(menu,&QObject::destroyed,this,[this,menu] { watched.remove(menu); });
                connect(menu,&QMenu::aboutToShow,this,[this] { if (lastRoot) wire(lastRoot); });
            }
        }
    }
protected:
    bool eventFilter(QObject *object,QEvent *event) override {
        if (event->type()==QEvent::Shortcut && object->objectName()=="resolveCustomizedActions") restoreForEditor();
        if (event->type()==QEvent::MouseButtonPress || event->type()==QEvent::KeyPress) if (auto menu=qobject_cast<QMenu *>(object)) {
            QAction *action=nullptr;
            if (event->type()==QEvent::MouseButtonPress) action=menu->actionAt(static_cast<QMouseEvent *>(event)->pos());
            else {
                const auto key=static_cast<QKeyEvent *>(event)->key();
                if (key==Qt::Key_Return || key==Qt::Key_Enter) action=menu->activeAction();
            }
            if (action && action->objectName()=="resolveCustomizedActions") restoreForEditor();
        }
        if (event->type()==QEvent::Hide && object->objectName()=="UiActionsEditorDialog") {
            editing=false; QTimer::singleShot(0,this,[this] { if (lastRoot) wire(lastRoot); });
        }
        return false;
    }
private:
    struct Entry { QPointer<QAction> action; QPointer<QMenu> origin,group; };
    static QList<WorkflowCommandMenu *> &instances() { static QList<WorkflowCommandMenu *> value; return value; }
    static bool isCommand(QAction *action) {
        return action->objectName().startsWith("workspaceWIPlugins_") && action->text().startsWith("OpenGate - ");
    }
    QList<Entry> entries; QSet<QMenu *> watched; QSet<QAction *> keyboardActions;
    QPointer<QMenu> lastRoot; bool editing=false;
};
}
