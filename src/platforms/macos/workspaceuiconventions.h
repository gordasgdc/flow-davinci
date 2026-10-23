// SPDX-License-Identifier: MIT
#pragma once
#include <QString>
#include <QWidget>
namespace Unbound {
// Accepted Mac presentation values, preserved exactly.
inline int nativeWorkspaceTitleGap(QWidget *) { return 12; }
inline QString nativeWorkspaceSnapLabel() { return QStringLiteral("Snap window edges (Option bypasses)"); }
inline QString nativeWorkspacePanelAlias(const QString &) { return {}; }
// Keep Resolve's live Edit controller visible and above the real player widgets
// in the QObject tree. Only its presentation layout is leased. The name and
// direct-child topology are verified in the Mac Resolve 21.1.1 inventory.
inline QWidget *nativeWorkspaceLayoutOwner(QWidget *page) {
    if (page && page->objectName()==QLatin1String("WStackPage_Conform")) {
        auto controller=page->findChild<QWidget *>("m_pConformPanel",Qt::FindDirectChildrenOnly);
        if (controller && controller->layout()) return controller;
    }
    return page;
}
}
