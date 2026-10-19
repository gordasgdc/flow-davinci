// SPDX-License-Identifier: MIT
#include <QApplication>
#include <QLineEdit>
#include <QTemporaryDir>
#include <QVBoxLayout>
#include <cstdio>
#include "nativekeyboardnavigation.h"
int main(int argc,char **argv) {
    QApplication app(argc,argv); QTemporaryDir logs; qputenv("UNBOUND_DIAGNOSTICS_DIR",logs.path().toUtf8());
    int failures=0; auto check=[&](bool ok,const char *why) { if (!ok) { ++failures; std::fprintf(stderr,"FAIL: %s\n",why); } };
    QWidget pane; auto layout=new QVBoxLayout(&pane); auto search=new QLineEdit(&pane); layout->addWidget(search);
    check(Unbound::filterNativeKeyboardCommands(&pane,"OpenGate") && search->text()=="OpenGate","navigation sets only the native command search");
    auto other=new QLineEdit(&pane); layout->addWidget(other); search->setText("unchanged"); other->setText("also unchanged");
    check(!Unbound::filterNativeKeyboardCommands(&pane,"OpenGate") && search->text()=="unchanged" && other->text()=="also unchanged","ambiguous native controls are not guessed");
    Unbound::NativeKeyboardNavigation navigation; int opened=0; QMenu native;
    auto action=native.addAction("Keyboard Customization..."); QObject::connect(action,&QAction::triggered,[&] { ++opened; });
    check(navigation.open("OpenGate") && opened==1,"OpenGate route invokes the same native editor command");
    action->setEnabled(false); check(!navigation.open("OpenGate") && opened==1,"unavailable native editor never falls back to a conflicting binding editor");
    std::printf("native keyboard navigation failures=%d\n",failures); return failures ? 1 : 0;
}
