// SPDX-License-Identifier: MIT
#ifndef UNBOUND_PANEFLOATINGWINDOW_H
#define UNBOUND_PANEFLOATINGWINDOW_H
#include <QWidget>
#include <QLayout>
#include <QPointer>
#include <QPainter>
#include <QApplication>
#include "panesnapwindows.h"
#ifdef Q_OS_WIN
#include <qt_windows.h>
#include <dwmapi.h>
#endif
namespace Unbound {
class PaneFloatingWindow : public QWidget
{
public:
    PaneFloatingWindow() : QWidget(nullptr, Qt::Window)
    {
        // Auxiliary taskbar windows must not keep Resolve alive.
        setAttribute(Qt::WA_QuitOnClose, false);
    }
    void beginDeferredResize()
    {
        if (m_deferredLayout || !layout() || !layout()->isEnabled()) return;
        m_deferredLayout = layout();
        m_deferredLayout->setEnabled(false);
    }
    void finishDeferredResize()
    {
#ifdef Q_OS_WIN
        const bool wasDragging = m_dragHit != 0;
        m_dragHit = 0;
        if (wasDragging && GetCapture() == reinterpret_cast<HWND>(effectiveWinId())) ReleaseCapture();
#endif
        if (!m_deferredLayout) return;
        auto saved = m_deferredLayout.data();
        m_deferredLayout = nullptr;
        saved->setEnabled(true);
        saved->invalidate();
        saved->activate();
        update();
    }
#ifdef Q_OS_WIN
    POINT dragStartPosition() const { return m_dragStart; }
    bool interactiveDragActive() const { return m_dragHit != 0; }
    void updateInteractiveDrag(POINT cursor)
    {
        if (!m_dragHit) return;
        RECT target = m_dragRect;
        const LONG dx = cursor.x - m_dragStart.x, dy = cursor.y - m_dragStart.y;
        if (m_dragHit == HTCAPTION) OffsetRect(&target, dx, dy);
        else {
            const bool left = m_dragHit == HTLEFT || m_dragHit == HTTOPLEFT || m_dragHit == HTBOTTOMLEFT;
            const bool right = m_dragHit == HTRIGHT || m_dragHit == HTTOPRIGHT || m_dragHit == HTBOTTOMRIGHT;
            const bool top = m_dragHit == HTTOP || m_dragHit == HTTOPLEFT || m_dragHit == HTTOPRIGHT;
            const bool bottom = m_dragHit == HTBOTTOM || m_dragHit == HTBOTTOMLEFT || m_dragHit == HTBOTTOMRIGHT;
            if (left) target.left += dx;
            if (right) target.right += dx;
            if (top) target.top += dy;
            if (bottom) target.bottom += dy;
            const LONG width = qBound(m_dragLimits.ptMinTrackSize.x, target.right - target.left, m_dragLimits.ptMaxTrackSize.x);
            const LONG height = qBound(m_dragLimits.ptMinTrackSize.y, target.bottom - target.top, m_dragLimits.ptMaxTrackSize.y);
            if (left) target.left = target.right - width; else target.right = target.left + width;
            if (top) target.top = target.bottom - height; else target.bottom = target.top + height;
        }
        m_snap.apply(target, m_dragHit, m_dragLimits);
        SetWindowPos(reinterpret_cast<HWND>(winId()), nullptr, target.left, target.top,
            target.right - target.left, target.bottom - target.top, SWP_NOZORDER | SWP_NOACTIVATE);
    }
#endif
protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.fillRect(rect(), QColor(20, 20, 20));
    }
    bool nativeEvent(const QByteArray &type, void *message, long *result) override
    {
#ifdef Q_OS_WIN
        const auto msg = static_cast<MSG *>(message);
        const auto handle = msg->hwnd;
        if (msg->message == WM_ERASEBKGND) {
            RECT client;
            GetClientRect(handle, &client);
            HBRUSH background = CreateSolidBrush(RGB(20, 20, 20));
            FillRect(reinterpret_cast<HDC>(msg->wParam), &client, background);
            DeleteObject(background);
            *result = 1;
            return true;
        }
        if (msg->message == WM_NCLBUTTONDOWN
            && (msg->wParam == HTCAPTION || (msg->wParam >= HTLEFT && msg->wParam <= HTBOTTOMRIGHT))) {
            // Consume the press instead of allowing DefWindowProc to enter its
            // modal sizing loop. Every mouse update returns to Qt's event loop.
            if (isMaximized()) showNormal();
            SetForegroundWindow(handle);
            GetWindowRect(handle, &m_dragRect);
            m_snap.begin(handle);
            GetCursorPos(&m_dragStart);
            m_dragLimits = MINMAXINFO{};
            SendMessage(handle, WM_GETMINMAXINFO, 0, reinterpret_cast<LPARAM>(&m_dragLimits));
            if (m_dragLimits.ptMaxTrackSize.x <= 0) m_dragLimits.ptMaxTrackSize.x = 32767;
            if (m_dragLimits.ptMaxTrackSize.y <= 0) m_dragLimits.ptMaxTrackSize.y = 32767;
            m_dragHit = static_cast<int>(msg->wParam);
            if (m_dragHit != HTCAPTION) beginDeferredResize();
            SetCapture(handle);
            if (GetCapture() != handle) finishDeferredResize();
            *result = 0;
            return true;
        }
        if (m_dragHit && msg->message == WM_MOUSEMOVE) {
            POINT cursor;
            GetCursorPos(&cursor);
            updateInteractiveDrag(cursor);
            *result = 0;
            return true;
        }
        if (m_dragHit && (msg->message == WM_LBUTTONUP || msg->message == WM_CAPTURECHANGED
            || msg->message == WM_CANCELMODE || (msg->message == WM_KEYDOWN && msg->wParam == VK_ESCAPE))) {
            if (msg->message == WM_KEYDOWN || msg->message == WM_CANCELMODE)
                SetWindowPos(handle, nullptr, m_dragRect.left, m_dragRect.top,
                    m_dragRect.right - m_dragRect.left, m_dragRect.bottom - m_dragRect.top, SWP_NOZORDER | SWP_NOACTIVATE);
            finishDeferredResize();
            *result = 0;
            return true;
        }
        if (msg->message == WM_SHOWWINDOW && !msg->wParam) finishDeferredResize();
        // WM_SIZING distinguishes border resizing from title-bar dragging.
        if (msg->message == WM_SIZING) beginDeferredResize();
        if (msg->message == WM_EXITSIZEMOVE) finishDeferredResize();
#endif
        return QWidget::nativeEvent(type, message, result);
    }
private:
    QPointer<QLayout> m_deferredLayout;
#ifdef Q_OS_WIN
    int m_dragHit = 0;
    RECT m_dragRect{};
    POINT m_dragStart{};
    MINMAXINFO m_dragLimits{};
    PaneSnapWindows m_snap;
#endif
};
}
#endif
