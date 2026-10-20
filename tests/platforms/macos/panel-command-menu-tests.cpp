// SPDX-License-Identifier: MIT
#include <QApplication>
#include <QTemporaryDir>
#include <QThread>
#include <cstdio>
#include "panelcommandmenu.h"
int main(int argc,char **argv){
    QApplication app(argc,argv);QTemporaryDir files;qputenv("UNBOUND_DIAGNOSTICS_DIR",files.path().toUtf8());
    int failures=0;auto check=[&](bool ok,const char *message){if(!ok){++failures;std::fprintf(stderr,"FAIL %s\n",message);}};
    auto settle=[] {QElapsedTimer timer;timer.start();while(timer.elapsed()<100){QApplication::processEvents();QThread::msleep(1);}};
    QWidget main;main.setObjectName("UiMainWindow");main.setAttribute(Qt::WA_DontShowOnScreen);main.show();QApplication::setActiveWindow(&main);settle();
    Unbound::PanelCommands registry(nullptr);registry.context=[] {return true;};
    int stabilizations=0,fixed=0,expanded=0,shown=0;bool available=true;
    registry.add({"edit.inspector.stabilize","Edit Inspector / Stabilize",[&]{return available;},[&]{++stabilizations;}});
    registry.add({"edit.timeline.fixed-playhead","Edit Timeline / Toggle Fixed Playhead",[&]{return available;},[&]{++fixed;}});
    registry.add({"panel.expand.source","Maximize / Restore Source Monitor",[&]{return available;},[&]{++expanded;}});
    registry.add({"panel.toggle.source","Show / Hide Source Monitor",[&]{return available;},[&]{++shown;}});
    QMenu native;auto original=native.addAction("OpenGate - Edit - Stabilize");original->setObjectName("workspaceWIPlugins_0");
    const auto originalActions=native.actions();const auto hosts=original->associatedWidgets();
    QMenu root;QPointer<QMenu> retained;
    {
        Unbound::PanelCommandMenu presentation(nullptr,&registry);auto menu=presentation.menu(&root);retained=menu;root.addMenu(menu);
        check(presentation.menu(&root)==menu&&root.findChildren<QMenu *>("openGateCommands").size()==1,"repeated wiring reuses one owned submenu");
        auto find=[&](QString id){for(auto action:menu->findChildren<QAction *>())if(action->property("openGateCommandId")==id)return action;return static_cast<QAction *>(nullptr);};
        auto stabilize=find("edit.inspector.stabilize");auto fixedAction=find("edit.timeline.fixed-playhead");
        check(stabilize&&fixedAction&&find("panel.expand.source")&&find("panel.toggle.source"),"command IDs are discoverable in presentation groups");
        QWidget popup(&main,Qt::Popup);popup.setAttribute(Qt::WA_DontShowOnScreen);popup.show();settle();
        stabilize->trigger();settle();check(stabilizations==0,"menu tracking cannot execute an operation before closing");
        popup.hide();QApplication::setActiveWindow(&main);settle();check(stabilizations==1,"closed menu delegates to shared registry exactly once");
        settle();check(stabilizations==1,"deferred menu callback cannot replay");
        fixedAction->trigger();find("panel.expand.source")->trigger();find("panel.toggle.source")->trigger();settle();
        check(fixed==1&&expanded==1&&shown==1,"menu uses existing fixed, expansion and visibility command operations");
        available=false;QMetaObject::invokeMethod(menu,"aboutToShow",Qt::DirectConnection);
        check(!stabilize->isEnabled()&&!fixedAction->isEnabled(),"unavailable page commands are disabled without native action relocation");
        check(native.actions()==originalActions&&original->parent()==&native&&original->associatedWidgets()==hosts,"native workflow ordering, ownership and hosts remain untouched");
    }
    check(!retained&&native.actions()==originalActions,"presentation teardown removes only its own submenu");
    std::printf("panel command menu failures=%d\n",failures);return failures?1:0;
}
