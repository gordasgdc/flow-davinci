// SPDX-License-Identifier: MIT
#include <QApplication>
#include <QVBoxLayout>
#include <cstdio>
#include "../../../src/core/panellayoutsurface.h"
#include "../../../src/core/panellayoutsnapshot.h"
#include "../../../src/core/nativearrangementplan.h"
#include "../../../src/core/dockscopetargets.h"
#include "../../../src/core/pageworkspacestate.h"
#include "../../../src/core/panelplacement.h"

int main(int argc,char **argv) {
    QApplication app(argc,argv);int failures=0;
    auto check=[&](bool ok,const char *why){if(!ok){++failures;std::fprintf(stderr,"FAIL: %s\n",why);}};
    using namespace Unbound;
    const QJsonObject source{{"panel","source"}},program{{"panel","program"}},mixer{{"panel","mixer"}},timeline{{"panel","timeline"}};
    const auto column=splitGroupTree(program,mixer,Qt::Vertical);
    auto tree=splitGroupTree(splitGroupTree(column,source,Qt::Orientation(0)),timeline,Qt::Horizontal);
    const auto key=groupSectionTarget(column);
    const auto section=insertGroupTree(tree,{{"panel","inspector"}},key,Qt::Horizontal,true);
    const auto leaf=insertGroupTree(tree,{{"panel","inspector"}},"program",Qt::Horizontal,true);
    check(section!=leaf&&groupTreeHasTarget(tree,key),"individual and section insertion retain distinct scopes");
    check(insertGroupTree(tree,source,"missing",Qt::Horizontal,true)==tree,"stale targets leave the arrangement untouched");
    const auto removed=removeGroupLeaf(tree,"mixer");QStringList names;
    check(groupTreeNames(removed,names)&&!names.contains("mixer")&&names.contains("program"),"removal reclaims a branch while retaining sibling identity");
    QWidget host;host.setAttribute(Qt::WA_DontShowOnScreen);host.resize(1000,800);auto layout=new QVBoxLayout(&host);
    QHash<QString,QWidget *> widgets;
    for(const auto &id:{"source","program","mixer","timeline"}){auto widget=new QWidget(&host);widget->setProperty("groupPanel",id);widget->setMinimumSize(80,80);widgets[id]=widget;}
    PanelLayoutCallbacks callbacks;callbacks.leaf=[&](const QString &id){return widgets.value(id);};
    callbacks.title=[](const QString &id){return id=="program"?QString("Program Monitor"):id=="mixer"?QString("Mixer"):id;};
    auto root=buildPanelLayoutSurface(tree,&host,&host,callbacks);check(root!=nullptr,"shared surface builds existing widgets");
    if(root){layout->addWidget(root);root->show();host.show();QApplication::processEvents();
        const auto snapshot=panelLayoutSnapshot(root);QStringList retained;
        check(groupTreeNames(snapshot,retained)&&retained.size()==4,"snapshot retains every panel once");
        auto tabs=root->findChild<QTabWidget *>();check(tabs&&tabs->tabText(0)=="Program Monitor + Mixer","tabbed section uses actual composite title");
        const auto parent=widgets["source"]->parentWidget();const auto frame=host.geometry();
        if(tabs)tabs->setCurrentIndex(1);
        check(widgets["source"]->parentWidget()==parent&&host.geometry()==frame,"selecting a covered tab preserves ownership and host geometry");
    }
    const QRect frame(100,120,800,600);
    const QList<QPoint> points{{101,350},{898,350},{500,121},{500,718}};
    const QList<DropPlacement> edges{DropPlacement::Left,DropPlacement::Right,DropPlacement::Above,DropPlacement::Below};
    for(int i=0;i<points.size();++i){const auto drop=selectPanelDrop(frame,points[i]);check(drop.placement==edges[i]&&frame.contains(drop.preview),"each edge preview describes placement within the destination");}
    check(selectPanelDrop(frame,QPoint(300,300)).placement==DropPlacement::None,"neutral release has no docking destination");
    const auto before=widgets["source"]->parentWidget();
    check(!buildPanelLayoutSurface(splitGroupTree(source,source,Qt::Horizontal),&host,&host,callbacks)&&widgets["source"]->parentWidget()==before,"duplicate leaf rejection does not reparent real content");
    NativeArrangementPlan plan;QJsonObject states;for(const auto &id:widgets.keys())states[id]=QJsonObject{{"hidden",false}};
    const QJsonObject saved{{"version",1},{"kind","managed"},{"page","edit"},{"tree",tree},{"panels",states}};
    QSet<QString> available;for(const auto &id:widgets.keys())available.insert(id);
    check(NativeArrangementPlan::parse(saved,"edit",available,plan).isEmpty(),"shared parser accepts the saved managed tree");
    const auto valid=plan.json();check(!NativeArrangementPlan::parse(saved,"color",available,plan).isEmpty()&&plan.json()==valid,"cross-page rejection preserves the previous valid plan");
    QJsonObject working=saved;working["layout_mode"]="managed";working["selected_arrangement"]="resolve-layout.edit";
    QJsonObject persisted;check(PageWorkspaceState::record(persisted,"edit",working).isEmpty(),"working state recorded independently of preset updates");
    QJsonObject native{{"page","edit"},{"layout_mode","native"}};PageWorkspaceState::record(persisted,"edit",native);
    check(PageWorkspaceState::managedView(persisted,"edit")["tree"]==tree,"Native transition preserves unsaved managed checkpoint");
    PanelPresentation presentation;check(presentation.move(PanelPlacement::FloatingSurface,"retained-host"),"explicit placement identifies retained host");
    presentation.visible=false;presentation.expanded=true;
    check(presentation.placement==PanelPlacement::FloatingSurface&&presentation.surface=="retained-host","visibility and temporary expansion preserve durable placement");
    std::printf("shared layout policy failures=%d\n",failures);return failures?1:0;
}
