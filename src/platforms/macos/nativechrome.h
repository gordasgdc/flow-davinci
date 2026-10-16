// SPDX-License-Identifier: MIT
#pragma once
class QWidget;
namespace Unbound {
void applyNativeChrome(QWidget *widget, bool compact);
int nativeChromeFlags(QWidget *widget);
}
