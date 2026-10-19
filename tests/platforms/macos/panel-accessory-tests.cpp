// SPDX-License-Identifier: MIT
#include <QApplication>
#include <QPushButton>
#include <QTemporaryDir>
#include <cstdio>
#include "footerpanel.h"

int main(int argc,char **argv)
{
    qputenv("QT_MAC_DISABLE_FOREGROUND_APPLICATION_TRANSFORM","1");
    QApplication app(argc,argv); app.setQuitOnLastWindowClosed(false);
    QTemporaryDir files; qputenv("UNBOUND_DIAGNOSTICS_DIR",files.path().toUtf8());
    int failures=0;
    const auto check=[&](bool ok,const char *why) { if (!ok) { ++failures; std::fprintf(stderr,"FAIL: %s\n",why); } };
    QWidget main; main.setAttribute(Qt::WA_DontShowOnScreen); auto native=new QVBoxLayout(&main);
    auto timeline=new QWidget(&main); timeline->setObjectName("frameConformTimelines");
    auto content=new QVBoxLayout(timeline); content->addWidget(new QPushButton("Timeline content",timeline)); native->addWidget(timeline);
    QPointer<QWidget> footer=new QWidget(&main); footer->setObjectName("m_PageSwitcherContainer");
    footer->setMinimumWidth(800); footer->setFixedHeight(32); auto row=new QHBoxLayout(footer);
    row->addWidget(new QPushButton("Native page action",footer)); native->addWidget(footer);
    main.resize(1000,500); main.show(); QApplication::processEvents();
    const auto policy=footer->sizePolicy(); const auto fill=footer->autoFillBackground();
    {
        Unbound::PanelAccessoryLease lease(nullptr);
        check(lease.attach(footer,&main,timeline).isEmpty(),"native footer attaches to owner without a floating window");
        QWidget carrier; carrier.setAttribute(Qt::WA_DontShowOnScreen); auto carried=new QVBoxLayout(&carrier);
        carried->addWidget(timeline); carrier.resize(400,350); carrier.show(); QApplication::processEvents();
        check(footer && footer->window()==&carrier && lease.active(),"moving owner carries the actual native footer through Qt ownership");
        auto viewport=timeline->findChild<QScrollArea *>("unboundAttachedAccessory_m_PageSwitcherContainer");
        check(viewport && viewport->horizontalScrollBar()->maximum()>0 && carrier.width()==400,
            "wide native footer remains reachable without forcing owner width");
        lease.setHidden(true); check(lease.hidden(),"footer door collapses its attached surface");
        lease.setHidden(false); check(!lease.hidden(),"footer door reopens without detaching");
        check(lease.release() && footer->parentWidget()==&main && native->indexOf(footer)==0 && native->count()==1,
            "release restores exact original native layout slot");
        check(footer->minimumWidth()==800 && footer->sizePolicy()==policy && footer->autoFillBackground()==fill,
            "release restores native metrics and background behavior");
        native->insertWidget(0,timeline);
        auto unsupported=new QWidget(&main); auto horizontal=new QHBoxLayout(unsupported); horizontal->addStretch();
        check(!lease.attach(footer,&main,unsupported).isEmpty() && footer->parentWidget()==&main,
            "unsupported attachment layout leaves native content untouched"); delete unsupported;
        check(lease.attach(footer,&main,timeline).isEmpty(),"accessory can attach again after restoration");
        QWidget foreign(&main); footer->setParent(&foreign);
        check(!lease.release() && footer->parentWidget()==&foreign,
            "unexpected ownership rejects restoration without stealing foreign content");
        viewport=timeline->findChild<QScrollArea *>("unboundAttachedAccessory_m_PageSwitcherContainer");
        viewport->takeWidget(); viewport->setWidget(footer); footer->show();
        delete timeline; timeline=nullptr;
        check(footer && footer->parentWidget()==&main && !lease.active(),"owner destruction restores footer instead of deleting native content");
    }
    timeline=new QWidget(&main); timeline->setObjectName("frameConformTimelines"); new QVBoxLayout(timeline);
    native->insertWidget(0,timeline); timeline->show(); QApplication::processEvents();
    {
        bool available=true;QWidget *owner=timeline;
        Unbound::FooterPanel panel(nullptr,[&] { return &main; },{});
        panel.attachmentOwner=[&]() -> QWidget * { return available ? owner : nullptr; };
        QMenu workspace(&main);auto navigation=workspace.addAction("Show Page Navigation");navigation->setCheckable(true);
        navigation->setShortcut(QKeySequence(Qt::Key_F12));
        panel.restore();
        check(navigation->isChecked(),"native Page Navigation action reflects the attached footer");
        navigation->trigger();QApplication::processEvents();
        check(panel.snapshot()["hidden"].toBool(),"native Page Navigation action hides the attached footer");
        navigation->trigger();QApplication::processEvents();
        check(!panel.snapshot()["hidden"].toBool(),"native Page Navigation action restores the attached footer");
        check(panel.snapshot()["attachment"]=="workspace" && timeline->isAncestorOf(footer),"default Page Navigation travels with its available workspace owner");
        // Never enter the floating fallback in this hidden-only fixture.
        if (!footer || !timeline->isAncestorOf(footer)) return 1;
        panel.toggleVisibility(); const auto saved=panel.snapshot();
        check(saved["hidden"].toBool() && !saved["floating"].toBool(),"attached hidden door persists independently of floating state");
        available=false; panel.syncAttachment();
        check(footer->parentWidget()==&main,"leaving Edit returns Page Navigation to its native position");
        available=true; panel.syncAttachment();
        check(timeline->isAncestorOf(footer) && panel.snapshot()["hidden"].toBool(),"returning owner restores attachment and hidden door");
        QWidget other(&main);auto otherLayout=new QVBoxLayout(&other);other.show();
        owner=&other;panel.syncAttachment();
        check(other.isAncestorOf(footer) && panel.snapshot()["hidden"].toBool() && otherLayout->count()==1,
            "switching workspace owners moves the same footer and preserves its hidden state");
        owner=timeline;panel.syncAttachment();
        check(timeline->isAncestorOf(footer) && otherLayout->count()==0,"old workspace retains no orphaned footer viewport");
        panel.dock(); panel.syncAttachment();
        check(footer->parentWidget()==&main && panel.snapshot()["attachment"]=="native","explicit native placement suppresses automatic attachment");
        panel.hidePanel(); check(footer->isHidden(),"manager can hide native Page Navigation");
        check(panel.showPanel().isEmpty() && footer->isVisible() && footer->parentWidget()==&main,
            "manager reopens native Page Navigation without floating or reparenting it");
        panel.setState(saved);
        check(timeline->isAncestorOf(footer) && panel.snapshot()["hidden"].toBool(),"saved attachment restores without invoking floating geometry");
        auto legacy=saved;legacy["attachment"]="timeline";panel.setState(legacy);
        check(panel.snapshot()["attachment"]=="workspace","legacy Timeline attachment migrates on load without rewriting presets");
        available=false;panel.setState({});panel.hidePanel();panel.showPanel();
        check(footer->parentWidget()==&main && footer->isVisible() && !panel.floatingWindow(),
            "unavailable modular workspace uses the native footer instead of unexpectedly floating");
        check(panel.releaseAttachment(),"shutdown can return attached footer without changing saved preference");
    }
    Unbound::flushDiagnostics();
    if (!failures) std::puts("PASS: accessory ownership, overflow, native restoration, owner destruction and footer attachment state");
    return failures ? 1 : 0;
}
