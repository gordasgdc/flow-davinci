// SPDX-License-Identifier: MIT
#include <QApplication>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QTemporaryDir>
#include <QPainter>
#include <cstdio>
#include "nativeeditcommands.h"
class PaintedMenuRow : public QWidget {
public:
    explicit PaintedMenuRow(QString label):caption(std::move(label)) { resize(320,32); }
    QString caption;
protected:
    void paintEvent(QPaintEvent *) override { QPainter painter(this); painter.drawText(rect(),Qt::AlignCenter,caption); }
};
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
    contentLayout->takeAt(0)->widget()->deleteLater();QApplication::sendPostedEvents(nullptr,QEvent::DeferredDelete);
    content->setAccessibleName("Fixed Playhead");
    check(commands.fixedPlayhead()&&!custom->isChecked(),"custom-painted native row is identified by accessibility metadata without text children");
    check(commands.fixedPlayhead()&&custom->isChecked(),"semantic row retains both toggle states");
    custom->setEnabled(false);
    check(!commands.fixedPlayhead() && custom->isChecked(),"disabled embedded native action is respected");
    custom->setEnabled(true); embedded.setProperty("unboundOwnMenu",true);
    check(!commands.fixedPlayhead() && custom->isChecked(),"helper-owned menu cannot impersonate a native target");
    QAction nativeFixed(&page);nativeFixed.setObjectName("viewTimelineScrollingFixed");nativeFixed.setCheckable(true);
    check(!commands.fixedPlayhead()&&!nativeFixed.isChecked(),"global scrolling QAction cannot substitute for real Edit settings");
    embedded.setProperty("unboundOwnMenu",false);
    embedded.removeAction(custom);delete custom;
    custom=new QWidgetAction(&embedded);
    auto painted=new PaintedMenuRow("Fixed Playhead"); custom->setDefaultWidget(painted);custom->setCheckable(true);embedded.addAction(custom);
    bool actualMode=false;int nativeCalls=0;
    QObject::connect(custom,&QAction::triggered,[&] { actualMode=!actualMode;++nativeCalls; });
    page.hide();preferences.hide();embedded.hide();painted->hide();
    bool editActive=true;commands.editActive=[&] {return editActive;};
    auto focusBefore=QApplication::focusWidget();
    check(commands.fixedPlayhead()&&actualMode&&nativeCalls==1,"hidden custom-painted native row invokes real settings handler");
    check(!page.isVisible()&&!embedded.isVisible()&&!painted->isVisible(),"invocation does not show timeline, settings menu or its row");
    check(QApplication::focusWidget()==focusBefore,"invocation does not move focus");
    check(commands.fixedPlayhead()&&!actualMode&&nativeCalls==2,"same hidden handler switches Fixed Playhead off");
    editActive=false;
    check(!commands.fixedPlayhead()&&nativeCalls==2,"active page contract prevents invocation outside Edit");
    editActive=true;
    painted->caption="Display Audio Waveforms";
    const bool result=commands.fixedPlayhead();
    check(!result&&nativeCalls==2,"different painted caption cannot be treated as Fixed Playhead");
    std::printf("native edit command failures=%d\n",failures); return failures ? 1 : 0;
}
