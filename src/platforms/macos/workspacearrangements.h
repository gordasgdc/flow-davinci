// SPDX-License-Identifier: MIT
#pragma once
#include <QApplication>
#include <QJsonArray>
#include <QJsonObject>
#include <QMenu>
#include <QSet>
#include <QTimer>
#include <functional>
#include "diagnostics.h"

namespace Unbound {
// Native menus and the controls window dispatch to the same arrangement owner.
// The native recovery action is outside the removable preset collection.
class WorkspaceArrangements : public QObject
{
public:
    explicit WorkspaceArrangements(QObject *owner) : QObject(owner) {}
    std::function<QJsonArray()> entries;
    std::function<QString()> selected;
    std::function<void(QString, QString)> invoke;
    std::function<void()> nativeWorkspace;
    void wire()
    {
        if (!entries || !selected || !invoke || !nativeWorkspace) return;
        for (auto widget : QApplication::allWidgets()) if (auto root = qobject_cast<QMenu *>(widget))
            if (root->objectName() == QLatin1String("workspace")) refresh(root);
    }
private:
    QAction *action(QMenu *menu, const QString &name, const QString &label, const QString &operation, const QString &id = {})
    {
        QAction *result = nullptr;
        for (auto item : menu->actions()) if (item->objectName() == name) { result = item; break; }
        if (!result) { result = menu->addAction(label); result->setObjectName(name); }
        result->setText(label); result->setProperty("unboundArrangementId", id);
        if (!wiredActions.contains(result)) {
            wiredActions.insert(result);
            connect(result, &QObject::destroyed, this, [this, result] { wiredActions.remove(result); });
            connect(result, &QAction::triggered, this, [this, result, operation] {
                const auto id = result->property("unboundArrangementId").toString();
                QTimer::singleShot(0, this, [this, operation, id] {
                    logEvent(QStringLiteral("workspace.menu.command"), {{"operation", operation}, {"id", id}});
                    if (operation == "Native") nativeWorkspace(); else invoke(operation, id);
                    wire();
                });
            });
        }
        return result;
    }
    void watch(QMenu *menu, QMenu *root)
    {
        if (wiredMenus.contains(menu)) return;
        wiredMenus.insert(menu);
        connect(menu, &QObject::destroyed, this, [this, menu] { wiredMenus.remove(menu); });
        connect(menu, &QMenu::aboutToShow, this, [this, root] { refresh(root); });
    }
    void refresh(QMenu *root)
    {
        action(root, "unboundRestoreNativeWorkspace", "Restore Native Workspace (Dock All)", "Native");
        auto menu = root->findChild<QMenu *>("unboundWorkspaceArrangements", Qt::FindDirectChildrenOnly);
        if (!menu) {
            menu = new QMenu("Custom Workspace Presets", root); menu->setObjectName("unboundWorkspaceArrangements"); root->addMenu(menu);
        }
        menu->setTitle("Custom Workspace Presets");
        menu->setProperty("unboundOwnMenu", true);
        watch(root, root); watch(menu, root);
        action(menu, "unboundSaveWorkspaceAs", QString::fromUtf8("Save Current Workspace As…"), QString::fromUtf8("Save As…"));
        const auto saved = entries(); const auto current = selected(); bool currentExists = false; QSet<QString> ids;
        for (const auto &value : saved) if (value.toObject()["id"].toString() == current) currentExists = true;
        action(menu, "unboundUpdateWorkspace", "Update Current Saved Workspace", "Update", current)->setEnabled(currentExists);
        action(menu, "unboundReorderWorkspaces", QString::fromUtf8("Reorder Presets…"), "Reorder");
        QList<QAction *> ordered;
        for (const auto &value : saved) {
            const auto entry = value.toObject(); const auto id = entry["id"].toString(); ids.insert(id);
            auto preset = menu->findChild<QMenu *>("unboundWorkspace_" + id, Qt::FindDirectChildrenOnly);
            if (!preset) {
                preset = new QMenu(menu); preset->setObjectName("unboundWorkspace_" + id);
                preset->setProperty("unboundArrangementId", id); menu->addMenu(preset);
            }
            preset->setProperty("unboundOwnMenu", true);
            auto title = entry["name"].toString(); title.replace('&', "&&"); preset->setTitle(title);
            preset->menuAction()->setCheckable(true); preset->menuAction()->setChecked(id == current);
            ordered.append(preset->menuAction());
            watch(preset, root);
            action(preset, "unboundRestoreArrangement", "Restore Workspace", "Restore", id);
            action(preset, "unboundUpdateArrangement", "Update from Current Layout", "Update", id);
            for (const auto &operation : {QString::fromUtf8("Rename…"), QString::fromUtf8("Duplicate…"), QStringLiteral("Delete")})
                action(preset, "unboundArrangement_" + operation, operation, operation, id);
        }
        for (auto preset : menu->findChildren<QMenu *>(QString(), Qt::FindDirectChildrenOnly))
            if (!ids.contains(preset->property("unboundArrangementId").toString())) {
                menu->removeAction(preset->menuAction()); preset->deleteLater();
            }
        QList<QAction *> currentOrder;
        for (auto item : menu->actions()) if (item->menu()) currentOrder.append(item);
        if (currentOrder != ordered) {
            for (auto item : currentOrder) menu->removeAction(item);
            for (auto item : ordered) menu->addAction(item);
        }
    }
    QSet<QAction *> wiredActions;
    QSet<QMenu *> wiredMenus;
};
}
