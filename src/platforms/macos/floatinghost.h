// SPDX-License-Identifier: MIT
#pragma once
#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include "nativechrome.h"
#include "windowsnap.h"
#include "diagnostics.h"
#include <QScreen>

namespace Unbound {
enum class WindowChrome { Standard, Compact, Hidden };

inline QRect resizeFromEdge(const QRect &original, QPoint delta, Qt::Edges edges, QSize minimum, QSize maximum)
{
    QRect result = original;
    int width = original.width(), height = original.height();
    if (edges & Qt::LeftEdge) width -= delta.x();
    if (edges & Qt::RightEdge) width += delta.x();
    if (edges & Qt::TopEdge) height -= delta.y();
    if (edges & Qt::BottomEdge) height += delta.y();
    width = qBound(minimum.width(), width, maximum.width());
    height = qBound(minimum.height(), height, maximum.height());
    result.setSize(QSize(width, height));
    if (edges & Qt::LeftEdge) result.moveRight(original.right());
    if (edges & Qt::TopEdge) result.moveBottom(original.bottom());
    return result;
}

class MacFloatingHost : public QWidget
{
public:
    explicit MacFloatingHost(QWidget *owner = nullptr) : QWidget(owner, owner ? Qt::Tool : Qt::Window)
    {
        qApp->installEventFilter(this);
        setMouseTracking(true);
        setAutoFillBackground(true);
    }
    ~MacFloatingHost() override { if (qApp) qApp->removeEventFilter(this); }
    void setChrome(WindowChrome value)
    {
        const bool visible = isVisible();
        const QRect position = geometry();
        chrome = value;
        setWindowFlag(Qt::FramelessWindowHint, value == WindowChrome::Hidden);
        const int top = value == WindowChrome::Compact ? 28 : 12;
        setContentsMargins(0, top, 0, 0);
        if (visible) show();
        applyNativeChrome(this, value == WindowChrome::Compact);
        if (position.isValid()) setGeometry(position);
        update();
    }
    void setSnapping(bool enabled) { snapping = enabled; if (!enabled && preview) preview->hide(); }
protected:
    void hideEvent(QHideEvent *event) override
    {
        if (preview) preview->hide();
        snapPosition = {};
        if (dragging) { dragging = false; releaseMouse(); }
        QWidget::hideEvent(event);
    }
    void paintEvent(QPaintEvent *event) override
    {
        QWidget::paintEvent(event);
        QPainter painter(this);
        painter.setPen(palette().color(QPalette::WindowText));
        if (chrome == WindowChrome::Compact)
            painter.drawText(QRect(84, 0, qMax(0, width() - 92), 28), Qt::AlignVCenter | Qt::AlignLeft, windowTitle());
        if (chrome != WindowChrome::Compact)
            painter.drawText(QRect(0, 0, width(), 12), Qt::AlignCenter, QStringLiteral("···"));
    }
    bool eventFilter(QObject *object, QEvent *event) override
    {
        auto widget = qobject_cast<QWidget *>(object);
        if (!widget || widget->window() != this) return false;
        if (event->type() != QEvent::MouseButtonPress && event->type() != QEvent::MouseButtonRelease && event->type() != QEvent::MouseMove)
            return false;
        auto mouse = static_cast<QMouseEvent *>(event);
        const QPoint point = mapFromGlobal(mouse->globalPos());
        Qt::Edges edges;
        if (chrome == WindowChrome::Hidden) {
            if (point.x() < 7) edges |= Qt::LeftEdge;
            if (point.x() >= width() - 7) edges |= Qt::RightEdge;
            if (point.y() < 4) edges |= Qt::TopEdge;
            if (point.y() >= height() - 7) edges |= Qt::BottomEdge;
        }
        const bool grip = widget == this && point.y() >= 4 && point.y() < contentsMargins().top();
        if (event->type() == QEvent::MouseButtonPress && mouse->button() == Qt::LeftButton && (edges || grip)) {
            dragging = true; dragEdges = edges; start = mouse->globalPos(); startGeometry = geometry(); grabMouse(); return true;
        }
        if (dragging && event->type() == QEvent::MouseMove) {
            const QPoint delta = mouse->globalPos() - start;
            if (dragEdges) setGeometry(resizeFromEdge(startGeometry, delta, dragEdges,
                minimumSize().expandedTo(minimumSizeHint()).expandedTo(QSize(80, 40)), maximumSize()));
            else {
                const QRect desired(startGeometry.topLeft() + delta, size());
                move(desired.topLeft());
                previewSnap(desired, mouse->modifiers().testFlag(Qt::AltModifier));
            }
            return true;
        }
        if (dragging && event->type() == QEvent::MouseButtonRelease) {
            if (!dragEdges && snapping && !mouse->modifiers().testFlag(Qt::AltModifier) && snapPosition.isValid()) {
                move(snapPosition.topLeft());
                logEvent(QStringLiteral("window.snap"), {{"window", objectName()}, {"x", x()}, {"y", y()}});
            }
            if (preview) preview->hide();
            snapPosition = {}; dragging = false; releaseMouse(); return true;
        }
        return false;
    }
private:
    void previewSnap(const QRect &desired, bool bypass)
    {
        snapPosition = {};
        if (!snapping || bypass) { if (preview) preview->hide(); return; }
        const QRect currentFrame = frameGeometry();
        const QPoint offset = currentFrame.topLeft() - geometry().topLeft();
        const QSize decoration = currentFrame.size() - size();
        const QRect proposed(desired.topLeft() + offset, desired.size() + decoration);
        QList<QRect> targets, displays;
        for (auto window : QApplication::topLevelWidgets())
            if (window != this && window->isVisible() && !window->isMinimized() &&
                (window->objectName().startsWith("unboundFloating_") || window->objectName() == "UiMainWindow"))
                targets.append(window->frameGeometry());
        for (auto screen : QGuiApplication::screens()) displays.append(screen->availableGeometry());
        const QRect snapped = snapWindowFrame(proposed, targets, displays);
        if (snapped == proposed) { if (preview) preview->hide(); return; }
        snapPosition = QRect(snapped.topLeft() - offset, desired.size());
        if (!preview) {
            preview = new QWidget(this, Qt::Tool | Qt::FramelessWindowHint | Qt::WindowTransparentForInput | Qt::WindowDoesNotAcceptFocus);
            preview->setObjectName(QStringLiteral("unboundSnapPreview"));
            preview->setAttribute(Qt::WA_ShowWithoutActivating);
            preview->setAttribute(Qt::WA_TransparentForMouseEvents);
            preview->setStyleSheet(QStringLiteral("background: rgba(80,140,210,45); border: 2px solid #5099df;"));
        }
        preview->setGeometry(snapped); preview->show(); preview->raise();
    }
    WindowChrome chrome = WindowChrome::Standard;
    bool snapping = true;
    QWidget *preview = nullptr;
    QRect snapPosition;
    bool dragging = false;
    Qt::Edges dragEdges;
    QPoint start;
    QRect startGeometry;
};
}
