// SPDX-License-Identifier: MIT
#include <QApplication>
#include <QPushButton>
#include <QTemporaryDir>
#include <cstdio>
#include "nativegroupslot.h"
#include "nativerootlayout.h"
#include "panellayoutsurface.h"
#include "panelsession.h"
#include "nativechrome.h"

int main(int argc,char **argv) {
    QApplication app(argc,argv); app.setQuitOnLastWindowClosed(false);
    QTemporaryDir files; qputenv("UNBOUND_DIAGNOSTICS_DIR",files.path().toUtf8());
    int failures=0;
    const auto check=[&](bool pass,const char *message) { if (!pass) { ++failures; std::fprintf(stderr,"FAIL: %s\n",message); } };
    {
        QWidget main; auto layout=new QVBoxLayout(&main);
        auto slot=new Unbound::NativePanelSlot(&main); slot->setObjectName("unboundVacatedSlot_fixture"); layout->addWidget(slot); slot->hide();
        auto surface=new QWidget(&main); auto controls=new QHBoxLayout(surface); auto button=new QPushButton("Native control",surface); controls->addWidget(button);
        const auto minimum=slot->minimumSize(); const auto policy=slot->sizePolicy(); const bool hidden=slot->isHidden();
        main.resize(700,400); main.show(); QApplication::processEvents();
        const auto key=Unbound::nativeKeyWindowNumber();
        Unbound::NativeGroupSlot mount;
        check(mount.mount(slot,surface,&main).isEmpty(),"owned native layout slot accepts an embedded arrangement"); QApplication::processEvents();
        check(surface->window()==&main && !surface->isWindow() && slot->isVisible() && button->isVisible(),
            "embedded content remains real controls inside the original native window");
        check(Unbound::nativeKeyWindowNumber()==key,"embedding does not activate another native window");
        check(slot->minimumSizeHint().width()>=button->minimumSizeHint().width(),"embedded arrangement supplies the native slot's usable size floor");
        mount.setVisible(false); check(slot->isHidden(),"all-hidden embedded group can collapse its native slot");
        mount.setVisible(true); check(!slot->isHidden(),"revealing an embedded group returns its same native slot");
        mount.release();
        check(!slot->layout() && slot->minimumSize()==minimum && slot->sizePolicy()==policy && slot->isHidden()==hidden,
            "unmount restores the exact empty-slot constraints and visibility");
        check(button && surface->parentWidget()==&main && !surface->isWindow(),"unmount never deletes or floats native content implicitly");
    }
    {
        QWidget main; QWidget nativePanel(&main),surface(&main);
        Unbound::NativeGroupSlot mount;
        check(!mount.mount(&nativePanel,&surface,&main).isEmpty() && !nativePanel.layout() && surface.parentWidget()==&main,
            "unrecognized native panels cannot be overwritten as restoration slots");
        Unbound::NativePanelSlot slot(&main); auto existing=new QHBoxLayout(&slot);
        check(!mount.mount(&slot,&surface,&main).isEmpty() && slot.layout()==existing,
            "existing native layouts are rejected without ownership changes");
        check(!mount.mount(&slot,&slot,&main).isEmpty(),"cyclic native placement is rejected");
    }
    {
        QWidget main; auto outer=new QVBoxLayout(&main); auto page=new QWidget(&main); outer->addWidget(page);
        auto original=new QHBoxLayout(page); auto nativeControl=new QPushButton("Native original",page); original->addWidget(nativeControl);
        auto unmanaged=new QPushButton("Native unmanaged",page); unmanaged->setGeometry(0,0,50,20);
        auto custom=new QWidget(&main); auto controls=new QVBoxLayout(custom); auto arranged=new QPushButton("Arranged native control",custom); controls->addWidget(arranged);
        page->setMinimumSize(500,200); const auto minimum=page->minimumSize(); const auto policy=page->sizePolicy();
        main.show(); QApplication::processEvents(); const bool unmanagedHidden=unmanaged->isHidden();
        const auto key=Unbound::nativeKeyWindowNumber(); Unbound::NativeRootLayout lease;
        check(lease.mount(page,custom).isEmpty(),"native page layout can be leased without replacing its main window"); QApplication::processEvents();
        check(page->layout()!=original && !nativeControl->isVisible() && !unmanaged->isVisible() && arranged->isVisible(),
            "custom arrangement occupies one workspace without displaying its parked native layout behind it");
        check(Unbound::nativeKeyWindowNumber()==key && custom->window()==&main,"native root transition retains the same native window and focus");
        check(lease.restore(),"native page layout restores"); QApplication::processEvents();
        check(page->layout()==original && nativeControl->parentWidget()==page && nativeControl->isVisible() &&
            unmanaged->isHidden()==unmanagedHidden && page->minimumSize()==minimum && page->sizePolicy()==policy,
            "native root recovery returns the original layout object, controls, visibility and constraints");
        check(arranged->parentWidget()==custom && custom->parentWidget()==&main && !custom->isWindow(),
            "restoring the native root retains the custom tree for its caller to release safely");
    }
    {
        QWidget main; auto page=new QWidget(&main); auto original=new QVBoxLayout(page);
        QPointer<QWidget> native=new QPushButton("Native owner lifetime",page); original->addWidget(native);
        auto custom=new QWidget(&main); auto layout=new QVBoxLayout(custom);
        QPointer<QWidget> arranged=new QPushButton("Arranged lifetime",custom); layout->addWidget(arranged);
        Unbound::NativeRootLayout lease; check(lease.mount(page,custom).isEmpty(),"native root destruction setup");
        delete page; QCoreApplication::sendPostedEvents(nullptr,QEvent::DeferredDelete);
        check(!native && !arranged && !lease.active() && !main.findChild<QWidget *>("unboundNativeLayoutStash"),
            "closing a native page destroys both its current and parked content without orphaned controls");
    }
    {
        QWidget main; QWidget page(&main); auto original=new QVBoxLayout(&page);
        QPointer<QWidget> native=new QPushButton("Native retained",&page); original->addWidget(native);
        auto custom=new QWidget(&main); auto customLayout=new QVBoxLayout(custom);
        QPointer<QWidget> arranged=new QPushButton("Arranged retained",custom); customLayout->addWidget(arranged);
        Unbound::NativeRootLayout lease; check(lease.mount(&page,custom).isEmpty(),"foreign layout replacement setup");
        QWidget parking(&main); parking.setLayout(page.layout()); auto foreign=new QHBoxLayout(&page);
        check(!lease.restore() && page.layout()==foreign && native && arranged,
            "unexpected native layout replacement rejects restoration without deleting either control tree");
        delete foreign; page.setLayout(parking.layout());
        check(lease.restore() && page.layout()==original,"restoration can retry when expected layout ownership returns");
    }
    {
        auto main=new QWidget; auto slot=new Unbound::NativePanelSlot(main); auto surface=new QWidget(main);
        QPointer<QWidget> content=new QPushButton("Native lifetime",surface);
        Unbound::NativeGroupSlot mount; check(mount.mount(slot,surface,main).isEmpty(),"native lifetime mount setup");
        delete main; mount.release();
        check(!content && !mount.active(),"native owner destruction cannot orphan or resurrect its panel content");
    }
    {
        QWidget main; QWidget page(&main); auto original=new QVBoxLayout(&page);
        auto custom=new QWidget(&main); auto customLayout=new QVBoxLayout(custom);
        QHash<QString,QWidget *> leaves;
        for (const auto &id : {QStringLiteral("source"),QStringLiteral("inspector"),QStringLiteral("program")})
            leaves[id]=new QPushButton(id,custom);
        Unbound::PanelLayoutCallbacks callbacks;
        callbacks.leaf=[&](const QString &id) { return leaves.value(id); };
        callbacks.title=[](const QString &id) { return id; };
        const QJsonObject tree{{"orientation","vertical"},{"children",QJsonArray{
            QJsonObject{{"orientation","tabs"},{"selected",1},{"children",QJsonArray{
                QJsonObject{{"panel","source"}},QJsonObject{{"panel","inspector"}}}}},
            QJsonObject{{"panel","program"}}}}};
        auto rendered=Unbound::buildPanelLayoutSurface(tree,custom,custom,callbacks);
        check(rendered,"shared split/tab renderer accepts a native arrangement tree");
        if (rendered) {
            customLayout->addWidget(rendered); Unbound::NativeRootLayout lease;
            check(lease.mount(&page,custom).isEmpty(),"rendered nested arrangement mounts inside native page");
            auto tabs=custom->findChild<QTabWidget *>();
            check(tabs && tabs->currentWidget()==leaves.value("inspector") &&
                leaves.value("program")->window()==&main,"native arrangement preserves selected tab and common native window");
            check(lease.restore() && page.layout()==original &&
                leaves.value("source")->parentWidget() && custom->isAncestorOf(leaves.value("program")),
                "native recovery preserves all nested real controls for session restoration");
        }
    }
    {
        QWidget main; auto outer=new QVBoxLayout(&main); auto page=new QWidget(&main); outer->addWidget(page);
        auto nativeLayout=new QVBoxLayout(page); auto panel=new QPushButton("Native session control",page);
        nativeLayout->addWidget(panel); main.show(); QApplication::processEvents();
        Unbound::MacPanelSession session;
        check(session.undock(panel,page,"Native session").isEmpty(),"native arrangement session starts with a reversible native slot");
        auto custom=new QWidget(&main); auto row=new QVBoxLayout(custom); auto cell=new QWidget(custom); row->addWidget(cell);
        QWidget unrelated(&main);
        check(!session.attachToGroup(&unrelated).isEmpty(),"unregistered native children cannot adopt panel sessions");
        Unbound::NativeRootLayout lease; check(lease.mount(page,custom).isEmpty(),"native session root mount");
        check(session.attachToGroup(cell).isEmpty(),"registered native arrangement adopts actual session content");
        session.finishGroupTransfer(); QApplication::processEvents();
        check(session.placement()==Unbound::PanelPlacement::NativeSurface && panel->window()==&main && panel->isVisible(),
            "embedded session reports native placement and retains its working control in the original window");
        const auto frame=main.geometry();
        session.setHiddenByUser(true); session.setHiddenByUser(false); QApplication::processEvents();
        check(session.placement()==Unbound::PanelPlacement::NativeSurface && main.geometry()==frame && panel->isVisible(),
            "native session door visibility retains its placement and outer window geometry");
        check(session.beginGroupedExpansion(),"native embedded session can temporarily expand without relinquishing its native slot");
        session.endGroupedExpansion(); QApplication::processEvents();
        check(session.window()==cell && panel->window()==&main && cell->layout(),
            "temporary expansion returns the actual control layout to its same embedded cell");
        session.detachFromGroup();
        check(session.floating() && panel->window()!=&main && lease.active(),
            "detaching one session leaves the native arrangement owner intact");
        check(lease.restore() && !Unbound::NativeRootLayout::contains(page,cell),"native restoration revokes the arrangement adoption scope");
        check(session.dock().isEmpty() && panel->parentWidget()==page && page->layout()==nativeLayout,
            "panel session returns to the exact restored native layout after arrangement recovery");
    }
    if (!failures) std::puts("PASS: embedded native layout ownership, constraints, visibility and destruction");
    return failures ? 1 : 0;
}
