// SPDX-License-Identifier: MIT
#include <QApplication>
#include <QTemporaryDir>
#include <QFile>
#include "workflowcommandmenu.h"
#include "keyboardlauncher.h"
#include <cstdio>
int main(int argc,char **argv) {
    QApplication app(argc,argv); QTemporaryDir logs; qputenv("UNBOUND_DIAGNOSTICS_DIR",logs.path().toUtf8());
    QWidget main; main.setObjectName("UiMainWindow");main.setAttribute(Qt::WA_DontShowOnScreen);
    QWidget floating; floating.setObjectName("unboundFloating_fixture");
    QMenu native,root(&main);
    auto before=native.addAction("Before");
    auto action=native.addAction("Flow DaVinci - Edit - Stabilize");action->setObjectName("workspaceWIPlugins_0");
    auto after=native.addAction("After");
    const auto original=native.actions();const auto hosts=action->associatedWidgets();
    int calls=0;QObject::connect(action,&QAction::triggered,[&] { ++calls; });
    bool ok=true;
    {
        Unbound::WorkflowCommandMenu observer(nullptr);
        // Repeated discovery during opening must not emit menu changes, reorder
        // actions, install shortcuts or associate native actions with new hosts.
        int changes=0;QObject::connect(action,&QAction::changed,[&] { ++changes; });
        QObject::connect(&native,&QMenu::aboutToShow,[&] { observer.wire(&root); });
        for (int i=0;i<50;++i) { observer.wire(&root);QMetaObject::invokeMethod(&native,"aboutToShow",Qt::DirectConnection); }
        QMenu owned;owned.setProperty("unboundOwnMenu",true);
        auto nested=owned.addMenu("Show / Hide");
        for(int i=0;i<20;++i){auto item=nested->addAction(QString::number(i));observer.wire(&root);delete item;observer.wire(&root);}
        Unbound::flushDiagnostics();QFile discovery(logs.path()+"/events.jsonl");discovery.open(QIODevice::ReadOnly);
        ok &= discovery.readAll().count("\"event\":\"command.menu.discovery\"")==1;
        ok &= native.actions()==original && action->associatedWidgets()==hosts;
        ok &= action->parent()==&native && action->shortcuts().isEmpty() && changes==0;
        ok &= !main.actions().contains(action) && !floating.actions().contains(action);
        ok &= !native.findChild<QMenu *>("openGateWorkflowCommands");
        action->trigger();ok &= calls==1;
        // Native-owned binding changes remain observable without OpenGate modifying them.
        action->setShortcut(QKeySequence("Alt+Shift+S"));observer.wire(&root);
        ok &= action->shortcut()==QKeySequence("Alt+Shift+S") && changes==1;
        auto keyboard=native.addAction("Keyboard Customization...");keyboard->setObjectName("resolveCustomizedActions");
        ok &= Unbound::openNativeKeyboardCustomization();
        ok &= native.actions().mid(0,3)==original;
        auto ephemeral=native.addAction("Flow DaVinci - Temporary");ephemeral->setObjectName("workspaceWIPlugins_temp");
        observer.wire(&root);ephemeral->trigger();
        Unbound::flushDiagnostics();QFile late(logs.path()+"/events.jsonl");late.open(QIODevice::ReadOnly);
        ok &= late.readAll().contains("workspaceWIPlugins_temp");
        delete ephemeral;observer.wire(&root);
        QObject::disconnect(&native,nullptr,nullptr,nullptr);
        QObject::disconnect(action,&QAction::changed,nullptr,nullptr);
    }
    ok &= native.actions().at(0)==before && native.actions().at(1)==action && native.actions().at(2)==after;
    ok &= action->associatedWidgets()==hosts;
    QFile events(logs.path()+"/events.jsonl");events.open(QIODevice::ReadOnly);const auto evidence=events.readAll();
    ok &= evidence.contains("command.native.action") && evidence.contains("command.native.binding");
    ok &= evidence.contains("native-unmodified") && !evidence.contains("command.menu.grouped");
    std::printf("native-unmodified workflow observer %s\n",ok ? "PASS" : "FAIL");return ok ? 0 : 1;
}
