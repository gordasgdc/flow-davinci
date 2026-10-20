// SPDX-License-Identifier: MIT
#include <QApplication>
#include <QTemporaryDir>
#include "workflowcommandmenu.h"
#include "keyboardlauncher.h"
#include <cstdio>
int main(int argc,char **argv) {
    QApplication app(argc,argv); QTemporaryDir logs; qputenv("UNBOUND_DIAGNOSTICS_DIR",logs.path().toUtf8());
    QWidget main; main.setObjectName("UiMainWindow");main.setAttribute(Qt::WA_DontShowOnScreen);
    QMenu native,root(&main); auto action=native.addAction("OpenGate - Edit - Stabilize");
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
    main.show();QApplication::setActiveWindow(&main);QApplication::processEvents();
    QKeyEvent assigned(QEvent::KeyPress,Qt::Key_S,Qt::AltModifier|Qt::ShiftModifier);
    QApplication::sendEvent(&main,&assigned);
    ok &= calls==2; // Actual Qt shortcut-map dispatch, not QAction::trigger().
    if (calls!=2) std::fprintf(stderr,"FAIL: assigned native shortcut does not execute after menu regrouping\n");
    QWidget floating(&main,Qt::Window);floating.setObjectName("unboundFloating_shortcutFixture");
    floating.setAttribute(Qt::WA_DontShowOnScreen);floating.show();presentation.wire(&root);
    QApplication::setActiveWindow(&floating);QApplication::processEvents();
    QKeyEvent floatingKey(QEvent::KeyPress,Qt::Key_S,Qt::AltModifier|Qt::ShiftModifier);
    QApplication::sendEvent(&floating,&floatingKey);ok &= calls==3;
    if (calls!=3) std::fprintf(stderr,"FAIL: assigned key does not execute in a newly created floating window\n");
    action->setShortcut(QKeySequence("Alt+Shift+T"));
    QKeyEvent obsolete(QEvent::KeyPress,Qt::Key_S,Qt::AltModifier|Qt::ShiftModifier);
    QApplication::sendEvent(&floating,&obsolete);ok &= calls==3;
    QKeyEvent updated(QEvent::KeyPress,Qt::Key_T,Qt::AltModifier|Qt::ShiftModifier);
    QApplication::sendEvent(&floating,&updated);ok &= calls==4;
    QWidget unrelated;unrelated.setAttribute(Qt::WA_DontShowOnScreen);unrelated.show();
    QApplication::setActiveWindow(&unrelated);QApplication::processEvents();
    QKeyEvent outside(QEvent::KeyPress,Qt::Key_T,Qt::AltModifier|Qt::ShiftModifier);
    QApplication::sendEvent(&unrelated,&outside);ok &= calls==4;
    ok &= action->shortcutContext()==Qt::WindowShortcut;
    native.addAction(action); presentation.wire(&root);
    ok &= !native.actions().contains(action) && group->actions().count(action)==1;
    auto keyboard=native.addAction("Keyboard Customization..."); keyboard->setObjectName("resolveCustomizedActions");
    bool enumerated=false;
    QObject::connect(keyboard,&QAction::triggered,[&] { enumerated=native.actions().contains(action); });
    ok &= Unbound::openNativeKeyboardCustomization() && enumerated;
    ok &= native.actions().contains(action) && !group->actions().contains(action);
    ok &= !main.actions().contains(action) && !floating.actions().contains(action);
    presentation.wire(&root); ok &= native.actions().contains(action);
    QWidget editor; editor.setObjectName("UiActionsEditorDialog");
    QEvent hidden(QEvent::Hide); QApplication::sendEvent(&editor,&hidden); QApplication::processEvents();
    ok &= !native.actions().contains(action) && group->actions().contains(action);
    QApplication::setActiveWindow(&main);QApplication::processEvents();
    QKeyEvent restored(QEvent::KeyPress,Qt::Key_T,Qt::AltModifier|Qt::ShiftModifier);
    QApplication::sendEvent(&main,&restored);ok &= calls==5;
    {
        QMenu owner;auto retained=owner.addAction("OpenGate - Show-Hide Inspector");
        retained->setObjectName("workspaceWIPlugins_retained");main.addAction(retained);
        {
            Unbound::WorkflowCommandMenu temporary(nullptr);temporary.wire(&root);
            ok &= !owner.actions().contains(retained);
        }
        ok &= owner.actions().contains(retained) && main.actions().contains(retained) && retained->parent()==&owner;
        ok &= !owner.findChild<QMenu *>("openGateWorkflowCommands");
    }
    std::printf("workflow command menu %s\n",ok ? "PASS" : "FAIL"); return ok ? 0 : 1;
}
