// SPDX-License-Identifier: MIT
#pragma once
#include "nativearrangement.h"
#include "panelgroup.h"
namespace Unbound {
// Bind session commands and saved workspace state to the page coordinator. UI
// controls and native menus call this same route; it owns no window chrome.
class NativeWorkspaceBinding : public QObject {
public:
    explicit NativeWorkspaceBinding(QObject *owner) : QObject(owner),arrangement(this) {}
    std::function<void()> changed;
    std::function<void()> beforeMount;
    std::function<void(const QString &)> failed;
    void registerPanel(const QString &id,MacPanelSession *session) { registry[id]=session; }
    bool contains(const QString &id) const { return arrangement.contains(id); }
    bool selected(const QString &id) const { return arrangement.selected(id); }
    bool active() const { return arrangement.active(); }
    bool ownsTarget(QWidget *target) const {
        auto surface=arrangement.surface();
        return surface && target && surface->isAncestorOf(target) && target->property("groupPanel").isValid();
    }
    QString validate(const QJsonObject &saved,const QJsonArray &floatingGroups={}) const {
        if (saved.isEmpty()) return {};
        QSet<QString> names; for (auto it=registry.begin();it!=registry.end();++it) names.insert(it.key());
        NativeArrangementPlan plan; const auto error=NativeArrangementPlan::parse(saved,"edit",names,plan);
        if (!error.isEmpty()) return error;
        const auto states=saved["panels"].toObject();
        for (const auto &group : floatingGroups) {
            QStringList members; groupTreeNames(PanelGroup::savedTree(group.toObject()),members);
            for (const auto &id : members) if (states.contains(id)) return QStringLiteral("A saved panel belongs to both native and floating groups.");
        }
        return {};
    }
    QString setPending(const QJsonObject &saved,const QJsonArray &floatingGroups={}) {
        const auto error=validate(saved,floatingGroups);
        if (error.isEmpty()) pending=saved; else logEvent("workspace.native-plan.failed",{{"error",error}});
        return error;
    }
    QStringList requiredPanels() const { return pending["panels"].toObject().keys(); }
    void queueRestore(QWidget *page) {
        if (queued || pending.isEmpty() || active()) return;
        queued=true; QPointer<QWidget> safePage=page;
        QTimer::singleShot(0,this,[this,safePage] {
            queued=false; const auto error=restoreReady(safePage);
            if (!error.isEmpty() && failed) failed(error);
        });
    }
    QString restoreReady(QWidget *page) {
        if (busy || pending.isEmpty() || active() || !page || !page->isVisible()) return {};
        QHash<QString,MacPanelSession *> sessions;
        const auto states=pending["panels"].toObject();
        for (auto it=states.begin();it!=states.end();++it) {
            auto session=registry.value(it.key());
            if (!session || !session->active() || session->grouped()) return {};
            sessions[it.key()]=session;
        }
        QScopedValueRollback<bool> guard(busy,true);
        logEvent("workspace.native-plan.restore.request",{{"page","edit"},{"panels",sessions.size()}});
        if (beforeMount) beforeMount();
        const auto error=arrangement.initializeSaved(page,sessions,pending,"edit");
        if (error.isEmpty()) { pending={}; if (changed) changed(); }
        return error;
    }
    QJsonObject saved() const { return active() ? arrangement.savedPlan("edit") : pending; }
    QString restorePresentation(const QJsonObject &saved) {
        if (busy) return QStringLiteral("The native workspace is changing.");
        QScopedValueRollback<bool> guard(busy,true); const auto error=arrangement.restorePresentation(saved);
        if (error.isEmpty() && changed) changed(); return error;
    }
    QString dock(QWidget *page,const QHash<QString,MacPanelSession *> &sessions,const QJsonObject &tree,
        const QString &target,Qt::Orientation direction,bool before) {
        if (busy) return QStringLiteral("The native workspace is changing.");
        QScopedValueRollback<bool> guard(busy,true);
        if (!active() && beforeMount) beforeMount();
        const auto error=active() ? arrangement.add(sessions,tree,target,direction,before) : arrangement.initialize(page,sessions,
            splitGroupTree({{"panel",nativeRemainderId()}},tree,direction,before));
        if (error.isEmpty()) { pending={}; if (changed) changed(); }
        return error;
    }
    QString dockSelection(QWidget *page,const QString &id,PanelGroup *group,const QString &target,
        Qt::Orientation direction,bool before) {
        if (busy || !page || !page->isVisible()) return QStringLiteral("Open the native page before docking here.");
        auto selected=registry.value(id);
        if (!selected || !selected->active() || contains(id)) return QStringLiteral("Choose a floating panel or group.");
        QHash<QString,MacPanelSession *> panels;
        const auto previous=group ? group->snapshot() : QJsonObject();
        const auto tree=group ? PanelGroup::savedTree(previous) : QJsonObject{{"panel",id}};
        for (auto it=registry.begin();it!=registry.end();++it)
            if (it.value() && (group ? group->contains(it.value()) : it.key()==id)) panels[it.key()]=it.value();
        logEvent("workspace.native-transfer.request",{{"panel",id},{"members",panels.size()},{"target",target},{"orientation",int(direction)}});
        if (group) group->dissolve(false,true);
        const auto error=dock(page,panels,tree,target,direction,before);
        if (!error.isEmpty() && group) {
            QList<MacPanelSession *> members; for (auto session : panels) members.append(session);
            const auto rollback=group->initialize(members,tree,WorkspaceSettings::rectangle(previous));
            if (rollback.isEmpty()) group->restore(previous);
            else return error+QStringLiteral(" Original group could not be rebuilt: ")+rollback;
        }
        return error;
    }
    bool setVisible(const QString &id,bool visible) {
        if (busy) return false; QScopedValueRollback<bool> guard(busy,true);
        const bool result=arrangement.setPanelVisible(id,visible); if (result && changed) changed(); return result;
    }
    bool detach(const QString &id) {
        if (busy) return false; QScopedValueRollback<bool> guard(busy,true);
        const bool result=arrangement.detach(id);
        if (result) if (auto session=registry.value(id)) if (session->floating()) session->setHiddenByUser(false);
        if (result && changed) changed(); return result;
    }
    bool release(bool dockPanels = false) {
        if (busy) return false; QScopedValueRollback<bool> guard(busy,true);
        pending={}; const bool result=arrangement.restore(dockPanels);
        if (!result) logEvent("workspace.native-recovery.failed");
        if (result && changed) changed(); return result;
    }
private:
    NativeArrangement arrangement;
    QHash<QString,QPointer<MacPanelSession>> registry;
    QJsonObject pending;
    bool busy=false,queued=false;
};
}
