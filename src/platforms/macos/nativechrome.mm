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
}
