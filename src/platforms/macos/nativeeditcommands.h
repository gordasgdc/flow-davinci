// SPDX-License-Identifier: MIT
#pragma once
#include <QAbstractButton>
#include <QActionGroup>
#include <QPointer>
#include <QMenu>
#include <QSet>
#include "panelcommands.h"
#include "nativeeditmenutarget.h"
#include "editcontrolinspection.h"
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
    // Edit-page presentation command: no clip-selection or focused-panel requirement.
    bool fixedPlayhead() {
        QSet<QAction *> matches;
        // This Edit action identity is present in Resolve's own keyboard preset.
        // Prefer its native View-menu command over custom-painted options rows.
        if (ready()) for (auto widget:QApplication::allWidgets()) {
            auto actions=widget->actions();actions.append(widget->findChildren<QAction *>(QString(),Qt::FindDirectChildrenOnly));
            for(auto action:actions)if(action->objectName()=="viewTimelineScrollingFixed")matches.insert(action);
        }
        if(matches.isEmpty() && ready()) for(auto widget:QApplication::allWidgets())if(auto menu=qobject_cast<QMenu *>(widget)) {
            if(!nativeTimelineOptionsMenu(menu))continue;
            for(auto action:menu->actions())if(nativeEditActionLabel(action,"Fixed Playhead"))matches.insert(action);
        }
        if (matches.size()!=1 || !(*matches.begin())->isEnabled()) return unavailable("fixed-playhead",matches.size(),"Edit's native Fixed Playhead action is unavailable or ambiguous; no scrolling state was changed.");
        auto action=*matches.begin();
        logEvent("command.edit-target.invoke",{{"command","fixed-playhead"},{"object",action->objectName()},
            {"checkable",action->isCheckable()},{"checked_before",action->isChecked()}});
        // Resolve may expose this as an exclusive scrolling-style choice,
        // rather than a simple checkable toggle. Preserve the previously selected
        // sibling when entering Fixed; return to it on the second invocation.
        QAction *target=action;
        if(auto group=action->actionGroup())if(group->isExclusive()) {
            if(!action->isChecked()) {
                auto checked=group->checkedAction();
                previousScrolling=checked!=action ? checked:nullptr;
            } else {
                target=previousScrolling;
                if(!target || target->actionGroup()!=group || !target->isEnabled()) {
                    target=nullptr;
                    for(auto sibling:group->actions())if(sibling->objectName()=="viewTimelineScrollingDisabled" && sibling->isEnabled())target=sibling;
                }
                if(!target)return unavailable("fixed-playhead",1,"No native return scrolling style is available; scrolling state was retained.");
            }
        }
        QPointer<QAction> retained(action);const auto targetName=target->objectName();
        target->trigger();
        logEvent("command.edit-target.result",{{"command","fixed-playhead"},{"success",true},
            {"checked_after",retained ? retained->isChecked():false},{"action_retained",bool(retained)},{"native_target",targetName}});
        return true;
    }
    bool stabilize() {
        QSet<QAbstractButton *> matches;
        if (ready()) for (auto top:QApplication::topLevelWidgets()) for (auto button:top->findChildren<QAbstractButton *>()) {
            if (!button->isVisible() || (label(button->text())!="Stabilize" && button->objectName()!="btnStabilize")) continue;
            bool inspector=false;
            for (auto parent=button->parentWidget();parent;parent=parent->parentWidget()) inspector |= parent->objectName()=="widgetEditInspector";
            if (inspector) matches.insert(button);
        }
        if (matches.size()!=1 || !(*matches.begin())->isEnabled()) {
            inspectStabilizationControls();
            return unavailable("stabilize",matches.size(),"Show the Inspector Stabilization section for a video clip; its native Stabilize button must be enabled.");
        }
        auto button=*matches.begin();
        logEvent("command.edit-target.invoke",{{"command","stabilize"},{"object",button->objectName()},{"method","native-button-click"}});
        button->click();
        logEvent("command.edit-target.result",{{"command","stabilize"},{"success",true},{"completion","native-button-invoked"}});
        return true;
    }
private:
    QPointer<QAction> previousScrolling;
    std::function<QWidget *()> editPage;
    bool ready() const { auto page=editPage ? editPage() : nullptr; return page && page->isVisible(); }
    static QString label(QString text) { text=text.section('\t',0,0); text.remove('&'); return text.trimmed(); }
    bool unavailable(const QString &command,int matches,const QString &message) {
        logEvent("command.edit-target.unavailable",{{"command",command},{"matches",matches},{"reason",message}});
        logEvent("command.edit-target.result",{{"command",command},{"success",false},{"reason",message}});
        if (report) report(message); return false;
    }
};
}
