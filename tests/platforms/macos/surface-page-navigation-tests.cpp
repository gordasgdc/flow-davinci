// SPDX-License-Identifier: MIT
#include <QApplication>
#include <QPushButton>
#include <QStackedWidget>
#include <QTemporaryDir>
#include <QElapsedTimer>
#include <QThread>
#include <QDir>
#include <cstdio>
#include "footerpanel.h"
int main(int argc,char **argv) {
 QApplication app(argc,argv);QTemporaryDir logs;qputenv("UNBOUND_DIAGNOSTICS_DIR",logs.path().toUtf8());
 int failures=0;auto check=[&](bool ok,const char *why){if(!ok){++failures;std::fprintf(stderr,"FAIL %s\n",why);}};
 auto settle=[] {QElapsedTimer t;t.start();while(t.elapsed()<120){QApplication::processEvents();QThread::msleep(1);}};
 QWidget main;main.setObjectName("UiMainWindow");main.setAttribute(Qt::WA_DontShowOnScreen);auto native=new QVBoxLayout(&main);
 auto pages=new QStackedWidget(&main);native->addWidget(pages);auto edit=new QWidget;edit->setObjectName("WStackPage_Conform");pages->addWidget(edit);auto audio=new QWidget;audio->setObjectName("WStackPage_Audio");pages->addWidget(audio);
 auto originalFooter=new QWidget(&main);originalFooter->setObjectName("m_PageSwitcherContainer");auto originalLayout=new QHBoxLayout(originalFooter);native->addWidget(originalFooter);
 auto middle=new QWidget(originalFooter);middle->setObjectName("frameControlPanelMiddle");auto middleLayout=new QHBoxLayout(middle);middleLayout->setContentsMargins(0,0,0,0);originalLayout->setContentsMargins(0,0,0,0);originalLayout->addWidget(middle);auto home=new QPushButton("Home",originalFooter);originalLayout->addWidget(home);
 const QStringList targets{"buttonBrowsePanel","buttonFirstCutPanel","buttonConformPanel","buttonFusionPanel","buttonColorPanel","buttonPhotoPanel","buttonAudioPanel","buttonDeliverPanel"};
 int pageClicks=0;
 auto inactiveMode=new QWidget(originalFooter);auto duplicateEdit=new QPushButton("Inactive Edit",inactiveMode);duplicateEdit->setObjectName("buttonConformPanel");inactiveMode->hide();
 for(const auto &id:targets){auto button=new QPushButton(id,middle);button->setObjectName(id);middleLayout->addWidget(button);QObject::connect(button,&QPushButton::clicked,[&,id]{++pageClicks;if(id=="buttonAudioPanel")pages->setCurrentWidget(audio);if(id=="buttonConformPanel")pages->setCurrentWidget(edit);});}
 auto visibility=new QAction("Show Page Navigation",&main);visibility->setCheckable(true);visibility->setChecked(true);int toggles=0;
 QObject::connect(visibility,&QAction::toggled,[&](bool value){++toggles;originalFooter->setVisible(value);});
 QWidget first(&main,Qt::Tool),second(&main,Qt::Tool);first.setObjectName("unboundFloating_first");second.setObjectName("unboundFloating_second");
 first.setAttribute(Qt::WA_DontShowOnScreen);second.setAttribute(Qt::WA_DontShowOnScreen);auto firstLayout=new QVBoxLayout(&first);auto secondLayout=new QVBoxLayout(&second);
 auto group=new QWidget(&first);firstLayout->addWidget(group);secondLayout->addWidget(new QWidget(&second));first.resize(1000,500);second.resize(1000,500);main.show();first.show();second.show();settle();
 const auto parent=originalFooter->parentWidget();const int index=native->indexOf(originalFooter);const auto policy=originalFooter->sizePolicy();const auto minimum=originalFooter->minimumSize();
 QJsonObject saved;QWidget *primary=&first;
 {
  Unbound::FooterPanel controller(nullptr,[&]{return &main;},{});controller.attachmentOwner=[&]{return primary;};controller.surfaceHosts=[&]{return QList<QWidget *>{&first,&second};};controller.restore();settle();
  auto a=first.findChild<Unbound::SurfacePageNavigation *>("openGateSurfacePageNavigation");auto b=second.findChild<Unbound::SurfacePageNavigation *>("openGateSurfacePageNavigation");
  check(a&&b&&a->isVisibleTo(&first)&&!b->isVisibleTo(&second),"main surface gets footer by default, secondary remains opt-in");
  check(firstLayout->indexOf(a)==firstLayout->count()-1&&firstLayout->indexOf(group)==0,"footer sits below entire group outside its content");
  const auto render=qEnvironmentVariable("OPENGATE_NAVIGATION_TEST_RENDER_DIR");
  if(!render.isEmpty()){QDir().mkpath(render);a->grab().save(render+"/navigation-wide.png");}
  controller.setPinned(&second,true);settle();check(b->isVisibleTo(&second),"second surface can pin synchronized footer");
  auto audioA=a->findChild<QToolButton *>("openGatePage_fairlight");audioA->click();settle();controller.syncAttachment();settle();
  check(pageClicks==1&&pages->currentWidget()==audio,"proxy invokes original native page button exactly once");
  check(a->findChild<QToolButton *>("openGatePage_fairlight")->isChecked()&&b->findChild<QToolButton *>("openGatePage_fairlight")->isChecked(),"active page synchronizes across surfaces");
  check(controller.hidePanel().isEmpty(),"global hide delegates to native action");settle();
  check(!visibility->isChecked()&&!a->isVisibleTo(&first)&&!b->isVisibleTo(&second)&&toggles==1,"native and both surfaces hide with exactly one native toggle");
  visibility->trigger();settle();check(a->isVisibleTo(&first)&&b->isVisibleTo(&second)&&toggles==2,"native hotkey/action restores all pinned footers");
  controller.setPinned(&first,false);controller.hidePanel();controller.showPanel();settle();
  check(!a->isVisibleTo(&first)&&b->isVisibleTo(&second),"global toggles retain per-surface pin choices");
  controller.setPinned(&first,true);first.resize(140,350);settle();
  check(a->horizontalScrollBar()->maximum()>0,"narrow footer scrolls rather than truncating pages");
  if(!render.isEmpty())a->grab().save(render+"/navigation-narrow.png");
  a->horizontalScrollBar()->setValue(a->horizontalScrollBar()->maximum());settle();auto deliver=a->findChild<QToolButton *>("openGatePage_deliver");
  check(a->viewport()->rect().contains(deliver->mapTo(a->viewport(),deliver->rect().center())),"last page is reachable at minimum width");
  check(deliver->toolTip()=="Deliver"&&deliver->accessibleName()=="Deliver","compact buttons retain complete names");
  QFont large=first.font();large.setPointSize(20);first.setFont(large);settle();a->horizontalScrollBar()->setValue(a->horizontalScrollBar()->maximum());settle();
  check(a->viewport()->rect().contains(deliver->mapTo(a->viewport(),deliver->rect().center())),"last page remains reachable after font scaling");
  first.resize(1400,500);settle();check(deliver->toolButtonStyle()==Qt::ToolButtonTextUnderIcon,"wide surface restores full page labels");
  auto nativeStrip=middle->findChild<Unbound::SurfacePageNavigation *>("openGateSurfacePageNavigation");
  check(nativeStrip&&nativeStrip->parentWidget()==middle&&home->isVisibleTo(originalFooter),"adaptive native pages preserve adjacent Home controls");
  if(nativeStrip)check(!QRect(nativeStrip->mapTo(originalFooter,QPoint()),nativeStrip->size()).intersects(home->geometry()),"native overlay does not cover neighboring footer controls");
  check(originalFooter->parentWidget()==parent&&native->indexOf(originalFooter)==index&&originalFooter->layout()==originalLayout&&originalFooter->sizePolicy()==policy&&originalFooter->minimumSize()==minimum,"native footer ownership, slot and constraints never change");
  QMenu menu;controller.populateSurfaceMenu(&menu,&second);auto navigation=menu.actions().first()->menu();check(navigation&&navigation->actions().size()==2&&navigation->actions().last()->isChecked(),"Panels exposes global visibility and this-surface placement");
  for(int repeat=0;repeat<5;++repeat)controller.restore();settle();
  check(first.findChildren<Unbound::SurfacePageNavigation *>("openGateSurfacePageNavigation").size()==1,"repeated refresh cannot duplicate surface footers");
  controller.setState({{"hidden",true},{"surfaces",controller.snapshot()["surfaces"]}});settle();
  check(visibility->isChecked()&&a->isVisibleTo(&first),"saved arrangement cannot override authoritative native global visibility");
  saved=controller.snapshot();check(!saved["floating"].toBool()&&saved["model"]=="surface-v1","snapshot migrates old attachment model to surface placement");
  controller.dock();controller.syncAttachment();settle();check(b->isVisibleTo(&second),"arrangement Dock All cannot move footer or erase placement");
 }
 check(!first.findChild<Unbound::SurfacePageNavigation *>("openGateSurfacePageNavigation")&&!second.findChild<Unbound::SurfacePageNavigation *>("openGateSurfacePageNavigation"),"teardown removes only owned strips");
 check(originalFooter->parentWidget()==&main,"teardown keeps native footer intact");
 {
  Unbound::FooterPanel reopened(nullptr,[&]{return &main;},saved);reopened.attachmentOwner=[&]{return primary;};reopened.surfaceHosts=[&]{return QList<QWidget *>{&first,&second};};reopened.restore();settle();
  check(reopened.pinned(&second)&&second.findChild<Unbound::SurfacePageNavigation *>("openGateSurfacePageNavigation")->isVisibleTo(&second),"saved surface placement restores independently of layout lease");
 }
 std::printf("surface navigation failures=%d\n",failures);return failures?1:0;
}
