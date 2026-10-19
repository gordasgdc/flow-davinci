// SPDX-License-Identifier: MIT
#pragma once
#include <QAbstractButton>
#include "diagnostics.h"
namespace Unbound {
inline bool setNativePanelVisibility(QWidget *main,const QString &toggle,bool visible) {
    if (!main || toggle.isEmpty()) return false;
    const auto base="btnEditToggle"+toggle;
    for (auto button:main->findChildren<QAbstractButton *>()) {
        const auto name=button->objectName();
        if (name!=base+"InSSM" && name!=base+"InDSM" && name!=base+"InEFTM") continue;
        if (!button->isEnabled() || !button->isCheckable()) continue;
        if (button->isChecked()!=visible) button->click();
        logEvent("panel.visibility.native-control",{{"toggle",toggle},{"button",name},{"requested",visible},{"checked",button->isChecked()}});
        return button->isChecked()==visible;
    }
    return false;
}
}
