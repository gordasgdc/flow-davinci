#include <QApplication>
#include <QPushButton>
#include <cstdio>
#include "../../../src/core/paneltitlegesture.h"
int main(int argc,char **argv){
 QApplication app(argc,argv);QPushButton title("Media Pool");title.setAttribute(Qt::WA_DontShowOnScreen);title.show();app.processEvents();int clicks=0,drags=0;
 QObject::connect(&title,&QPushButton::clicked,[&]{++clicks;});Unbound::PanelTitleGesture gesture(&title,&title,[&](QPoint){++drags;});
 auto send=[&](QEvent::Type type,QPoint pos,Qt::MouseButton button,Qt::MouseButtons buttons){QMouseEvent e(type,pos,title.mapToGlobal(pos),button,buttons,Qt::NoModifier);QApplication::sendEvent(&title,&e);};
 send(QEvent::MouseButtonPress,{5,5},Qt::LeftButton,Qt::LeftButton);send(QEvent::MouseButtonRelease,{5,5},Qt::LeftButton,Qt::NoButton);
 if(clicks!=1||drags)return 1;
 send(QEvent::MouseButtonPress,{5,5},Qt::LeftButton,Qt::LeftButton);send(QEvent::MouseMove,{80,5},Qt::NoButton,Qt::LeftButton);send(QEvent::MouseButtonRelease,{80,5},Qt::LeftButton,Qt::NoButton);
 if(clicks!=1||drags!=1||title.isDown())return 2;
 std::puts("PASS: native title click retained; threshold drag dispatches once without an accidental click");return 0;
}
