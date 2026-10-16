// SPDX-License-Identifier: MIT
#pragma once
#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include "nativechrome.h"

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
    MacFloatingHost() : QWidget(nullptr, Qt::Window)
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
        const int top = value == WindowChrome::Compact ? 28 : value == WindowChrome::Hidden ? 12 : 0;
        setContentsMargins(0, top, 0, 0);
        if (visible) show();
        applyNativeChrome(this, value == WindowChrome::Compact);
        if (position.isValid()) setGeometry(position);
        update();
    }
protected:
    void paintEvent(QPaintEvent *event) override
    {
        QWidget::paintEvent(event);
        QPainter painter(this);
        painter.setPen(palette().color(QPalette::WindowText));
        if (chrome == WindowChrome::Compact)
            painter.drawText(QRect(84, 0, qMax(0, width() - 92), 28), Qt::AlignVCenter | Qt::AlignLeft, windowTitle());
        if (chrome == WindowChrome::Hidden)
            painter.drawText(QRect(0, 0, width(), 12), Qt::AlignCenter, QStringLiteral("···"));
    }
    bool eventFilter(QObject *object, QEvent *event) override
    {
        if (chrome == WindowChrome::Standard) return false;
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
            else move(startGeometry.topLeft() + delta);
            return true;
        }
        if (dragging && event->type() == QEvent::MouseButtonRelease) { dragging = false; releaseMouse(); return true; }
        return false;
    }
private:
    WindowChrome chrome = WindowChrome::Standard;
    bool dragging = false;
    Qt::Edges dragEdges;
    QPoint start;
    QRect startGeometry;
};
}
