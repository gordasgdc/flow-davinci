// SPDX-License-Identifier: MIT
#import <AppKit/AppKit.h>
#include <QWidget>
#include "nativechrome.h"
namespace Unbound {
void applyNativeChrome(QWidget *widget, bool compact)
{
    NSView *view = reinterpret_cast<NSView *>(widget->winId());
    NSWindow *window = view.window;
    if (!window) return;
    window.titleVisibility = compact ? NSWindowTitleHidden : NSWindowTitleVisible;
    window.titlebarAppearsTransparent = compact;
    if (compact) window.styleMask |= NSWindowStyleMaskFullSizeContentView;
    else window.styleMask &= ~NSWindowStyleMaskFullSizeContentView;
}
int nativeChromeFlags(QWidget *widget)
{
    NSView *view = reinterpret_cast<NSView *>(widget->winId());
    NSWindow *window = view.window;
    if (!window) return 0;
    return ((window.styleMask & NSWindowStyleMaskTitled) ? 1 : 0) |
        ((window.styleMask & NSWindowStyleMaskResizable) ? 2 : 0) |
        ((window.styleMask & NSWindowStyleMaskFullSizeContentView) ? 4 : 0);
}
void orderNativeWindows(const QList<QWidget *> &bottomToTop)
{
    NSWindow *previous = nil;
    for (QWidget *widget : bottomToTop) {
        if (!widget || !widget->isVisible() || widget->isMinimized()) continue;
        NSView *view = reinterpret_cast<NSView *>(widget->winId());
        NSWindow *window = view.window;
        if (!window) continue;
        [window orderWindow:NSWindowAbove relativeTo:(previous && previous.level == window.level ? previous.windowNumber : 0)];
        previous = window;
    }
}
qint64 nativeKeyWindowNumber() { return NSApp.keyWindow ? NSApp.keyWindow.windowNumber : 0; }
qint64 nativeWindowNumber(QWidget *widget)
{
    NSView *view = reinterpret_cast<NSView *>(widget->winId());
    return view.window ? view.window.windowNumber : 0;
}
QList<qint64> nativeOrderedWindowNumbers()
{
    QList<qint64> result;
    for (NSWindow *window in NSApp.orderedWindows) result.append(window.windowNumber);
    return result;
}
}
