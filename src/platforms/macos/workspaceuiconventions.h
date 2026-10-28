// SPDX-License-Identifier: MIT
#pragma once
#include <QString>
#include "../../core/flowlocale.h"
class QWidget;
namespace Unbound {
// Accepted Mac presentation values, preserved exactly.
inline int nativeWorkspaceTitleGap(QWidget *) { return 12; }
inline QString nativeWorkspaceSnapLabel() { return FD("Snap window edges (Option bypasses)"); }
inline QString nativeWorkspacePanelAlias(const QString &) { return {}; }
}
