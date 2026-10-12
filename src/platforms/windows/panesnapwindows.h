// SPDX-License-Identifier: MIT
#ifndef UNBOUND_PANESNAPWINDOWS_H
#define UNBOUND_PANESNAPWINDOWS_H
#include "panesnap.h"
#include <QApplication>
#include <QWidget>
#include <QPointer>
#include <QMargins>
#ifdef Q_OS_WIN
#include <qt_windows.h>
#include <dwmapi.h>
namespace Unbound {
inline QRect paneContactBounds(HWND handle)
{
    RECT bounds{};
    if (FAILED(DwmGetWindowAttribute(handle, DWMWA_EXTENDED_FRAME_BOUNDS, &bounds, sizeof(bounds))))
        GetWindowRect(handle, &bounds);
    // DWM's decorative outline can leave a visible seam even when its bounding
    // rectangles touch. Join the surfaces inside that outline; do not include
    // invisible resize borders or shadows in the contact rectangle.
    UINT border = 0;
    DwmGetWindowAttribute(handle, DWMWA_VISIBLE_FRAME_BORDER_THICKNESS, &border, sizeof(border));
    const int inset = qMin(int(border), 4);
    return QRect(bounds.left + inset, bounds.top + inset,
        bounds.right - bounds.left - 2 * inset, bounds.bottom - bounds.top - 2 * inset);
}
class PaneSnapWindows
{
public:
    void begin(HWND handle)
    {
        m_handle = handle;
        m_windows.clear();
        m_workAreas.clear();
        // Use native coordinates, as for the window rectangles below. Qt screen
        // geometry can be logical coordinates on mixed-DPI desktops.
        EnumDisplayMonitors(nullptr, nullptr, collectWorkArea, reinterpret_cast<LPARAM>(&m_workAreas));
        for (auto widget : QApplication::topLevelWidgets()) {
            if (reinterpret_cast<HWND>(widget->effectiveWinId()) != handle
                && (widget->objectName() == QLatin1String("UiMainWindow")
                    || widget->objectName().startsWith(QLatin1String("gammarayFloating_"))))
                m_windows.append(widget);
        }
        RECT bounds{};
        GetWindowRect(handle, &bounds);
        const auto contact = paneContactBounds(handle);
        m_insets = QMargins(contact.x() - bounds.left, contact.y() - bounds.top,
            bounds.right - contact.x() - contact.width(), bounds.bottom - contact.y() - contact.height());
    }
    void apply(RECT &target, int hit, const MINMAXINFO &limits, bool bypass = false) const
    {
        if (!m_handle || bypass || (GetAsyncKeyState(VK_MENU) & 0x8000)) return;
        QVector<QRect> targets;
        for (const auto &widget : m_windows) {
            if (!widget || !widget->isVisible() || widget->isMinimized()) continue;
            const auto handle = reinterpret_cast<HWND>(widget->effectiveWinId());
            if (handle && IsWindowVisible(handle) && !IsIconic(handle)) targets.append(paneContactBounds(handle));
        }
        // Window and monitor edges compete by distance in the same calculation;
        // existing window adjacency wins ties. Refresh monitor areas each drag.
        targets += m_workAreas;
        const bool move = hit == HTCAPTION;
        const bool left = move || hit == HTLEFT || hit == HTTOPLEFT || hit == HTBOTTOMLEFT;
        const bool right = move || hit == HTRIGHT || hit == HTTOPRIGHT || hit == HTBOTTOMRIGHT;
        const bool top = move || hit == HTTOP || hit == HTTOPLEFT || hit == HTTOPRIGHT;
        const bool bottom = move || hit == HTBOTTOM || hit == HTBOTTOMLEFT || hit == HTBOTTOMRIGHT;
        const int horizontal = m_insets.left() + m_insets.right();
        const int vertical = m_insets.top() + m_insets.bottom();
        const QRect visible(target.left + m_insets.left(), target.top + m_insets.top(),
            target.right - target.left - horizontal, target.bottom - target.top - vertical);
        const QSize minimum(qMax(1, int(limits.ptMinTrackSize.x) - horizontal), qMax(1, int(limits.ptMinTrackSize.y) - vertical));
        const QSize maximum(limits.ptMaxTrackSize.x > 0 ? limits.ptMaxTrackSize.x - horizontal : 16777215,
            limits.ptMaxTrackSize.y > 0 ? limits.ptMaxTrackSize.y - vertical : 16777215);
        const int distance = qMax(1, MulDiv(12, GetDpiForWindow(m_handle), 96));
        const auto offset = paneSnapOffset(visible, targets, distance, left, right, top, bottom, minimum, maximum);
        if (left) target.left += offset.x();
        if (right) target.right += offset.x();
        if (top) target.top += offset.y();
        if (bottom) target.bottom += offset.y();
    }
private:
    static BOOL CALLBACK collectWorkArea(HMONITOR monitor, HDC, LPRECT, LPARAM data)
    {
        MONITORINFO info{};
        info.cbSize = sizeof(info);
        if (GetMonitorInfoW(monitor, &info)) {
            const auto &r = info.rcWork;
            reinterpret_cast<QVector<QRect> *>(data)->append(QRect(r.left, r.top, r.right-r.left, r.bottom-r.top));
        }
        return TRUE;
    }
    HWND m_handle = nullptr;
    QList<QPointer<QWidget>> m_windows;
    QVector<QRect> m_workAreas;
    QMargins m_insets;
};

}
#endif
#endif
