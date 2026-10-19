// SPDX-License-Identifier: MIT
#pragma once
#include <QAbstractButton>
#include <QMenu>
#include <QSet>
#include "panelcommands.h"
namespace Unbound {
// Dispatch the actual native action/signal; never hardcode an internal address,
// mouse coordinate, keyboard mapping or a second implementation of stabilization.
class NativeEditCommands : public QObject {
public:
    NativeEditCommands(QObject *owner,std::function<QWidget *()> page) : QObject(owner),editPage(std::move(page)) {}
    std::function<void(QString)> report;
    void addTo(PanelCommands *commands) {
        commands->add({"edit.timeline.fixed-playhead","Edit Timeline / Toggle Fixed Playhead",[this] { return ready(); },[this] { fixedPlayhead(); }});
        commands->add({"edit.inspector.stabilize","Edit Inspector / Stabilize",[this] { return ready(); },[this] { stabilize(); }});
    }
    bool fixedPlayhead() {
        QSet<QAction *> matches;
        if (ready()) for (auto widget:QApplication::allWidgets()) if (auto menu=qobject_cast<QMenu *>(widget)) {
            if (menu->property("unboundOwnMenu").toBool()) continue;
            bool timelineOptions=false;
            for (auto action:menu->actions()) timelineOptions |= label(action->text())=="Display Stacked Timelines";
            if (!timelineOptions) continue;
            for (auto action:menu->actions()) if (label(action->text())=="Fixed Playhead") matches.insert(action);
        }
        if (matches.size()!=1 || !(*matches.begin())->isEnabled()) return unavailable("fixed-playhead",matches.size(),"Open Timeline View Options once so Resolve creates its Fixed Playhead action.");
        auto action=*matches.begin();
        logEvent("command.edit-target.invoke",{{"command","fixed-playhead"},{"object",action->objectName()},
            {"checkable",action->isCheckable()},{"checked_before",action->isChecked()}});
        action->trigger(); return true;
    }
    bool stabilize() {
        QSet<QAbstractButton *> matches;
        if (ready()) for (auto top:QApplication::topLevelWidgets()) for (auto button:top->findChildren<QAbstractButton *>()) {
            if (!button->isVisible() || label(button->text())!="Stabilize") continue;
            bool inspector=false;
            for (auto parent=button->parentWidget();parent;parent=parent->parentWidget()) inspector |= parent->objectName()=="widgetEditInspector";
            if (inspector) matches.insert(button);
        }
        if (matches.size()!=1 || !(*matches.begin())->isEnabled()) return unavailable("stabilize",matches.size(),"Show the Inspector Stabilization section for a video clip; its native Stabilize button must be enabled.");
        auto button=*matches.begin();
        logEvent("command.edit-target.invoke",{{"command","stabilize"},{"object",button->objectName()},{"method","native-button-click"}});
        button->click(); return true;
    }
private:
    std::function<QWidget *()> editPage;
    bool ready() const { auto page=editPage ? editPage() : nullptr; return page && page->isVisible(); }
    static QString label(QString text) { text=text.section('\t',0,0); text.remove('&'); return text.trimmed(); }
    bool unavailable(const QString &command,int matches,const QString &message) {
        logEvent("command.edit-target.unavailable",{{"command",command},{"matches",matches},{"reason",message}});
        if (report) report(message); return false;
    }
};
}
