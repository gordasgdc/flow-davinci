// SPDX-License-Identifier: MIT
#pragma once
#include <QApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QLabel>
#include <QPainter>
#include "nativechrome.h"
#include "windowsnap.h"
#include "diagnostics.h"
#include <QScreen>
#include <QPointer>
#include <QTimer>
#include <QCursor>
#include <functional>

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
    ~MacFloatingHost() override { delete releaseWatch.data(); if (qApp) qApp->removeEventFilter(this); }
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
    void setSnapping(bool enabled) { snapping = enabled; if (!enabled) { attachmentTarget = nullptr; snapPosition = {}; hidePreviews(); } }
    std::function<void(QWidget *, Qt::Edge)> attachRequested;
    void beginPointerDrag(const QPoint &global, bool continuing = false)
    {
        snapPosition = {}; attachmentTarget = nullptr;
        if (dragging) finishPointerDrag(true, true, "restarted");
        dragging = true; movedEnough = continuing; start = global; startGeometry = geometry();
        delete releaseWatch.data();
        releaseWatch = watchNativePointerRelease(this, [this](bool option) {
            if (dragging) { movePointerDrag(QCursor::pos(), option); finishPointerDrag(option, false, "native-mouse-up"); }
        });
        logEvent(QStringLiteral("drag.begin"), {{"window", objectName()}});
        // Cocoa can deactivate the window synchronously while capture is acquired.
        grabMouse();
        if (!dragging && QWidget::mouseGrabber() == this) {
            releaseMouse();
            logEvent(QStringLiteral("drag.capture.recovered"), {{"window", objectName()}, {"reason", "cancelled-during-grab"}});
        }
    }
protected:
    void hideEvent(QHideEvent *event) override
    {
        if (dragging) finishPointerDrag(true, true, "hidden");
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
        if (!dragging && QWidget::mouseGrabber() == this &&
            (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease ||
             event->type() == QEvent::MouseMove || event->type() == QEvent::KeyPress)) {
            releaseMouse();
            logEvent(QStringLiteral("drag.capture.recovered"), {{"window", objectName()}, {"reason", "orphaned-capture"}});
        }
        if (dragging && (event->type() == QEvent::ApplicationDeactivate || event->type() == QEvent::WindowDeactivate)) {
            if (event->type() == QEvent::ApplicationDeactivate || object == this) finishPointerDrag(true, true, "deactivated");
        }
        if (dragging && event->type() == QEvent::KeyPress && static_cast<QKeyEvent *>(event)->key() == Qt::Key_Escape) {
            setGeometry(startGeometry); finishPointerDrag(true, true, "escape"); return true;
        }
        if (dragging && event->type() == QEvent::MouseButtonRelease && static_cast<QMouseEvent *>(event)->button() == Qt::LeftButton) {
            auto mouse = static_cast<QMouseEvent *>(event);
            const bool option = mouse->modifiers().testFlag(Qt::AltModifier);
            movePointerDrag(mouse->globalPos(), option); finishPointerDrag(option, false, "qt-mouse-up"); return true;
        }
        if (dragging && event->type() == QEvent::MouseMove) {
            auto mouse = static_cast<QMouseEvent *>(event);
            if (!mouse->buttons().testFlag(Qt::LeftButton)) { finishPointerDrag(mouse->modifiers().testFlag(Qt::AltModifier), false, "button-no-longer-down"); return false; }
            movePointerDrag(mouse->globalPos(), mouse->modifiers().testFlag(Qt::AltModifier));
            return true;
        }
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
            dragEdges = edges; beginPointerDrag(mouse->globalPos()); return true;
        }
        return false;
    }
