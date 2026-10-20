// SPDX-License-Identifier: MIT
#include <QApplication>
#include <QTemporaryDir>
#include <QVBoxLayout>
#include <QPushButton>
#include <cstdio>
#include "paneloverflow.h"
#include "attachedtoolbarreveal.h"
int main(int argc,char **argv) {
 QApplication app(argc,argv);QTemporaryDir logs;qputenv("UNBOUND_DIAGNOSTICS_DIR",logs.path().toUtf8());
 QWidget page,host;page.setAttribute(Qt::WA_DontShowOnScreen);host.setAttribute(Qt::WA_DontShowOnScreen);
 host.setObjectName("unboundFloating_group_timeline");host.resize(480,400);auto outer=new QVBoxLayout(&host);
 auto timeline=new QWidget;timeline->setObjectName("frameConformTimelines");auto contents=new QVBoxLayout(timeline);
 auto parent=new QWidget(timeline);auto row=new QVBoxLayout(parent);auto toolbar=new QPushButton("Native toolbar",parent);toolbar->setObjectName("frameEditToolbarTop");row->addWidget(toolbar);contents->addWidget(parent);
 auto tracks=new QWidget(timeline);tracks->setMinimumSize(840,650);contents->addWidget(tracks);
 auto nativeMinimum=timeline->minimumSize();auto nativePolicy=timeline->sizePolicy();
 Unbound::PanelOverflow overflow;outer->addWidget(overflow.wrap(timeline,&host));page.show();host.show();
 for(int i=0;i<6;++i)QApplication::processEvents();
 auto area=host.findChild<QScrollArea *>();bool ok=area && area->horizontalScrollBar()->maximum()>0 && area->verticalScrollBar()->maximum()>0;
 ok &= timeline->minimumSize()==nativeMinimum && timeline->sizePolicy()==nativePolicy;
 if(area){area->horizontalScrollBar()->setValue(area->horizontalScrollBar()->maximum());area->verticalScrollBar()->setValue(area->verticalScrollBar()->maximum());}
 const auto offset=overflow.offset();ok &= offset.x()>0 && offset.y()>0;
 tracks->setMinimumWidth(1050);for(int i=0;i<6;++i)QApplication::processEvents();
 ok &= area && area->horizontalScrollBar()->maximum()>=offset.x()+150;
 parent->hide();ok &= Unbound::revealAttachedTimelineToolbar(&page,toolbar,timeline) && toolbar->isVisible();
 QWidget parking;overflow.releaseInto(&parking);ok &= timeline->parentWidget()==&parking && timeline->minimumSize()==nativeMinimum && timeline->sizePolicy()==nativePolicy;
 std::printf("timeline overflow %s\n",ok?"PASS":"FAIL");return ok?0:1;
}
