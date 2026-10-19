// SPDX-License-Identifier: MIT
#include <QApplication>
#include <QPushButton>
#include "floatinghost.h"
#include "compacttoolbar.h"
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
    center.setObjectName("UiProjectsMainBarImp");auto project=new QPushButton("Project A",&center);project->setObjectName("btnProjectsMainBarProjectList");
    (new QVBoxLayout(&center))->addWidget(project);(new QVBoxLayout(&toolbar))->addWidget(&center);toolbar.show();center.show();project->show();
    double scale=1.;int floated=0;
    bool source=true;int calls=0;QMenu global;auto globalAction=global.addAction("Show Source Monitor");
    Unbound::WorkspaceHeaders headers(nullptr);
    headers.primary=[&] { return &first; };headers.projectSurface=[&] { return &toolbar; };headers.allPanels=[&] { return &global; };
    headers.add({"timeline","Timeline",[&] { return &first; },[] { return true; },[](bool) {}});
    headers.add({"source","Source Monitor",[&] { return &second; },[&] { return source; },[&](bool shown) { source=shown;++calls; }});
    headers.add({"unboundWorkspaceToolbar","Workspace Toolbar",[&] { return &first; },[] { return true; },[](bool) {},
        [&] { return scale; },[&](double value) { scale=value; },[&] { ++floated; }});
    auto settle=[] { QElapsedTimer t;t.start();while (t.elapsed()<90) { QApplication::processEvents();QThread::msleep(1); } };
    headers.schedule();settle();bool ok=first.windowTitle()=="Project A" && second.windowTitle().contains("Source Monitor");
    project->hide();project->setText("Project B");headers.schedule();settle();ok &= first.windowTitle()=="Project B";
    QMenu local;headers.populate(&local,&second);ok &= local.actions().size()==2 && local.actions()[1]->text()=="Source Monitor";
    local.actions()[1]->trigger();settle();ok &= !source && calls==1;
    QMenu primary;headers.populate(&primary,&first);bool shared=false;
    for (auto action:primary.actions()) if (action->menu()) shared |= action->menu()->actions().contains(globalAction);
    ok &= shared && first.size()==QSize(600,400);
    for (auto action:primary.actions()) if (auto controls=action->menu()) if (action->text()=="Workspace Toolbar Controls") {
        for (auto item:controls->actions()) {
            if (item->text()=="Float Separately") item->trigger();
            if (item->menu()) item->menu()->actions().first()->trigger();
        }
    }
    settle();ok &= floated==1 && qAbs(scale-.9)<.001;
    QWidget compactWrapper;QStackedWidget stack(&compactWrapper);QWidget toolbarRow;
    toolbarRow.setFixedHeight(30);stack.addWidget(&toolbarRow);Unbound::CompactToolbar fit(&compactWrapper,&stack);settle();
    ok &= compactWrapper.maximumHeight()==30;
    toolbarRow.setFixedHeight(24);QEvent relayout(QEvent::LayoutRequest);QApplication::sendEvent(&stack,&relayout);settle();
    ok &= compactWrapper.maximumHeight()==24;
    Unbound::MacFloatingHost chrome(&main);chrome.setAttribute(Qt::WA_DontShowOnScreen);chrome.setChrome(Unbound::WindowChrome::Standard);
    ok &= chrome.contentsMargins().top()==0;
    chrome.setChrome(Unbound::WindowChrome::Compact);chrome.show();settle();
    ok &= Unbound::setNativeHeaderMenu(&chrome,true,[] {});
    Unbound::MacFloatingHost strip(&main);strip.setAttribute(Qt::WA_DontShowOnScreen);strip.setProperty("unboundToolbarStrip",true);
    strip.setChrome(Unbound::WindowChrome::Standard);strip.show();settle();
    ok &= strip.contentsMargins().top()==0 && strip.contentsMargins().left()>=100;
    ok &= Unbound::setNativeHeaderMenu(&strip,true,[] {});
    int clicked=0;Unbound::setNativePin(&first,false,[](bool) {});
    ok &= Unbound::setNativeHeaderMenu(&first,true,[&] { ++clicked; });
    auto button=Unbound::nativeHeaderMenuFrameForTest(&first);auto traffic=Unbound::nativeTrafficFramesForTest(&first);
    ok &= !button.isEmpty() && traffic.size()==4;
    for (const auto &frame:traffic) ok &= !frame.intersects(button);
    Unbound::clickNativeHeaderMenuForTest(&first);ok &= clicked==1;
    std::printf("workspace headers %s\n",ok ? "PASS" : "FAIL");return ok ? 0 : 1;
}
