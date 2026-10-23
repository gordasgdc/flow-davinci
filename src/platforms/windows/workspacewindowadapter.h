// SPDX-License-Identifier: MIT
#pragma once
#include <QWidget>
#include "../../platform/nativewindowcontract.h"
#include "panefloatingwindow.h"
#include "panetaskbar.h"
namespace Unbound {
class WorkspaceHostWindow : public PaneFloatingWindow {
public:
    WorkspaceHostWindow(QWidget *owner,Qt::WindowFlags flags) {
        flags=(flags & ~Qt::WindowType_Mask)|Qt::Window|Qt::CustomizeWindowHint|Qt::WindowSystemMenuHint;
        setParent(owner,flags);setNativeTitleBarEnabled(true);
        if(owner)inheritPaneTaskbarIdentity(this,owner);
    }
};
// Windows Qt page coordinates already exclude the OS caption. The existing
// native page layout reserves any custom header; there is no Cocoa content
// overlap inset to apply. Preserve its own margins in NativeRootLayout.
inline int nativeWorkspaceTopInset(QWidget *) { return 0; }
}
