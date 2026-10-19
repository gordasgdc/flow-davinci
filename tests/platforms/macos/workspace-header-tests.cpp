// SPDX-License-Identifier: MIT
#include <QApplication>
#include <QElapsedTimer>
#include <QThread>
#include <QTemporaryDir>
#include <QVBoxLayout>
#include "workspaceheaders.h"
#include <cstdio>
int main(int argc,char **argv) {
    QApplication app(argc,argv);QTemporaryDir logs;qputenv("UNBOUND_DIAGNOSTICS_DIR",logs.path().toUtf8());
    QWidget main,first(&main,Qt::Tool),second(&main,Qt::Tool),toolbar(&main),center(&toolbar);
    first.setObjectName("unboundFloating_primary");second.setObjectName("unboundFloating_secondary");
    for (auto widget:{&main,&first,&second}) { widget->setAttribute(Qt::WA_DontShowOnScreen);widget->resize(600,400);widget->show(); }
    center.setObjectName("centerFrame");auto project=new QLabel("Project A",&center);project->setObjectName("labelTitle");
    (new QVBoxLayout(&center))->addWidget(project);(new QVBoxLayout(&toolbar))->addWidget(&center);toolbar.show();center.show();project->show();
    bool source=true;int calls=0;QMenu global;auto globalAction=global.addAction("Show Source Monitor");
    Unbound::WorkspaceHeaders headers(nullptr);
    headers.primary=[&] { return &first; };headers.projectSurface=[&] { return &toolbar; };headers.allPanels=[&] { return &global; };
    headers.add({"timeline","Timeline",[&] { return &first; },[] { return true; },[](bool) {}});
    headers.add({"source","Source Monitor",[&] { return &second; },[&] { return source; },[&](bool shown) { source=shown;++calls; }});
    auto settle=[] { QElapsedTimer t;t.start();while (t.elapsed()<90) { QApplication::processEvents();QThread::msleep(1); } };
    headers.schedule();settle();bool ok=first.windowTitle()=="Project A" && second.windowTitle().contains("Source Monitor");
    project->setText("Project B");headers.schedule();settle();ok &= first.windowTitle()=="Project B";
    QMenu local;headers.populate(&local,&second);ok &= local.actions().size()==2 && local.actions()[1]->text()=="Source Monitor";
    local.actions()[1]->trigger();settle();ok &= !source && calls==1;
    QMenu primary;headers.populate(&primary,&first);bool shared=false;
    for (auto action:primary.actions()) if (action->menu()) shared |= action->menu()->actions().contains(globalAction);
    ok &= shared && first.size()==QSize(600,400);
    int clicked=0;Unbound::setNativePin(&first,false,[](bool) {});
    ok &= Unbound::setNativeHeaderMenu(&first,true,[&] { ++clicked; });
    auto button=Unbound::nativeHeaderMenuFrameForTest(&first);auto traffic=Unbound::nativeTrafficFramesForTest(&first);
    ok &= !button.isEmpty() && traffic.size()==4;
    for (const auto &frame:traffic) ok &= !frame.intersects(button);
    Unbound::clickNativeHeaderMenuForTest(&first);ok &= clicked==1;
    std::printf("workspace headers %s\n",ok ? "PASS" : "FAIL");return ok ? 0 : 1;
}
