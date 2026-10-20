// SPDX-License-Identifier: MIT
#include <QApplication>
#include <QTabBar>
#include <QTemporaryDir>
#include <QElapsedTimer>
#include <QThread>
#include <QScrollArea>
#include <cstdio>
#include "timelinetabscrolling.h"
int main(int argc,char **argv){
 QApplication app(argc,argv);QTemporaryDir logs;qputenv("UNBOUND_DIAGNOSTICS_DIR",logs.path().toUtf8());
 int failures=0;auto check=[&](bool ok,const char *why){if(!ok){++failures;std::fprintf(stderr,"FAIL %s\n",why);}};
 auto settle=[] {QElapsedTimer t;t.start();while(t.elapsed()<60){QApplication::processEvents();QThread::msleep(1);}};
 QTabBar tabs;tabs.setAttribute(Qt::WA_DontShowOnScreen);tabs.setUsesScrollButtons(true);tabs.setExpanding(false);tabs.setElideMode(Qt::ElideNone);
 for(int i=0;i<15;++i)tabs.addTab(QString("Timeline %1 long title").arg(i));
 tabs.resize(340,36);tabs.show();settle();
 const int selected=tabs.currentIndex();
 {
  Unbound::TimelineTabScroller adapter(&tabs);settle();auto scroll=tabs.findChild<QScrollBar *>("openGateTimelineTabScroll");
  check(adapter.supported()&&scroll&&scroll->maximum()>0,"overflowing Qt tabs gain scrollbar");
  const int leftBefore=tabs.tabRect(0).left();scroll->setValue(scroll->maximum());settle();
  check(tabs.tabRect(0).left()<leftBefore&&tabs.currentIndex()==selected,"scrollbar moves tabs without selecting another timeline");
  QWheelEvent wheel(QPointF(100,10),QPointF(100,10),QPoint(120,0),QPoint(),Qt::NoButton,Qt::NoModifier,Qt::ScrollUpdate,false);
  QApplication::sendEvent(&tabs,&wheel);settle();check(tabs.currentIndex()==selected,"trackpad scroll preserves selection");
  for(auto button:tabs.findChildren<QToolButton *>())if(button->arrowType()==Qt::LeftArrow&&button->isEnabled()){button->click();settle();break;}
  check(tabs.currentIndex()==selected,"native arrows remain functional without changing selection");
 }
 check(!tabs.findChild<QScrollBar *>("openGateTimelineTabScroll"),"teardown removes only added scrollbar");
 check(tabs.count()==15&&tabs.currentIndex()==selected,"native tabs retained on teardown");
 QWidget custom;custom.setAttribute(Qt::WA_DontShowOnScreen);custom.resize(300,40);custom.show();
 {Unbound::TimelineTabScroller unsupported(&custom);check(!unsupported.supported()&&!custom.findChild<QScrollBar *>(),"unrecognized custom tabs get diagnostics only, not guessed mutations");}
 QWidget nativeModel;nativeModel.setAttribute(Qt::WA_DontShowOnScreen);nativeModel.resize(300,40);auto area=new QScrollArea(&nativeModel);area->setGeometry(0,0,300,30);
 auto content=new QWidget;content->setFixedSize(1200,20);area->setWidget(content);nativeModel.show();area->show();settle();
 {Unbound::TimelineTabScroller adapter(&nativeModel);settle();auto scroll=nativeModel.findChild<QScrollBar *>("openGateTimelineTabScroll");
  check(adapter.supported()&&scroll&&scroll->maximum()>0,"native scroll model is reused");
  scroll->setValue(scroll->maximum());settle();check(area->horizontalScrollBar()->value()==area->horizontalScrollBar()->maximum(),"scrollbar delegates to native model");}
 std::printf("timeline tab scrolling failures=%d\n",failures);return failures?1:0;
}
