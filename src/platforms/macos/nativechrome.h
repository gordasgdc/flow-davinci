// SPDX-License-Identifier: MIT
#pragma once
#include <QList>
class QWidget;
namespace Unbound {
void applyNativeChrome(QWidget *widget, bool compact);
int nativeChromeFlags(QWidget *widget);
void orderNativeWindows(const QList<QWidget *> &bottomToTop);
qint64 nativeKeyWindowNumber();
qint64 nativeWindowNumber(QWidget *widget);
QList<qint64> nativeOrderedWindowNumbers();
}