private:
    void movePointerDrag(const QPoint &global, bool bypass)
    {
        dragPointer = global;
        const QPoint delta = global - start;
        movedEnough |= delta.manhattanLength() >= QApplication::startDragDistance();
        if (dragEdges) setGeometry(resizeFromEdge(startGeometry, delta, dragEdges,
            minimumSize().expandedTo(minimumSizeHint()).expandedTo(QSize(80, 40)), maximumSize()));
        else {
            const QRect desired(startGeometry.topLeft() + delta, size());
            setGeometry(desired);
            if (movedEnough) previewSnap(desired, bypass);
            else { snapPosition = {}; attachmentTarget = nullptr; hidePreviews(); }
        }
    }
    void finishPointerDrag(bool bypass, bool cancelled, const QString &reason)
    {
        const auto target = attachmentTarget;
        const auto edge = attachmentEdge;
        const bool attach = !cancelled && movedEnough && !dragEdges && target && attachRequested && snapping && !bypass;
        if (!cancelled && !dragEdges && snapping && !bypass && snapPosition.isValid()) setGeometry(snapPosition);
        dragging = false; dragEdges = {}; snapPosition = {}; attachmentTarget = nullptr;
        delete releaseWatch.data(); releaseWatch = nullptr;
        hidePreviews();
        if (QWidget::mouseGrabber() == this) releaseMouse();
        logEvent(QStringLiteral("drag.end"), {{"window", objectName()}, {"reason", reason}, {"cancelled", cancelled}, {"attach", attach}});
        if (attach) QTimer::singleShot(0, this, [this, target, edge] { if (target && attachRequested) attachRequested(target, edge); });
    }
    void previewSnap(const QRect &desired, bool bypass)
    {
        snapPosition = {};
        attachmentTarget = nullptr;
        if (!snapping || bypass) { hidePreviews(); return; }
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
        QList<QLine> alignment;
        const QRect snapped = alignWindowEdges(snapWindowFrame(proposed, targets, displays), targets, alignment);
        for (int i = 0; i < 2; ++i) {
            if (i >= alignment.size()) { if (guideWidgets[i]) guideWidgets[i]->hide(); continue; }
            if (!guideWidgets[i]) {
                guideWidgets[i] = new QWidget(this, Qt::Tool | Qt::FramelessWindowHint | Qt::WindowTransparentForInput | Qt::WindowDoesNotAcceptFocus);
                guideWidgets[i]->setObjectName(QStringLiteral("unboundAlignmentGuide"));
                guideWidgets[i]->setAttribute(Qt::WA_ShowWithoutActivating); guideWidgets[i]->setAttribute(Qt::WA_TransparentForMouseEvents);
                guideWidgets[i]->setStyleSheet(QStringLiteral("background:#6582a0;"));
            }
            const auto line = alignment[i];
            guideWidgets[i]->setGeometry(QRect(line.p1(), line.p2()).normalized().adjusted(0, 0, 1, 1));
            guideWidgets[i]->show(); guideWidgets[i]->raise();
        }
        snapPosition = QRect(snapped.topLeft() - offset, desired.size());
        QRect outline = snapped;
        DockZone chosen;
        int closest = 100000;
        if (attachRequested) for (auto candidate : QApplication::topLevelWidgets()) {
            if (candidate == this || !candidate->isVisible() || candidate->isMinimized() ||
                !candidate->objectName().startsWith("unboundFloating_")) continue;
            QList<QWidget *> surfaces{candidate};
            for (auto child : candidate->findChildren<QWidget *>())
                if (child->objectName().startsWith("unboundGroupCell_") && child->isVisible()) surfaces.append(child);
            for (auto slot : surfaces) {
                const auto frame = slot->isWindow() ? slot->frameGeometry() : QRect(slot->parentWidget()->mapToGlobal(QPoint()), slot->parentWidget()->size());
                const auto zone = localDockZone(frame, dragPointer);
                if (!zone.target.isValid()) continue;
                const int distance = (zone.active ? -10000 : 0) + (zone.target.center() - dragPointer).manhattanLength() +
                    (frame.contains(dragPointer) ? 0 : 1000) + (slot == candidate && surfaces.size() > 1 ? 50 : 0);
                if (distance >= closest) continue;
                closest = distance; chosen = zone;
                attachmentTarget = zone.active ? slot : nullptr;
            }
        }
        if (chosen.target.isValid()) {
            if (!dropMarker) {
                auto label = new QLabel(this, Qt::Tool | Qt::FramelessWindowHint | Qt::WindowTransparentForInput | Qt::WindowDoesNotAcceptFocus);
                label->setObjectName(QStringLiteral("unboundDockTarget")); label->setAlignment(Qt::AlignCenter);
                label->setAttribute(Qt::WA_ShowWithoutActivating); label->setAttribute(Qt::WA_TransparentForMouseEvents);
                label->setStyleSheet(QStringLiteral("background:#34475b; color:white; border:1px solid #80a5c9; border-radius:4px;"));
                dropMarker = label;
            }
            static_cast<QLabel *>(dropMarker)->setText(chosen.edge == Qt::LeftEdge ? QString::fromUtf8("←") : chosen.edge == Qt::RightEdge ? QString::fromUtf8("→") : chosen.edge == Qt::TopEdge ? QString::fromUtf8("↑") : QString::fromUtf8("↓"));
            dropMarker->setGeometry(chosen.target); dropMarker->show(); dropMarker->raise();
            attachmentEdge = chosen.edge;
            if (chosen.active) outline = chosen.preview;
        } else if (dropMarker) dropMarker->hide();
        if (snapped == proposed && !attachmentTarget) { if (preview) preview->hide(); return; }
        if (!preview) {
            preview = new QWidget(this, Qt::Tool | Qt::FramelessWindowHint | Qt::WindowTransparentForInput | Qt::WindowDoesNotAcceptFocus);
            preview->setObjectName(QStringLiteral("unboundSnapPreview"));
            preview->setAttribute(Qt::WA_ShowWithoutActivating);
            preview->setAttribute(Qt::WA_TransparentForMouseEvents);
            preview->setStyleSheet(QStringLiteral("background: rgba(80,140,210,45); border: 2px solid #5099df;"));
        }
        preview->setGeometry(outline); preview->show(); preview->raise();
        if (dropMarker && dropMarker->isVisible()) dropMarker->raise();
    }
    void hidePreviews() { if (preview) preview->hide(); if (dropMarker) dropMarker->hide(); for (auto guide : guideWidgets) if (guide) guide->hide(); }
    WindowChrome chrome = WindowChrome::Standard;
    bool snapping = true;
    QWidget *preview = nullptr;
    QWidget *dropMarker = nullptr;
    QWidget *guideWidgets[2] = {nullptr, nullptr};
    QPoint dragPointer;
    QRect snapPosition;
    QPointer<QWidget> attachmentTarget;
    Qt::Edge attachmentEdge = Qt::LeftEdge;
    bool dragging = false;
    bool movedEnough = false;
    Qt::Edges dragEdges;
    QPoint start;
    QRect startGeometry;
    QPointer<QObject> releaseWatch;
};
}
