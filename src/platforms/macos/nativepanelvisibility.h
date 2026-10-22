// SPDX-License-Identifier: MIT
#pragma once
#include <QAbstractButton>
#include <QSignalBlocker>
#include <QStackedWidget>
#include "diagnostics.h"
namespace Unbound {
inline bool currentNativePanelControl(QAbstractButton *button,QWidget *main) {
    if(button->isVisible())return true;
    if(button->isHidden())return false;
    // Hiding the Workspace Toolbar must not disable native reveal actions.
    // Its current stacked page remains authoritative even while the whole band
    // is hidden. A non-current SSM/DSM/EFTM branch is never a fallback.
    bool currentStack=false;
    for(auto child=static_cast<QWidget *>(button);child && child!=main;child=child->parentWidget()) {
        if(auto stack=qobject_cast<QStackedWidget *>(child->parentWidget())) {
            if(stack->currentWidget()!=child)return false;
            currentStack=true;
        }
    }
    return currentStack;
}
inline bool setNativePanelVisibility(QWidget *main,const QString &toggle,bool visible,QWidget *target=nullptr) {
    if (!main || toggle.isEmpty()) return false;
    const auto base="btnEditToggle"+toggle;
    for (auto button:main->findChildren<QAbstractButton *>()) {
        const auto name=button->objectName();
        if (name!=base+"InSSM" && name!=base+"InDSM" && name!=base+"InEFTM") continue;
        // Inactive SSM/DSM/EFTM variants retain checked states.
        if (!currentNativePanelControl(button,main) || !button->isEnabled() || !button->isCheckable()) continue;
        const bool stale=target && target->isVisible()!=visible && button->isChecked()==visible;
        if(stale) {
            // OpenGate mirrors caller state with blocked signals while content
            // is leased. On native reacquisition, that mirror is not evidence
            // that Resolve has revealed the original owner. Dispatch exactly
            // one normal native activation, with the requested checked value.
            const QSignalBlocker block(button);button->setChecked(!visible);
        }
        if (button->isChecked()!=visible) button->click();
        logEvent("panel.visibility.native-control",{{"toggle",toggle},{"button",name},{"requested",visible},{"checked",button->isChecked()},{"reconciled_stale_check",stale},{"target_visible",target && target->isVisible()},{"control_visible",button->isVisible()}});
        return button->isChecked()==visible;
    }
    return false;
}
}
