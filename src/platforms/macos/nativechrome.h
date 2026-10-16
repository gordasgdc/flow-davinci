// SPDX-License-Identifier: MIT
#pragma once
#include <QList>
#include <QPoint>
#include <functional>
class QWidget;
class QObject;
namespace Unbound {
void applyNativeChrome(QWidget *widget, bool compact);
int nativeChromeFlags(QWidget *widget);
void orderNativeWindows(const QList<QWidget *> &bottomToTop);
qint64 nativeKeyWindowNumber();
qint64 nativeWindowNumber(QWidget *widget);
QList<qint64> nativeOrderedWindowNumbers();
QObject *watchNativePointerRelease(QObject *owner, std::function<void(QPoint, bool)> released);
void normalizeNativePanel(QWidget *widget);
int nativeVisibilityFlags(QWidget *widget);
bool setNativeBackgroundHidden(QWidget *main, const QList<QWidget *> &panels, bool hidden);
#ifdef UNBOUND_NATIVE_TESTS
void postNativePointerReleaseForTest(QWidget *window);
#endif
}
