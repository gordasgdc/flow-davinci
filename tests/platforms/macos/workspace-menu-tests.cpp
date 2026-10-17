// SPDX-License-Identifier: MIT
#include <QApplication>
#include <QTemporaryDir>
#include <cstdio>
#include "workspacearrangements.h"
#include "arrangementcontrols.h"
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
    Unbound::ArrangementStore store; QString first,second;
    check(store.create("Edit & Review",{{"version",1}},first).isEmpty() && store.create("Audio",{{"version",1}},second).isEmpty(), "create stable saved workspace identities");
    QMenu root; root.setObjectName("workspace"); root.setAttribute(Qt::WA_DontShowOnScreen);
    int nativeCalls = 0, restores = 0; QString selected = first;
    const auto configure = [&](Unbound::WorkspaceArrangements &menus) {
        menus.entries = [&] { return store.list(); }; menus.selected = [&] { return selected; };
        menus.nativeWorkspace = [&] { ++nativeCalls; };
        menus.invoke = [&](QString operation,QString id) {
            if (operation == "Restore" && !store.find(id).isEmpty()) { selected = id; ++restores; }
            if (operation == "Delete") { store.remove(id); if (id == selected) selected.clear(); }
            if (operation == "Update") store.update(id,{{"version",2}});
        };
    };
    {
        Unbound::WorkspaceArrangements menus(nullptr); configure(menus); menus.wire(); menus.wire();
        auto submenu = root.findChild<QMenu *>("unboundWorkspaceArrangements");
        auto preset = root.findChild<QMenu *>("unboundWorkspace_"+second);
        check(submenu && submenu->title() == "Custom Workspace Presets" && preset && !root.isVisible(), "native submenu is populated before display");
        check(root.findChild<QMenu *>("unboundWorkspace_"+first)->title() == "Edit && Review", "preset names retain literal ampersands in native menus");
        auto original = preset->findChild<QAction *>("unboundRestoreArrangement");
        original->trigger(); QApplication::processEvents(); menus.wire();
        check(restores == 1 && selected == second && preset->menuAction()->isChecked(), "menu restores stable ID once and synchronizes selection");
        store.rename(second,"Mix"); store.reorder({second,first}); menus.wire();
        QList<QString> ids; for (auto action : submenu->actions()) if (action->menu()) ids.append(action->menu()->property("unboundArrangementId").toString());
        check(preset->title() == "Mix" && preset->findChild<QAction *>("unboundRestoreArrangement") == original && ids == QList<QString>{second,first}, "rename and reorder synchronize without rebuilding action identity");
        original->trigger(); QApplication::processEvents();
        check(restores == 2, "repeated menu synchronization creates no duplicate restore callbacks");
        preset->findChild<QAction *>("unboundUpdateArrangement")->trigger(); QApplication::processEvents();
        check(store.find(second)["workspace"].toObject()["version"] == 2, "menu updates the selected workspace record");
        preset->findChild<QAction *>("unboundArrangement_Delete")->trigger(); QApplication::processEvents();
        check(store.find(second).isEmpty() && root.findChild<QAction *>("unboundRestoreNativeWorkspace"), "deleting a preset cannot delete the native recovery action");
        root.findChild<QAction *>("unboundRestoreNativeWorkspace")->trigger(); QApplication::processEvents();
        check(nativeCalls == 1, "native recovery remains callable after deleting the selected preset");
    }
    {
        Unbound::WorkspaceArrangements reopened(nullptr); configure(reopened); reopened.wire();
        root.findChild<QAction *>("unboundRestoreNativeWorkspace")->trigger(); QApplication::processEvents();
        check(nativeCalls == 2, "native recovery rebinds once after controller recreation");
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
        auto submenu = root.findChild<QMenu *>("unboundWorkspaceArrangements"); int presets = 0, recovery = 0;
        for (auto action : submenu->actions()) if (action->menu()) ++presets;
        for (auto action : root.actions()) if (action->objectName() == "unboundRestoreNativeWorkspace") ++recovery;
        check(presets == 0 && recovery == 1 && root.findChild<QAction *>("unboundSaveWorkspaceAs")->isEnabled(),
            "deleting every preset retains one permanent recovery command and Save As");
    }
    check(Unbound::nativeFrontmostProcessForTest() == foreground, "hidden workspace-menu tests preserve foreground app");
    if (!failures) std::puts("PASS: native workspace preset sync, stable identities, persistent grip ordering and permanent native recovery");
    return failures ? 1 : 0;
}
