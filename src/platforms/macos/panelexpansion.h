// SPDX-License-Identifier: MIT
#pragma once
#include <QPointer>
#include <QWidget>
#include "nativechrome.h"
namespace Unbound {
class PanelExpansion
{
public:
    ~PanelExpansion() { restore(); }
    void foreground(QWidget *host)
    {
        restore(); current = host;
        if (!current) return;
        current->setProperty("unboundExpanded", true);
        setNativeExpansionPriority(current, true);
        current->raise(); current->activateWindow();
    }
    void restore()
    {
        if (current) {
            setNativeExpansionPriority(current, false);
            current->setProperty("unboundExpanded", false);
        }
        current = nullptr;
    }
private:
    QPointer<QWidget> current;
};
}
