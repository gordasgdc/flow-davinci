// SPDX-License-Identifier: MIT
#include <QApplication>
#include <QPushButton>
#include <QTemporaryDir>
#include <QVBoxLayout>
#include <cstdio>
#include "../../../src/core/panelsession.h"
#include "../../../src/core/panellayoutsurface.h"
#include "../../../src/core/panellayoutsnapshot.h"

int main(int argc,char **argv) {
    QApplication app(argc,argv);app.setQuitOnLastWindowClosed(false);QTemporaryDir files;
    const bool displayed=app.arguments().contains("--displayed");
    qputenv("UNBOUND_TEST_HIDDEN_HOSTS",displayed?"0":"1");
    qputenv("UNBOUND_DIAGNOSTICS_DIR",files.path().toUtf8());int failures=0;
    auto check=[&](bool ok,const char *why){if(!ok){++failures;std::fprintf(stderr,"FAIL: %s\n",why);}};
    auto turn=[&]{for(int i=0;i<8;++i)app.processEvents();};
    using namespace Unbound;
    QWidget main;if(!displayed)main.setAttribute(Qt::WA_DontShowOnScreen);main.setObjectName("UiMainWindow");main.resize(1100,700);auto outer=new QVBoxLayout(&main);
    auto page=new QWidget(&main);page->setObjectName("WStackPage_Conform");outer->addWidget(page);auto original=new QHBoxLayout(page);
    auto source=new QPushButton("Native Source",page),mixer=new QPushButton("Native Mixer",page);
    source->setObjectName("widgetEditSourcePlayer");mixer->setObjectName("frameEditAudioMeter");
    source->setMinimumSize(170,120);mixer->setMinimumSize(140,100);original->addWidget(source,3);original->addWidget(mixer,1);
    const auto sourceMinimum=source->minimumSize(),mixerMinimum=mixer->minimumSize();const auto policy=source->sizePolicy();
    int edits=0;QObject::connect(source,&QPushButton::clicked,&main,[&]{++edits;});main.show();turn();
    const auto mainFrame=main.geometry();
    ManagedPanelSession a,b;
    check(a.undock(source,page,"Source Monitor").isEmpty()&&b.undock(mixer,page,"Mixer").isEmpty(),"same managed sessions acquire native content on both platforms");
    auto retained=a.window();check(retained&&retained!=&main&&a.nativeOwnerWindow()==&main&&a.pageWidget()==page,"session retains native ownership boundary");
    if(!retained||!b.window())return 1;
    retained->setGeometry(140,160,480,350);turn();const auto frame=retained->geometry();
    a.setHiddenByUser(true);turn();check(!retained->isVisible()&&a.active(),"hiding keeps the lease active");
    page->hide();turn();page->show();turn();check(!retained->isVisible(),"page return preserves explicit user hide");
    a.setHiddenByUser(false);turn();check(a.window()==retained&&retained->geometry()==frame&&source->window()==retained,"recall retains actual host, frame and native content");
    source->click();check(edits==1,"native source signals survive floating recall");
    WorkspaceFloatingHost group(&main);group.setObjectName("unboundFloating_group_fixture");group.resize(900,600);auto column=new QVBoxLayout(&group);
    auto left=new QWidget(&group),right=new QWidget(&group);left->setProperty("groupPanel",source->objectName());right->setProperty("groupPanel",mixer->objectName());
    check(a.attachToGroup(left,true).isEmpty()&&b.attachToGroup(right,true).isEmpty(),"existing group transfer API donates layouts into shared cells");
    const auto tree=splitGroupTree({{"panel",source->objectName()}},{{"panel",mixer->objectName()}},Qt::Orientation(0));
    PanelLayoutCallbacks callbacks;callbacks.leaf=[&](const QString &id){return id==source->objectName()?left:right;};callbacks.title=[&](const QString &id){return id==source->objectName()?a.panelTitle():b.panelTitle();};
    auto surface=buildPanelLayoutSurface(tree,&group,&group,callbacks);check(surface!=nullptr,"existing surface renders the donated native layouts");if(surface)column->addWidget(surface);
    group.show();a.finishGroupTransfer();b.finishGroupTransfer();turn();
    auto tabs=qobject_cast<QTabWidget *>(surface);check(tabs&&tabs->tabText(0)=="Source Monitor"&&tabs->tabText(1)=="Mixer","group tabs have actual panel titles");
    const auto groupFrame=group.geometry();if(tabs)tabs->setCurrentIndex(1);turn();
    check(source->window()==&group&&mixer->window()==&group&&group.geometry()==groupFrame,"tab selection leaves native content in its retained group");
    check(b.beginGroupedExpansion(),"same session supports temporary grouped expansion");turn();
    auto expanded=b.expandedWindow();check(expanded&&mixer->window()==expanded,"temporary expansion leases the same real Mixer content");
    b.endGroupedExpansion();turn();check(!b.expandedWindow()&&mixer->window()==&group,"temporary restore returns Mixer to its retained group");
    a.detachFromGroup();b.detachFromGroup();group.hide();turn();check(a.window()==retained&&retained->geometry()==frame,"group detach returns the original retained host and floating frame");
    check(a.dock().isEmpty()&&b.dock().isEmpty(),"Native return succeeds through the common session");turn();
    check(page->layout()==original&&source->parentWidget()==page&&mixer->parentWidget()==page,"Native return restores exact original widget/layout ownership");
    check(source->minimumSize()==sourceMinimum&&mixer->minimumSize()==mixerMinimum&&source->sizePolicy()==policy&&main.geometry()==mainFrame,"constraints and primary frame survive the assembled session sequence");
    source->click();check(edits==2,"native signal still works after ownership return");
    std::printf("shared managed-session failures=%d\n",failures);return failures?1:0;
}
