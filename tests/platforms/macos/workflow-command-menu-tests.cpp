// SPDX-License-Identifier: MIT
#include <QApplication>
#include <QTemporaryDir>
#include "workflowcommandmenu.h"
#include "keyboardlauncher.h"
#include <cstdio>
int main(int argc,char **argv) {
    QApplication app(argc,argv); QTemporaryDir logs; qputenv("UNBOUND_DIAGNOSTICS_DIR",logs.path().toUtf8());
    QWidget main; QMenu native,root(&main); auto action=native.addAction("OpenGate - Edit - Stabilize");
    action->setObjectName("workspaceWIPlugins_0"); action->setShortcut(QKeySequence("Alt+Shift+S"));
    auto other=native.addAction("CineWaltz Relay"); other->setObjectName("workspaceWIPlugins_1");
    auto manager=native.addAction("OpenGate"); manager->setObjectName("workspaceWIPlugins_2");
    int calls=0; QObject::connect(action,&QAction::triggered,[&] { ++calls; });
    Unbound::WorkflowCommandMenu presentation(nullptr); presentation.wire(&root);
    auto group=native.findChild<QMenu *>("openGateWorkflowCommands");
    bool ok=group && !native.actions().contains(action) && group->actions().contains(action) && action->parent()==&native;
    ok &= native.actions().contains(other) && native.actions().contains(manager);
    ok &= action->shortcut()==QKeySequence("Alt+Shift+S") && action->isEnabled(); action->trigger(); ok &= calls==1;
    presentation.wire(&root); ok &= group->actions().count(action)==1;
    native.addAction(action); presentation.wire(&root);
    ok &= !native.actions().contains(action) && group->actions().count(action)==1;
    auto keyboard=native.addAction("Keyboard Customization..."); keyboard->setObjectName("resolveCustomizedActions");
    bool enumerated=false;
    QObject::connect(keyboard,&QAction::triggered,[&] { enumerated=native.actions().contains(action); });
    ok &= Unbound::openNativeKeyboardCustomization() && enumerated;
    ok &= native.actions().contains(action) && !group->actions().contains(action);
    presentation.wire(&root); ok &= native.actions().contains(action);
    QWidget editor; editor.setObjectName("UiActionsEditorDialog");
    QEvent hidden(QEvent::Hide); QApplication::sendEvent(&editor,&hidden); QApplication::processEvents();
    ok &= !native.actions().contains(action) && group->actions().contains(action);
    std::printf("workflow command menu %s\n",ok ? "PASS" : "FAIL"); return ok ? 0 : 1;
}
