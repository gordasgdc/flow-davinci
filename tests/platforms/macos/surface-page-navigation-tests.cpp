// SPDX-License-Identifier: MIT
#include <QApplication>
#include <QPushButton>
#include <QStackedWidget>
#include <QTemporaryDir>
#include <QElapsedTimer>
#include <QThread>
#include <cstdio>
#include "footerpanel.h"
class NativePaintProbe : public QPushButton {
public:using QPushButton::QPushButton;
 void paintEvent(QPaintEvent *) override {QPainter p(this);p.fillRect(rect(),QColor(35,45,55));p.setPen(QColor(240,200,90));p.drawText(rect(),Qt::AlignCenter,"Native Edit");}
};
int main(int argc,char **argv){
 QApplication app(argc,argv);QTemporaryDir logs;qputenv("UNBOUND_DIAGNOSTICS_DIR",logs.path().toUtf8());int failures=0;
 auto check=[&](bool ok,const char *why){if(!ok){++failures;std::fprintf(stderr,"FAIL: %s\n",why);}};
 auto settle=[] {QElapsedTimer t;t.start();while(t.elapsed()<140){QApplication::processEvents();QThread::msleep(1);}};
 {NativePaintProbe original;original.resize(120,36);original.setAttribute(Qt::WA_DontShowOnScreen);Unbound::NativePageButton mirror(nullptr);mirror.resize(original.size());mirror.setAttribute(Qt::WA_DontShowOnScreen);mirror.setNativeTarget(&original);
  for(int ratio:{1,2}){QImage a(original.size()*ratio,QImage::Format_ARGB32_Premultiplied),b(a.size(),a.format());a.setDevicePixelRatio(ratio);b.setDevicePixelRatio(ratio);a.fill(Qt::transparent);b.fill(Qt::transparent);original.render(&a);mirror.render(&b);check(a==b,"secondary native painting matches at normal and Retina density");}}
 QWidget main;main.setObjectName("UiMainWindow");main.setAttribute(Qt::WA_DontShowOnScreen);main.resize(1100,600);auto layout=new QVBoxLayout(&main);
 auto stack=new QStackedWidget(&main);layout->addWidget(stack);auto edit=new QWidget;edit->setObjectName("WStackPage_Conform");stack->addWidget(edit);auto audio=new QWidget;audio->setObjectName("WStackPage_Audio");stack->addWidget(audio);
 auto native=new QWidget(&main);native->setObjectName("m_PageSwitcherContainer");auto row=new QHBoxLayout(native);layout->addWidget(native);
 auto middle=new QWidget(native);middle->setObjectName("frameControlPanelMiddle");auto pages=new QHBoxLayout(middle);row->addWidget(middle);QPushButton *audioButton=nullptr;int pageClicks=0,homeClicks=0,settingsClicks=0;
 const QStringList ids{"buttonBrowsePanel","buttonFirstCutPanel","buttonConformPanel","buttonFusionPanel","buttonColorPanel","buttonPhotoPanel","buttonAudioPanel","buttonDeliverPanel"};
 for(auto id:ids){auto button=new QPushButton(id,middle);button->setObjectName(id);pages->addWidget(button);if(id=="buttonAudioPanel"){audioButton=button;QObject::connect(button,&QPushButton::clicked,[&]{++pageClicks;stack->setCurrentWidget(audio);});}}
 auto home=new QPushButton("Home",native);home->setObjectName("buttonHome");row->addWidget(home);QObject::connect(home,&QPushButton::clicked,[&]{++homeClicks;});auto settings=new QPushButton("Settings",native);settings->setObjectName("buttonConfig");row->addWidget(settings);QObject::connect(settings,&QPushButton::clicked,[&]{++settingsClicks;});
 auto action=new QAction("Show Page Navigation",&main);action->setCheckable(true);action->setChecked(true);QObject::connect(action,&QAction::toggled,[&](bool visible){native->setVisible(visible);});
 QWidget first(&main,Qt::Tool),second(&main,Qt::Tool);first.setObjectName("unboundFloating_first");second.setObjectName("unboundFloating_second");for(auto host:{&first,&second}){host->setAttribute(Qt::WA_DontShowOnScreen);host->resize(1000,500);auto box=new QVBoxLayout(host);box->addWidget(new QWidget(host));host->show();}main.show();settle();
 const auto parent=native->parentWidget();const auto index=layout->indexOf(native);const auto minimum=native->minimumSize();const auto policy=native->sizePolicy();QWidget *primary=&first;QJsonObject saved;
 {
 Unbound::FooterPanel controller(nullptr,[&]{return &main;},{});controller.primarySurface=[&]{return primary;};controller.surfaceHosts=[&]{return QList<QWidget *>{&first,&second};};controller.restore();settle();
 auto band=first.findChild<QScrollArea *>("unboundAttachedAccessory_m_PageSwitcherContainer");auto body=first.layout()->itemAt(0)->widget();
 check(band&&band->widget()==native&&native->window()==&first,"primary gets the actual complete native footer");check(home->window()==&first&&settings->window()==&first,"original Home and Settings travel with the real footer");check(body->geometry().bottom()<band->geometry().top(),"native footer reserves a bottom band outside content");home->click();settings->click();check(homeClicks==1&&settingsClicks==1,"real native Home and Settings signals stay connected");
 auto b=second.findChild<Unbound::SurfacePageNavigation *>("openGateSurfacePageNavigation");check(b&&!b->isVisibleTo(&second),"secondary footer stays opt-in");controller.setPinned(&second,true);settle();
 auto mirrorHome=b->findChild<QToolButton *>("openGatePage_home"),mirrorSettings=b->findChild<QToolButton *>("openGatePage_settings");check(mirrorHome&&mirrorSettings,"secondary includes native Home and Settings");mirrorHome->click();mirrorSettings->click();settle();check(homeClicks==2&&settingsClicks==2,"secondary controls invoke the real native functions exactly once");
 b->findChild<QToolButton *>("openGatePage_fairlight")->click();settle();check(pageClicks==1&&stack->currentWidget()==audio&&audioButton->window()==&first,"page request uses the real native footer without replacing it");
 const int bodyHeight=body->height();controller.hidePanel();settle();check(!action->isChecked()&&band->isHidden()&&!b->isVisibleTo(&second)&&body->height()>bodyHeight,"native visibility hides all surfaces and reclaims space");action->trigger();settle();check(!band->isHidden()&&b->isVisibleTo(&second),"native hotkey restores pinned footers");
 second.resize(140,350);settle();check(b->horizontalScrollBar()->maximum()>0,"narrow copy scrolls without losing controls");b->horizontalScrollBar()->setValue(b->horizontalScrollBar()->maximum());settle();check(b->viewport()->rect().contains(mirrorSettings->mapTo(b->viewport(),mirrorSettings->rect().center())),"Settings remains reachable at narrow widths");
 controller.setPinned(&first,false);settle();check(native->parentWidget()==parent&&layout->indexOf(native)==index&&b->isVisibleTo(&second),"unpin primary restores native slot while optional copy stays visible");controller.setPinned(&first,true);settle();check(native->window()==&first,"repin mounts the same native footer again");
 primary=&main;controller.refreshSurfaces();settle();check(native->parentWidget()==parent&&layout->indexOf(native)==index,"native main preference restores exact original slot");check(!main.findChild<Unbound::SurfacePageNavigation *>("openGateSurfacePageNavigation",Qt::FindDirectChildrenOnly),"native main uses original footer without an overlay");
 controller.setPinned(&main,false);controller.hidePanel();controller.showPanel();settle();check(native->isHidden()&&action->isChecked()&&b->isVisibleTo(&second),"global toggles preserve independent main pin choice");controller.setPinned(&main,true);settle();saved=controller.snapshot();check(!saved["floating"].toBool()&&saved["model"]=="surface-v1","state remains compatible with pinned surface model");
 }
 check(native->parentWidget()==parent&&layout->indexOf(native)==index&&native->minimumSize()==minimum&&native->sizePolicy()==policy,"shutdown restores native ownership and constraints");check(!first.findChild<QScrollArea *>("unboundAttachedAccessory_m_PageSwitcherContainer")&&!second.findChild<Unbound::SurfacePageNavigation *>("openGateSurfacePageNavigation"),"shutdown removes only owned wrappers and copies");
 {Unbound::FooterPanel restored(nullptr,[&]{return &main;},saved);restored.primarySurface=[&]{return &main;};restored.surfaceHosts=[&]{return QList<QWidget *>{&first,&second};};restored.restore();settle();check(restored.pinned(&second),"saved secondary pin survives restart");}
 std::printf("surface navigation failures=%d\n",failures);return failures?1:0;
}
