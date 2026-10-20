// SPDX-License-Identifier: MIT
#include <QApplication>
#include <QPushButton>
#include <QLineEdit>
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
    stabilize->setText(""); stabilize->setObjectName("btnStabilize");
    check(commands.stabilize() && clicks==2,"native named analysis button does not require Qt text");
    QWidget color; auto colorButton=new QPushButton("Stabilize",&color); colorButton->setObjectName("btnStabilize");
    color.setAttribute(Qt::WA_DontShowOnScreen); color.show(); QApplication::processEvents();
    int colorClicks=0; QObject::connect(colorButton,&QPushButton::clicked,[&] { ++colorClicks; });
    check(commands.stabilize() && clicks==3 && colorClicks==0,"identical Color control cannot receive Edit analysis");
    stabilize->hide();
    check(!commands.stabilize() && clicks==3 && colorClicks==0,"hidden Edit control cannot fall through to Color");
    stabilize->show();
    QMenu unrelated; auto wrong=unrelated.addAction("Fixed Playhead"); wrong->setCheckable(true);
    check(!commands.fixedPlayhead() && !wrong->isChecked(),"unrelated same-label action is not invoked");
    QMenu timeline; timeline.addAction("Display Stacked Timelines"); auto fixed=timeline.addAction("Fixed Playhead"); fixed->setCheckable(true);
    check(commands.fixedPlayhead() && fixed->isChecked() && !wrong->isChecked(),"only native Timeline Options action is toggled");
    check(commands.fixedPlayhead() && !fixed->isChecked(),"second command reverses the same native state");
    // No selected clip or timeline focus is necessary for presentation toggling.
    stabilize->setEnabled(false);
    QLineEdit inspectorFocus(inspector);inspectorFocus.show();inspectorFocus.setFocus();QApplication::processEvents();
    check(commands.fixedPlayhead() && fixed->isChecked(),"Inspector focus and unavailable clip target do not block fixed playhead");
    check(commands.fixedPlayhead() && !fixed->isChecked(),"fixed playhead restores state independent of clip selection");
    page.hide();QApplication::processEvents();
    check(!commands.fixedPlayhead() && !fixed->isChecked(),"leaving Edit cannot toggle the timeline presentation");
    page.show();QApplication::processEvents();
    fixed->setEnabled(false); check(!commands.fixedPlayhead() && !fixed->isChecked(),"disabled timeline action is respected");
    timeline.removeAction(fixed);
    QWidget preferences; preferences.setObjectName("btnEditToolbarPreferences");
    QMenu embedded(&preferences); auto custom=new QWidgetAction(&embedded);
    auto content=new QWidget; auto contentLayout=new QVBoxLayout(content);
    contentLayout->addWidget(new QLabel("Fixed Playhead",content)); custom->setDefaultWidget(content);
    custom->setCheckable(true); embedded.addAction(custom);
    check(commands.fixedPlayhead() && custom->isChecked(),"blank native action is identified through its embedded row label");
    custom->setEnabled(false);
    check(!commands.fixedPlayhead() && custom->isChecked(),"disabled embedded native action is respected");
    custom->setEnabled(true); embedded.setProperty("unboundOwnMenu",true);
    check(!commands.fixedPlayhead() && custom->isChecked(),"helper-owned menu cannot impersonate a native target");
    QAction nativeFixed(&page);nativeFixed.setObjectName("viewTimelineScrollingFixed");nativeFixed.setCheckable(true);
    check(commands.fixedPlayhead() && nativeFixed.isChecked(),"exact native Edit View command works without a labeled options row");
    check(commands.fixedPlayhead() && !nativeFixed.isChecked(),"native scrolling command toggles back without clip selection");
    QAction duplicateNative(&page);duplicateNative.setObjectName("viewTimelineScrollingFixed");duplicateNative.setCheckable(true);
    check(!commands.fixedPlayhead()&&!nativeFixed.isChecked()&&!duplicateNative.isChecked(),"duplicate native identities fail closed");
    duplicateNative.setObjectName("unrelated");
    QActionGroup styles(&page);styles.setExclusive(true);styles.addAction(&nativeFixed);
    QAction paged(&page);paged.setObjectName("viewTimelineScrollingPage");paged.setCheckable(true);styles.addAction(&paged);
    QAction disabled(&page);disabled.setObjectName("viewTimelineScrollingDisabled");disabled.setCheckable(true);styles.addAction(&disabled);
    paged.setChecked(true);
    check(commands.fixedPlayhead()&&nativeFixed.isChecked(),"exclusive native scrolling group enters Fixed");
    check(commands.fixedPlayhead()&&paged.isChecked()&&!nativeFixed.isChecked(),"second invocation restores prior Page scrolling style");
    disabled.setChecked(true);
    check(commands.fixedPlayhead()&&nativeFixed.isChecked(),"No Scrolling baseline enters Fixed");
    check(commands.fixedPlayhead()&&disabled.isChecked(),"previous No Scrolling state is restored");
    std::printf("native edit command failures=%d\n",failures); return failures ? 1 : 0;
}
