// SPDX-License-Identifier: MIT
#pragma once
#include <QWidget>
namespace Unbound {
// Windows Qt page coordinates already exclude the OS caption. The existing
// native page layout reserves any custom header; there is no Cocoa content
// overlap inset to apply. Preserve its own margins in NativeRootLayout.
inline int nativeWorkspaceTopInset(QWidget *) { return 0; }
}
