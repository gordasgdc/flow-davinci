// SPDX-License-Identifier: MIT
#include <QApplication>
#include <QTemporaryDir>
#include <QStackedWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <cstdio>
#include "interactiondiagnostics.h"
int main(int argc,char **argv) {
 QApplication app(argc,argv); QTemporaryDir dir; qputenv("UNBOUND_DIAGNOSTICS_DIR",dir.path().toUtf8());
 QWidget main; main.setAttribute(Qt::WA_DontShowOnScreen);main.setObjectName("UiMainWindow");
 auto layout=new QVBoxLayout(&main);auto stack=new QStackedWidget;layout->addWidget(stack);
 auto edit=new QWidget;edit->setObjectName("WStackPage_Conform");stack->addWidget(edit);
 auto fusion=new QWidget;fusion->setObjectName("WStackPage_Fusion");stack->addWidget(fusion);
 auto hidden=new QPushButton("Hidden native command",edit);hidden->setObjectName("hiddenNative");hidden->hide();
 Unbound::InteractionDiagnostics diagnostics(nullptr);main.show();QApplication::processEvents();
 hidden->click();auto dynamic=new QPushButton("Dynamic hidden command",edit);dynamic->setObjectName("dynamicNative");dynamic->hide();
 QApplication::processEvents();dynamic->click();stack->setCurrentWidget(fusion);QApplication::processEvents();
 Unbound::flushDiagnostics();QFile file(dir.path()+"/events.jsonl");file.open(QIODevice::ReadOnly);
 int hiddenCount=0,dynamicCount=0;bool fusionSeen=false,ok=true;QString session;double sequence=0;
 while(!file.atEnd()) {auto record=QJsonDocument::fromJson(file.readLine()).object();
  auto id=record["session"].toString();if(session.isEmpty())session=id;
  ok &= !id.isEmpty() && id==session && record["event_sequence"].toDouble()>sequence;sequence=record["event_sequence"].toDouble();
  if(record["event"]=="input.button.clicked") {auto name=record["button"].toObject()["name"].toString();
   if(name=="hiddenNative")++hiddenCount;if(name=="dynamicNative")++dynamicCount;
   ok &= record["page"]=="edit" && record["observation"]=="qt-button-signal";
  }
  if(record["event"]=="workspace.page.changed" && record["page"]=="fusion")fusionSeen=true;
 }
 ok &= hiddenCount==1 && dynamicCount==1 && fusionSeen;
 std::printf("interaction audit %s hidden=%d dynamic=%d fusion=%d\n",ok?"PASS":"FAIL",hiddenCount,dynamicCount,fusionSeen);return ok?0:1;
}
