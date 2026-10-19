// SPDX-License-Identifier: MIT
#pragma once
#include "panelexpansion.h"
#include "panelsession.h"
#include "panelgroup.h"
#include "surfacereadiness.h"
#include "workspacesettings.h"
#include <QWindow>

namespace Unbound {
// Owns one reversible expansion transaction, including readiness cancellation
// and original presentation. Manager supplies transitions, not internal state.
class PanelExpansionController : public QObject {
public:
    explicit PanelExpansionController(QObject *owner) : QObject(owner) {}
    struct Target {
        int index=-1;
        QString name;
        MacPanelSession *session=nullptr;
        PanelGroup *group=nullptr;
        QJsonObject saved,native;
        std::function<bool()> pending;
        std::function<void()> undock,cancelPending,restored;
        std::function<void(QJsonObject)> restoreSaved;
        std::function<QString(QJsonObject)> restoreNative;
    };
    std::function<bool()> running;
    std::function<void(int)> update;
    std::function<void()> save;
    std::function<void(QString)> report;
    int index() const { return current; }
    void sessionInactive(int index) { if (current==index) current=-1; }
    QJsonObject panelSnapshot(int index,const QJsonObject &value) const { return current==index ? target.saved : value; }
    QJsonObject groupSnapshot(PanelGroup *group) const
    { return current>=0 && group==originalGroup ? originalGroupState : group->snapshot(); }
    QJsonObject nativeSnapshot(const QJsonObject &value) const
    { return current>=0 && !target.native.isEmpty() ? target.native : value; }
    void begin(Target value,QWidget *page)
    {
        target=std::move(value);
        originalOffset=target.session->overflowOffset();
        wasFloating=target.session->active(); wasGrouped=target.session->grouped();
        originalGroup=target.group;
        originalGroupState=originalGroup ? originalGroup->snapshot() : QJsonObject();
        if (wasGrouped && !target.session->beginGroupedExpansion()) return;
        current=target.index; const unsigned request=++version;
        if (!wasFloating) target.undock();
        new SurfaceReadiness(this,page,
            [this,request] { return current==target.index && request==version && running(); },
            [this] { return target.session->active() && !target.pending() && host(); },
            [this,request](bool ready) {
                if (current!=target.index || request!=version) return;
                auto window=host();
                if (!ready || !window) { current=-1; update(target.index); save(); return; }
                auto screen=window->screen(); if (!screen) screen=QApplication::primaryScreen();
                const auto margins=window->windowHandle() ? window->windowHandle()->frameMargins() : QMargins();
                window->setGeometry(screen->availableGeometry().adjusted(margins.left(),margins.top(),-margins.right(),-margins.bottom()));
                target.session->setHiddenByUser(false); priority.foreground(window); update(target.index);
                logEvent("panel.expand",{{"panel",target.name},{"originally_floating",wasFloating}});
            });
        update(target.index);
    }
    void restore()
    {
        if (current<0) return;
        const int index=current; priority.restore(); ++version; current=-1;
        target.cancelPending(); QString error;
        if (wasGrouped) {
            target.session->endGroupedExpansion();
            if (originalGroup && originalGroup->active()) originalGroup->restore(originalGroupState);
        }
        originalGroup=nullptr;
        if (target.session->active()) {
            target.session->setFloatingGeometry(WorkspaceSettings::rectangle(target.saved));
            target.session->setHiddenByUser(target.saved["hidden"].toBool());
            if (!wasFloating) error=target.session->dock();
        }
        target.restoreSaved(target.saved);
        if (!target.native.isEmpty()) {
            const auto nativeError=target.restoreNative(target.native);
            if (!nativeError.isEmpty()) error=nativeError;
            target.native={};
        }
        target.session->restoreOverflowOffset(originalOffset);
        target.restored();
        update(index); save();
        logEvent("panel.expand.restore",{{"panel",target.name},{"success",error.isEmpty()},{"error",error}});
        if (!error.isEmpty()) report(error);
    }
private:
    QWidget *host() const { return wasGrouped ? target.session->expandedWindow() : target.session->window(); }
    Target target;
    int current=-1;
    unsigned version=0;
    bool wasFloating=false,wasGrouped=false;
    QPoint originalOffset;
    QPointer<PanelGroup> originalGroup;
    QJsonObject originalGroupState;
    PanelExpansion priority;
};
}
