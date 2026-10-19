// SPDX-License-Identifier: MIT
#include <QApplication>
#include <QElapsedTimer>
#include <QFile>
#include <QJsonDocument>
#include <QTemporaryDir>
#include <QThread>
#include <cstdio>
#include "panelcontrols.h"
class StandaloneShows : public QObject {
public:
    int count=0;
    bool eventFilter(QObject *object,QEvent *event) override {
        if (event->type()==QEvent::Show && (object->objectName()=="unboundFloating_frameEditPageMediaPool" ||
            object->objectName()=="unboundFloating_widgetEditInspector")) ++count;
        return false;
    }
};
static void settle(int duration=50) {
    QElapsedTimer timer; timer.start();
    do { QApplication::processEvents(); QThread::msleep(1); } while (timer.elapsed()<duration);
    QCoreApplication::sendPostedEvents(nullptr,QEvent::DeferredDelete);
}
int main(int argc,char **argv) {
    QApplication app(argc,argv); app.setQuitOnLastWindowClosed(false); QTemporaryDir files;
    qputenv("UNBOUND_DIAGNOSTICS_DIR",files.path().toUtf8());
    qputenv("UNBOUND_WORKSPACE_FILE",(files.path()+"/Workspace.json").toUtf8());
    qputenv("UNBOUND_ARRANGEMENTS_FILE",(files.path()+"/Arrangements.json").toUtf8());
    int failures=0; auto check=[&](bool pass,const char *message) { if (!pass) { ++failures; std::fprintf(stderr,"FAIL: %s\n",message); } };
    QWidget main; main.setObjectName("UiMainWindow"); auto outer=new QVBoxLayout(&main);
    auto page=new QWidget(&main); page->setObjectName("WStackPage_Conform"); outer->addWidget(page); auto native=new QHBoxLayout(page);
    auto media=new QWidget(page); media->setObjectName("frameEditPageMediaPool"); auto contents=new QVBoxLayout(media);
    auto header=new QWidget(media); auto row=new QHBoxLayout(header); auto options=new QToolButton(header); options->setObjectName("btnMediaPoolOptions");
    auto menu=new QMenu(options); options->setMenu(menu); row->addWidget(options); contents->addWidget(header);
    auto mediaControl=new QPushButton("Native media control",media); contents->addWidget(mediaControl); native->addWidget(media);
    auto remaining=new QPushButton("Native remaining panel",page); native->addWidget(remaining);
    auto inspector=new QWidget(page); inspector->setObjectName("widgetEditInspector");
    auto inspectorLayout=new QVBoxLayout(inspector); auto inspectorHeader=new QWidget(inspector); auto inspectorRow=new QHBoxLayout(inspectorHeader);
    auto inspectorOptions=new QToolButton(inspectorHeader); inspectorOptions->setObjectName("btnTitleOptions"); inspectorRow->addWidget(inspectorOptions); inspectorLayout->addWidget(inspectorHeader);
    auto inspectorControl=new QPushButton("Native Inspector control",inspector); inspectorLayout->addWidget(inspectorControl); native->addWidget(inspector);
    auto callers=new QMenu(&main); callers->setObjectName("workspaceShowPanel");
    main.resize(1100,650); main.show(); settle();
    {
        PanelControls controls; settle(300);
        auto undock=controls.findChild<QPushButton *>("undock_frameEditPageMediaPool");
        check(undock && undock->isEnabled(),"native Media Pool has a callable undock command");
        if (undock) undock->click(); settle();
        check(mediaControl->window()!=&main,"native panel command first floats the actual Media Pool content");
        QMetaObject::invokeMethod(menu,"aboutToShow",Qt::DirectConnection); QAction *dockNative=nullptr;
        for (auto action : menu->actions()) if (action->objectName()=="unboundDockNativeWorkspace") dockNative=action;
        check(dockNative && dockNative->isVisible(),"native options menu exposes Dock in Native Workspace for a floating panel");
        if (dockNative) dockNative->trigger(); settle();
        check(mediaControl->window()==&main && remaining->isVisible() && page->findChild<QWidget *>("unboundNativeArrangement"),
            "native menu command embeds the real panel while preserving remaining native controls");
        const auto frame=main.geometry();
        auto hide=controls.findChild<QPushButton *>("hide_frameEditPageMediaPool");
        auto show=controls.findChild<QPushButton *>("show_frameEditPageMediaPool");
        if (hide) hide->click(); settle(); check(!mediaControl->isVisible() && remaining->isVisible(),"panel manager Hide routes into native arrangement visibility");
        if (show) show->click(); settle();
        if (main.geometry()!=frame) std::fprintf(stderr,"Native UI frame before=%d,%d,%d,%d after=%d,%d,%d,%d\n",frame.x(),frame.y(),frame.width(),frame.height(),
            main.x(),main.y(),main.width(),main.height());
        check(mediaControl->isVisible() && main.geometry()==frame,"panel manager Show recalls native placement without changing the outer frame");
        auto undockInspector=controls.findChild<QPushButton *>("undock_widgetEditInspector");
        if (undockInspector) undockInspector->click(); settle();
        auto target=page->findChild<QWidget *>("unboundGroupCell_frameEditPageMediaPool");
        auto source=dynamic_cast<Unbound::MacFloatingHost *>(inspectorControl->window());
        check(source && source->attachRequested && target,"native arrangement exposes a real cell to floating-panel drop dispatch");
        if (source && source->attachRequested && target) source->attachRequested(target,Qt::Edge(0)); settle();
        check(inspectorControl->window()==&main && inspectorControl->isVisible() && !mediaControl->isVisible(),
            "floating Inspector drop creates a selected native tab beside Media Pool");
        QAction *inspectorCaller=nullptr;
        for (auto action : callers->actions()) if (action->objectName()=="unboundCaller_widgetEditInspector") inspectorCaller=action;
        check(inspectorCaller,"Workspace panel submenu retains an Inspector caller in a native tab group");
        if (inspectorCaller) inspectorCaller->trigger(); settle();
        check(!inspectorControl->isVisible() && mediaControl->isVisible(),"Workspace Inspector command closes its door and returns to Media Pool");
        if (inspectorCaller) inspectorCaller->trigger(); settle();
        check(inspectorControl->isVisible() && !mediaControl->isVisible(),"Workspace Inspector command reveals and selects its retained native tab");
        auto expandMedia=controls.findChild<QPushButton *>("expand_frameEditPageMediaPool");
        if (expandMedia) expandMedia->click(); settle(80);
        check(mediaControl->window()!=&main && inspectorControl->window()==&main,"temporary expansion borrows only the requested native tab's content");
        if (expandMedia) expandMedia->click(); settle();
        check(mediaControl->window()==&main && !mediaControl->isVisible() && inspectorControl->isVisible() && main.geometry()==frame,
            "native maximize restoration returns exact tab selection and outer frame");
        auto hideAll=controls.findChild<QPushButton *>("bulk_3"); if (hideAll) hideAll->click(); settle();
        auto root=page->findChild<QWidget *>("unboundNativeArrangement"); auto split=root ? root->findChild<QSplitter *>() : nullptr;
        check(!mediaControl->isVisible() && !inspectorControl->isVisible() && remaining->isVisible() && split && split->sizes().size()==2 && split->sizes()[1]==0,
            "Hide All collapses the empty native tab branch while preserving remaining native controls");
        if (inspectorCaller) inspectorCaller->trigger(); settle();
        check(inspectorControl->isVisible() && main.geometry()==frame,"Workspace caller reopens an entirely closed native branch without resizing the outer frame");
    }
    settle(); check(media->parentWidget()==page && page->layout()==native,"helper shutdown returns the exact native layout and panel slot");
    QFile saved(files.path()+"/Workspace.json"); saved.open(QIODevice::ReadOnly);
    const auto workspace=QJsonDocument::fromJson(saved.readAll()).object();
    check(workspace["native_arrangement"].toObject()["panels"].toObject().contains("frameEditPageMediaPool"),
        "normal workspace saving retains the native arrangement plan");
    {
        PanelControls controls; settle(400);
        check(mediaControl->window()==&main && page->findChild<QWidget *>("unboundNativeArrangement") && remaining->isVisible(),
            "helper reopening restores saved native placement through ordinary startup commands");
        auto undock=controls.findChild<QPushButton *>("undock_frameEditPageMediaPool");
        check(undock && undock->isEnabled(),"embedded panel retains a discoverable Undock command");
        if (undock) undock->click(); settle();
        check(mediaControl->window()!=&main && mediaControl->isVisible() && inspectorControl->window()==&main,
            "native arrangement Undock reveals its floating content while leaving other native tabs in place");
        auto glyph=main.findChild<QToolButton *>("unboundNativeUndock_widgetEditInspector");
        check(glyph && glyph->isVisible(),"embedded Inspector retains its native header Undock glyph");
        if (glyph) glyph->click(); settle();
        check(inspectorControl->window()!=&main && inspectorControl->isVisible() && !page->findChild<QWidget *>("unboundNativeArrangement"),
            "native header Undock floats the final tab and releases the empty custom arrangement");
        auto first=controls.findChild<QComboBox *>("groupFirst");
        auto second=controls.findChild<QComboBox *>("groupSecond");
        if (first) first->setCurrentIndex(first->findText("Media Pool"));
        if (second) second->setCurrentIndex(second->findText("Inspector"));
        auto tabs=controls.findChild<QPushButton *>("group_3");
        if (tabs) tabs->click(); settle(200);
        check(mediaControl->window()==inspectorControl->window() && mediaControl->window()!=&main,
            "two floating panels form a tabbed group before native transfer");
        StandaloneShows standalone; app.installEventFilter(&standalone);
        QMetaObject::invokeMethod(menu,"aboutToShow",Qt::DirectConnection);
        for (auto action : menu->actions()) if (action->objectName()=="unboundDockNativeWorkspace") action->trigger();
        settle();
        app.removeEventFilter(&standalone);
        check(mediaControl->window()==&main && inspectorControl->window()==&main && page->findChild<QWidget *>("unboundNativeArrangement"),
            "a native menu action retained across helper reopening reconnects to the new helper");
        check(standalone.count==0,"floating group transfer never shows intermediate standalone member windows");
        auto nativeTabs=page->findChild<QTabWidget *>("unboundPanelTabs");
        check(nativeTabs && nativeTabs->count()==2,"native group retains draggable panel tabs");
        if (nativeTabs) {
            auto bar=nativeTabs->tabBar(); const auto start=bar->tabRect(0).center();
            const auto end=QPoint(start.x(),bar->height()+40);
            QMouseEvent press(QEvent::MouseButtonPress,start,bar->mapToGlobal(start),Qt::LeftButton,Qt::LeftButton,Qt::NoModifier);
            QMouseEvent move(QEvent::MouseMove,end,bar->mapToGlobal(end),Qt::NoButton,Qt::LeftButton,Qt::NoModifier);
            QApplication::sendEvent(bar,&press); QApplication::sendEvent(bar,&move); settle();
            const bool mediaFloating=mediaControl->window()!=&main;
            const bool inspectorFloating=inspectorControl->window()!=&main;
            check(mediaFloating!=inspectorFloating,"dragging a native tab outside its strip floats only that panel");
            if (auto host=dynamic_cast<Unbound::MacFloatingHost *>((mediaFloating ? mediaControl : inspectorControl)->window())) {
                QMouseEvent release(QEvent::MouseButtonRelease,QPoint(80,12),host->mapToGlobal(QPoint(80,12)),Qt::LeftButton,Qt::NoButton,Qt::NoModifier);
                QApplication::sendEvent(host,&release);
            }
            settle();
        }
    }
    Unbound::flushDiagnostics(); QFile events(files.path()+"/events.jsonl"); events.open(QIODevice::ReadOnly); const auto trace=events.readAll();
    check(trace.contains("workspace.native-transfer.request") && trace.contains("workspace.native-command.complete") &&
        trace.contains("workspace.native-plan.restore.request") && trace.contains("workspace.native-arrangement.presentation"),
        "connected native UI commands retain transfer, completion, restore and presentation diagnostics");
    if (!failures) std::puts("PASS: native menu docking, panel doors, workspace saving, helper reopen and native undock commands");
    return failures ? 1 : 0;
}
