// SPDX-License-Identifier: MIT
#include <QApplication>
#include <QPushButton>
#include "floatinghost.h"
#include "compacttoolbar.h"
#include "toolbardoors.h"
#include "paneloverflow.h"
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
    QWidget doorSurface;QWidget doorRow(&doorSurface);doorRow.setObjectName("frameEditTopBarLeftInSSM");new QHBoxLayout(&doorRow);
    bool doorShown=false;int doorCalls=0;Unbound::ToolbarDoors doors(nullptr);
    doors.add({"source","Source Monitor",[&] { return doorShown; },[&] { doorShown=!doorShown;++doorCalls; }});
    doors.wire(&doorSurface);doors.wire(&doorSurface);auto door=doorRow.findChild<QToolButton *>("openGateToolbarDoor_source");
    ok &= door && doorRow.findChildren<QToolButton *>().size()==2;if (door) door->click();settle();doors.wire(&doorSurface);
    ok &= doorShown && doorCalls==1 && door->isChecked();
    // Added controls compress before entering the native project title's area.
    QWidget crowded;crowded.setAttribute(Qt::WA_DontShowOnScreen);crowded.resize(900,60);
    QWidget nativeRow(&crowded);nativeRow.setObjectName("frameEditTopBarLeftFixture");nativeRow.setGeometry(0,0,900,40);
    auto nativeLayout=new QHBoxLayout(&nativeRow);nativeLayout->setContentsMargins(0,0,0,0);
    auto nativeButton=new QPushButton("Native",&nativeRow);nativeButton->setFixedWidth(260);nativeLayout->addWidget(nativeButton);
    auto title=new QPushButton("Project title",&crowded);title->setObjectName("btnProjectsMainBarProjectList");title->setGeometry(650,0,150,30);
    Unbound::ToolbarDoors responsive(nullptr);
    for(const auto &id:QStringList{"Timeline","SourceMonitor","ProgramMonitor","page-navigation"})responsive.add({id,id,[]{return true;},[]{}});
    crowded.show();nativeRow.show();title->show();responsive.wire(&crowded);settle();
    auto owned=nativeRow.findChild<QWidget *>("openGateToolbarDoors");
    auto limits=owned->findChild<QWidget *>("openGateDoorControls");
    ok &= owned && limits->maximumWidth()<=378;
    title->move(440,0);QEvent resized(QEvent::Resize);QApplication::sendEvent(&crowded,&resized);settle();
    ok &= owned->property("openGateDoorMode")=="icons";
    title->move(310,0);QApplication::sendEvent(&crowded,&resized);settle();
    ok &= owned->property("openGateDoorMode")=="overflow";
    ok &= limits->mapToGlobal(QPoint(limits->width(),0)).x() <= title->mapToGlobal(QPoint()).x();
    crowded.resize(1600,60);nativeRow.resize(1600,40);title->move(1300,0);QApplication::sendEvent(&crowded,&resized);settle();
    ok &= owned->property("openGateDoorMode")=="labels";
    ok &= limits->mapToGlobal(QPoint(limits->width(),0)).x() <= title->mapToGlobal(QPoint()).x();
    auto overflowButton=owned->findChild<QToolButton *>("openGateToolbarOverflow");
    ok &= overflowButton->menu()->actions().size()==4 && nativeButton->width()==260 && title->width()==150;
    QWidget scrollOwner;scrollOwner.setAttribute(Qt::WA_DontShowOnScreen);scrollOwner.resize(300,65);
    auto rowContent=new QWidget;rowContent->setMinimumWidth(900);rowContent->setFixedHeight(30);
    auto last=new QPushButton("Last",rowContent);auto contentLayout=new QHBoxLayout(rowContent);contentLayout->addStretch();contentLayout->addWidget(last);
    Unbound::PanelOverflow overflow;auto scroll=qobject_cast<QScrollArea *>(overflow.wrap(rowContent,&scrollOwner,true));
    (new QVBoxLayout(&scrollOwner))->addWidget(scroll);scrollOwner.show();settle();
    ok &= scroll->horizontalScrollBar()->maximum()>0 && scroll->verticalScrollBarPolicy()==Qt::ScrollBarAlwaysOff && scrollOwner.width()==300;
    scroll->horizontalScrollBar()->setValue(scroll->horizontalScrollBar()->maximum());settle();
    ok &= scroll->viewport()->rect().contains(last->mapTo(scroll->viewport(),last->rect().center()));
    int clicked=0;Unbound::setNativePin(&first,false,[](bool) {});
    ok &= Unbound::setNativeHeaderMenu(&first,true,[&] { ++clicked; });
    const QString longTitle="A long project title that should elide by width while controls remain stationary";
    first.setWindowTitle(longTitle);Unbound::setNativePin(&first,false,[](bool) {});
    ok &= Unbound::nativeTitleToolTipForTest(&first)==longTitle;
    auto button=Unbound::nativeHeaderMenuFrameForTest(&first);auto traffic=Unbound::nativeTrafficFramesForTest(&first);
    ok &= !button.isEmpty() && traffic.size()==4;
    for (const auto &frame:traffic) ok &= !frame.intersects(button);
    Unbound::clickNativeHeaderMenuForTest(&first);ok &= clicked==1;
    std::printf("workspace headers %s\n",ok ? "PASS" : "FAIL");return ok ? 0 : 1;
}
