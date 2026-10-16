// SPDX-License-Identifier: MIT
#import <AppKit/AppKit.h>
#include <QWidget>
#include <QPointer>
#include <QTimer>
#include <QVariant>
#include "nativechrome.h"
namespace Unbound {
class PointerReleaseWatch : public QObject
{
public:
    PointerReleaseWatch(QObject *owner, std::function<void(bool)> callback) : QObject(owner)
    {
        QPointer<PointerReleaseWatch> alive(this);
        monitor = [[NSEvent addLocalMonitorForEventsMatchingMask:NSEventMaskLeftMouseUp handler:^NSEvent *(NSEvent *event) {
            if (alive) {
                const bool option = event.modifierFlags & NSEventModifierFlagOption;
                QTimer::singleShot(0, alive, [alive, callback, option] { if (alive) callback(option); });
            }
            return event;
        }] retain];
    }
    ~PointerReleaseWatch() override { if (monitor) { [NSEvent removeMonitor:monitor]; [monitor release]; } }
private:
    id monitor = nil;
};
QObject *watchNativePointerRelease(QObject *owner, std::function<void(bool)> released)
{ return new PointerReleaseWatch(owner, std::move(released)); }
#ifdef UNBOUND_NATIVE_TESTS
void postNativePointerReleaseForTest(QWidget *widget)
{
    NSWindow *window = reinterpret_cast<NSView *>(widget->winId()).window;
    NSEvent *event = [NSEvent mouseEventWithType:NSEventTypeLeftMouseUp location:NSMakePoint(20, 20) modifierFlags:0
        timestamp:NSProcessInfo.processInfo.systemUptime windowNumber:window.windowNumber context:nil eventNumber:0 clickCount:1 pressure:0];
    [NSApp postEvent:event atStart:NO];
}
#endif
void applyNativeChrome(QWidget *widget, bool compact)
{
    NSView *view = reinterpret_cast<NSView *>(widget->winId());
    NSWindow *window = view.window;
    if (!window) return;
    // Qt tool ownership preserves the main window's shortcut scope; native level
    // stays normal so the explicit hierarchy and optional control pin decide Z.
    if (widget->windowType() == Qt::Tool) { window.level = NSNormalWindowLevel; window.hidesOnDeactivate = NO; }
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
    NSWindow *anchor = NSApp.keyWindow;
    for (auto widget : bottomToTop) if (widget && nativeWindowNumber(widget) == anchor.windowNumber) anchor = nil;
    NSWindow *previous = nil;
    for (QWidget *widget : bottomToTop) {
        if (!widget || !widget->isVisible() || widget->isMinimized()) continue;
        NSView *view = reinterpret_cast<NSView *>(widget->winId());
        NSWindow *window = view.window;
        if (!window) continue;
        if (!previous && anchor && anchor.isVisible && anchor.level == window.level)
            [window orderWindow:NSWindowBelow relativeTo:anchor.windowNumber];
        else [window orderWindow:NSWindowAbove relativeTo:(previous && previous.level == window.level ? previous.windowNumber : 0)];
        previous = window;
    }
}
void normalizeNativePanel(QWidget *widget)
{
    if (!widget || widget->windowType() != Qt::Tool) return;
    NSWindow *window = reinterpret_cast<NSView *>(widget->winId()).window;
    window.hidesOnDeactivate = NO; window.level = NSNormalWindowLevel;
}
int nativeVisibilityFlags(QWidget *widget)
{
    NSWindow *window = reinterpret_cast<NSView *>(widget->winId()).window;
    return (window.isVisible ? 1 : 0) | (window.hidesOnDeactivate ? 2 : 0) | (window.isMiniaturized ? 4 : 0);
}
bool setNativeBackgroundHidden(QWidget *main, const QList<QWidget *> &panels, bool hidden)
{
    if (!main) return false;
    NSWindow *background = reinterpret_cast<NSView *>(main->winId()).window;
    if (hidden) {
        [background orderOut:nil];
        for (auto panel : panels) if (panel && panel->isVisible() && !panel->isMinimized()) {
            NSWindow *window = reinterpret_cast<NSView *>(panel->winId()).window;
            [window orderFront:nil];
        }
        for (auto panel : panels) if (panel && panel->isVisible() && !panel->isMinimized() && !(nativeVisibilityFlags(panel) & 1)) {
            [background orderFront:nil]; return false;
        }
    } else [background orderFront:nil];
    main->setProperty("unboundBackgroundHidden", hidden); return true;
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
