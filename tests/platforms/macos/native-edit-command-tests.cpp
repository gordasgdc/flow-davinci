// SPDX-License-Identifier: MIT
#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTemporaryDir>
#include <cstdio>
#include "nativeeditcommands.h"
int main(int argc,char **argv) {
    QApplication app(argc,argv); QTemporaryDir logs; qputenv("UNBOUND_DIAGNOSTICS_DIR",logs.path().toUtf8());
    int failures=0; auto check=[&](bool ok,const char *message) { if (!ok) { ++failures; std::fprintf(stderr,"FAIL: %s\n",message); } };
    QWidget page; page.setAttribute(Qt::WA_DontShowOnScreen); auto layout=new QVBoxLayout(&page);
    auto inspector=new QWidget(&page); inspector->setObjectName("widgetEditInspector"); layout->addWidget(inspector);
    auto row=new QVBoxLayout(inspector); auto stabilize=new QPushButton("Stabilize",inspector); row->addWidget(stabilize);
    int clicks=0; QObject::connect(stabilize,&QPushButton::clicked,[&] { ++clicks; });
    Unbound::NativeEditCommands commands(nullptr,[&] { return &page; });
    check(!commands.stabilize() && clicks==0,"inactive page cannot trigger analysis");
    page.show(); QApplication::processEvents();
    check(commands.stabilize() && clicks==1,"visible Inspector native button receives original click signal");
    stabilize->setEnabled(false); check(!commands.stabilize() && clicks==1,"disabled stabilization is never invoked"); stabilize->setEnabled(true);
    auto duplicate=new QPushButton("Stabilize",inspector); row->addWidget(duplicate); duplicate->show(); QApplication::processEvents();
    check(!commands.stabilize() && clicks==1,"ambiguous stabilization targets are refused"); delete duplicate;
    QMenu unrelated; auto wrong=unrelated.addAction("Fixed Playhead"); wrong->setCheckable(true);
    check(!commands.fixedPlayhead() && !wrong->isChecked(),"unrelated same-label action is not invoked");
    QMenu timeline; timeline.addAction("Display Stacked Timelines"); auto fixed=timeline.addAction("Fixed Playhead"); fixed->setCheckable(true);
    check(commands.fixedPlayhead() && fixed->isChecked() && !wrong->isChecked(),"only native Timeline Options action is toggled");
    check(commands.fixedPlayhead() && !fixed->isChecked(),"second command reverses the same native state");
    fixed->setEnabled(false); check(!commands.fixedPlayhead() && !fixed->isChecked(),"disabled timeline action is respected");
    std::printf("native edit command failures=%d\n",failures); return failures ? 1 : 0;
}
