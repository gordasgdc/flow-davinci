// SPDX-License-Identifier: MIT
#include <QApplication>
#include <QTemporaryDir>
#include <cstdio>
#include "workspacearrangements.h"
#include "nativepaneldecoration.h"
#include "arrangementcontrols.h"
#include "pagecontext.h"
int main(int argc, char **argv)
{
    qputenv("QT_MAC_DISABLE_FOREGROUND_APPLICATION_TRANSFORM", "1");
    const auto foreground = Unbound::nativeFrontmostProcessForTest();
    QApplication app(argc,argv); app.setQuitOnLastWindowClosed(false);
    QTemporaryDir temporary;
    qputenv("UNBOUND_ARRANGEMENTS_FILE", (temporary.path()+"/Arrangements.json").toUtf8());
    qputenv("UNBOUND_DIAGNOSTICS_DIR", temporary.path().toUtf8());
    int failures = 0;
    const auto check = [&](bool pass,const char *message) { if (!pass) { ++failures; std::fprintf(stderr,"FAIL: %s\n",message); } };
    {
        QWidget main; main.setAttribute(Qt::WA_DontShowOnScreen);
        auto pages = new QStackedWidget(&main); (new QVBoxLayout(&main))->addWidget(pages);
        auto edit = new QWidget(pages); edit->setObjectName("WStackPage_Conform"); pages->addWidget(edit);
        auto color = new QWidget(pages); color->setObjectName("WStackPage_Color"); pages->addWidget(color);
        auto audio = new QWidget(pages); audio->setObjectName("WStackPage_Audio"); pages->addWidget(audio);
        main.show();
        check(Unbound::currentWorkspacePage(&main) == "edit","current page follows native Edit widget visibility");
        pages->setCurrentWidget(color);
        check(Unbound::currentWorkspacePage(&main) == "color","page identity follows the active native stack without a key mapping");
        pages->setCurrentWidget(audio);
        check(Unbound::currentWorkspacePage(&main) == "fairlight","native Audio page maps to the Fairlight menu label");
        main.hide();
        check(Unbound::currentWorkspacePage(&main).isEmpty(),"hidden or unavailable pages are not guessed to be Edit");
    }
    Unbound::ArrangementStore store; QString first,second;
    check(store.create("Edit & Review",{{"version",1}},first).isEmpty() && store.create("Audio",{{"version",1}},second).isEmpty(), "create stable saved workspace identities");
    {
        QWidget nativePanel; auto row = new QHBoxLayout(&nativePanel);
        auto options = new QToolButton(&nativePanel); options->setObjectName("btnMediaPoolOptions"); row->addWidget(options);
        Unbound::NativePanelDecoration decoration(&nativePanel); int toggles=0;
        decoration.wire(&nativePanel,"pool","Media Pool",false,[&] { ++toggles; });
        decoration.wire(&nativePanel,"pool","Media Pool",true,[&] { ++toggles; });
        auto button = nativePanel.findChild<QToolButton *>("unboundNativeUndock_pool");
        check(button && row->indexOf(button)+2 == row->indexOf(options),"undock glyph occupies its own native layout slot before options");
        check(nativePanel.findChildren<QToolButton *>("unboundNativeUndock_pool").size() == 1,"native header rewiring cannot duplicate the button");
        if (button) button->click();
        check(toggles == 1 && button->toolTip() == "Dock Media Pool","native header button dispatches once and reflects docking state");
        int drags=0;
        decoration.wire(&nativePanel,"pool","Media Pool",true,[&] { ++toggles; },[&](QPoint) { ++drags; });
        QMouseEvent press(QEvent::MouseButtonPress,QPointF(4,4),QPointF(100,100),Qt::LeftButton,Qt::LeftButton,Qt::NoModifier);
        const int distance=QApplication::startDragDistance()+10;
        QMouseEvent move(QEvent::MouseMove,QPointF(4+distance,4),QPointF(100+distance,100),Qt::NoButton,Qt::LeftButton,Qt::NoModifier);
        QMouseEvent release(QEvent::MouseButtonRelease,QPointF(4,4),QPointF(100,100),Qt::LeftButton,Qt::NoButton,Qt::NoModifier);
        QApplication::sendEvent(button,&press); QApplication::sendEvent(button,&move); QApplication::sendEvent(button,&release);
        check(drags==1 && toggles==1,"native undock drag crosses threshold once and cannot become a dock click on release");
    }
    {
        QMenu nativeCommands; int opened=0;
        auto keyboard = nativeCommands.addAction(QString::fromUtf8("Keyboard Customization…"));
        QObject::connect(keyboard,&QAction::triggered,[&] { ++opened; });
        check(Unbound::openNativeKeyboardCustomization() && opened==1,"keyboard launcher invokes the native action once");
        keyboard->setEnabled(false);
        check(!Unbound::openNativeKeyboardCustomization() && opened==1,"disabled native keyboard command is not invoked");
        keyboard->setEnabled(true);
        QMenu ambiguous; ambiguous.addAction("Keyboard Customization...");
        check(!Unbound::openNativeKeyboardCustomization() && opened==1,"ambiguous native keyboard actions are rejected");
    }
    {
        QWidget panel; auto row=new QHBoxLayout(&panel); auto options=new QToolButton(&panel);
        options->setObjectName("btnMediaPoolOptions"); row->addWidget(options);
        { Unbound::NativePanelDecoration decoration(nullptr); decoration.wire(&panel,"lifetime","Media Pool",false,[] {}); }
        check(panel.findChildren<QToolButton *>().size()==1 && options->parentWidget()==&panel,
            "destroying header decoration removes owned buttons and stale drag callbacks while preserving native options");
    }
    QMenuBar bar; bar.setAttribute(Qt::WA_DontShowOnScreen);
    QMenu workspace(&bar); workspace.setObjectName("workspace"); bar.addMenu(&workspace); bar.addMenu("Help");
    QMenu *root = nullptr;
    int managerCalls = 0;
    int nativeCalls = 0, restores = 0; QString selected = first, activePage = "edit", nativePage;
    const auto configure = [&](Unbound::WorkspaceArrangements &menus) {
        menus.entries = [&] { return store.list(); }; menus.selected = [&] { return selected; };
        menus.currentPage = [&] { return activePage; };
        menus.nativeWorkspace = [&](QString page) { ++nativeCalls; nativePage = page; };
        menus.invoke = [&](QString operation,QString id) {
            if (operation == "Restore" && !store.find(id).isEmpty()) { selected = id; ++restores; }
            if (operation == "Delete") { store.remove(id); if (id == selected) selected.clear(); }
            if (operation == "Update") store.update(id,{{"version",2}});
        };
    };
    {
        Unbound::WorkspaceArrangements menus(nullptr); configure(menus); menus.wire(); menus.wire();
        root = bar.findChild<QMenu *>("unboundWorkspaceUI");
        check(root && bar.actions().indexOf(root->menuAction()) == bar.actions().indexOf(workspace.menuAction())+1,
            "Workspace UI is a separate top-level menu beside Workspace");
        if (!root) return 1;
        auto submenu = root->findChild<QMenu *>("unboundWorkspaceArrangements");
        auto preset = root->findChild<QMenu *>("unboundWorkspace_"+second);
        check(submenu && submenu->title() == "Manage Saved Workspaces" && preset && !root->isVisible(), "menu is populated before native display");
        auto manager = root->findChild<QAction *>("unboundShowPanelManager");
        check(manager && !manager->isEnabled(), "manager action is disabled without an owner callback");
        menus.showPanelManager = [&] { ++managerCalls; };
        menus.wire();
        manager->trigger(); QApplication::processEvents();
        check(managerCalls == 1, "Workspace UI opens the panel manager through its owner callback");
        check(root->actions().first()->text() == "Native Edit UI" && !workspace.findChild<QAction *>("unboundRestoreNativeWorkspace"),
            "native Edit recovery is first in Workspace UI and separate from native Workspace commands");
        check(root->findChild<QMenu *>("unboundWorkspace_"+first)->title() == "Edit && Review", "preset names retain literal ampersands in native menus");
        auto original = root->findChild<QAction *>("unboundWorkspaceRestore_"+second,Qt::FindDirectChildrenOnly);
        original->trigger(); QApplication::processEvents(); menus.wire();
        check(restores == 1 && selected == second && preset->menuAction()->isChecked(), "menu restores stable ID once and synchronizes selection");
        store.rename(second,"Mix"); store.reorder({second,first}); menus.wire();
        QList<QString> ids; for (auto action : submenu->actions()) if (action->menu()) ids.append(action->menu()->property("unboundArrangementId").toString());
        check(preset->title() == "Mix" && root->findChild<QAction *>("unboundWorkspaceRestore_"+second,Qt::FindDirectChildrenOnly) == original && ids == QList<QString>{second,first}, "rename and reorder synchronize without rebuilding action identity");
        original->trigger(); QApplication::processEvents();
        check(restores == 2, "repeated menu synchronization creates no duplicate restore callbacks");
        preset->findChild<QAction *>("unboundUpdateArrangement")->trigger(); QApplication::processEvents();
        check(store.find(second)["workspace"].toObject()["version"] == 2, "menu updates the selected workspace record");
        preset->findChild<QAction *>("unboundArrangement_Delete")->trigger(); QApplication::processEvents();
        check(store.find(second).isEmpty() && root->findChild<QAction *>("unboundRestoreNativeWorkspace"), "deleting a preset cannot delete the native recovery action");
        root->findChild<QAction *>("unboundRestoreNativeWorkspace")->trigger(); QApplication::processEvents();
        check(nativeCalls == 1, "native recovery remains callable after deleting the selected preset");
        QString color; store.create("Color Review",{{"page","color"},{"version",1}},color);
        activePage = "color"; menus.wire();
        auto colorRecall = root->findChild<QAction *>("unboundWorkspaceRestore_"+color,Qt::FindDirectChildrenOnly);
        check(root->actions().first()->text() == "Native Color UI" && colorRecall && root->actions().contains(colorRecall) &&
            !root->actions().contains(root->findChild<QAction *>("unboundWorkspaceRestore_"+first,Qt::FindDirectChildrenOnly)),
            "page switching relabels native recovery and filters saved workspaces without mixing Edit and Color");
        check(!colorRecall->isEnabled(),"unimplemented page backends cannot restore Edit content under another page's name");
        root->actions().first()->trigger(); QApplication::processEvents();
        check(nativeCalls == 2 && nativePage == "color","native recovery receives the page shown in its label");
        menus.supportedPage = [](QString) { return true; }; menus.wire();
        colorRecall->trigger(); activePage = "edit"; QApplication::processEvents();
        check(restores == 2,"queued page-specific commands cannot run after switching to another page");
        store.remove(color); menus.wire();
    }
    {
        Unbound::WorkspaceArrangements reopened(nullptr); configure(reopened); reopened.wire();
        root->findChild<QAction *>("unboundRestoreNativeWorkspace")->trigger(); QApplication::processEvents();
        check(nativeCalls == 3, "native recovery rebinds once after controller recreation");
    }
    Unbound::ArrangementStore reloaded;
    check(reloaded.list().size() == 1 && reloaded.list()[0].toObject()["id"] == first, "saved deletion and order survive store reload");
    QString third; check(reloaded.create("Third",{{"version",1}},third).isEmpty(), "prepare widget reorder fixture");
    {
        QWidget owner; owner.setAttribute(Qt::WA_DontShowOnScreen); auto layout = new QVBoxLayout(&owner);
        Unbound::ArrangementControls controls(&owner,layout,first);
        controls.workspace = [] { return QJsonObject{{"version",3}}; }; controls.report = [](QString) {};
        int updates = 0; controls.changed = [&] { ++updates; };
        controls.invoke("Reorder"); auto order = owner.findChild<Unbound::ArrangementOrder *>("arrangementOrder");
        check(order && !order->isHidden() && order->count() == 2, "Unbound widget exposes reorderable grip rows");
        check(order->model()->moveRows(QModelIndex(),0,1,QModelIndex(),2), "grip list model accepts a real row move");
        QApplication::processEvents();
        Unbound::ArrangementStore reordered;
        check(updates == 1 && reordered.list()[0].toObject()["id"] == third && controls.selection()->currentData().toString() == first,
            "widget row move persists order while retaining the selected workspace identity");
        check(!reordered.reorder({third,third}).isEmpty(), "invalid reorder cannot duplicate or erase saved workspaces");
    }
    store = Unbound::ArrangementStore();
    for (const auto &entry : store.list()) store.remove(entry.toObject()["id"].toString());
    {
        Unbound::WorkspaceArrangements empty(nullptr); configure(empty); empty.wire();
        auto submenu = root->findChild<QMenu *>("unboundWorkspaceArrangements"); int presets = 0, recovery = 0;
        for (auto action : submenu->actions()) if (action->menu()) ++presets;
        for (auto action : root->actions()) if (action->objectName() == "unboundRestoreNativeWorkspace") ++recovery;
        check(presets == 0 && recovery == 1 && root->findChild<QAction *>("unboundSaveWorkspaceAs")->isEnabled(),
            "deleting every preset retains one permanent recovery command and Save As");
    }
    check(Unbound::nativeFrontmostProcessForTest() == foreground, "hidden workspace-menu tests preserve foreground app");
    if (!failures) std::puts("PASS: native workspace preset sync, stable identities, persistent grip ordering and permanent native recovery");
    return failures ? 1 : 0;
}
