// SPDX-License-Identifier: MIT
#pragma once
#include <QApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QLabel>
#include <QToolButton>
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
        pinButton = new QToolButton(this); pinButton->setObjectName("unboundPinWindow");
        pinButton->setText(QString::fromUtf8("◇")); pinButton->setCheckable(true); pinButton->setAutoRaise(true);
        pinButton->setFocusPolicy(Qt::NoFocus); pinButton->setToolTip("Keep this window above the Resolve workspace");
        connect(pinButton, &QToolButton::toggled, this, [this](bool pinned) {
            setProperty("unboundPinned", pinned); pinButton->setText(pinned ? QString::fromUtf8("◆") : QString::fromUtf8("◇"));
            logEvent(QStringLiteral("window.pin"), {{"window", objectName()}, {"pinned", pinned}});
        });
    }
    void setPinned(bool pinned) { pinButton->setChecked(pinned); }
    bool pinned() const { return pinButton->isChecked(); }
    ~MacFloatingHost() override { delete releaseWatch.data(); if (qApp) qApp->removeEventFilter(this); }
    void setChrome(WindowChrome value)
    {
        const bool visible = isVisible();
        const QRect position = geometry();
        chrome = value;
        setWindowFlag(Qt::FramelessWindowHint, value == WindowChrome::Hidden);
        const int top = value == WindowChrome::Compact ? 28 : 24;
        setContentsMargins(0, top, 0, 0);
        if (visible) show();
        applyNativeChrome(this, value == WindowChrome::Compact);
        if (position.isValid()) setGeometry(position);
        pinButton->setGeometry(value == WindowChrome::Compact ? 84 : qMax(0, width() - 25), 0, 24, top);
        pinButton->raise(); update();
    }
    void setSnapping(bool enabled) { snapping = enabled; if (!enabled) { attachmentTarget = nullptr; snapPosition = {}; hidePreviews(); } }
    std::function<void(QWidget *, Qt::Edge)> attachRequested;
    void beginPointerDrag(const QPoint &global, bool continuing = false)
    {
        snapPosition = {}; attachmentTarget = nullptr;
        if (dragging) finishPointerDrag(true, true, "restarted");
        ++dragGeneration;
        dragging = true; movedEnough = continuing; start = global; startGeometry = geometry();
        delete releaseWatch.data();
        releaseWatch = watchNativePointerRelease(this, [this](QPoint position, bool option) {
            if (dragging) { movePointerDrag(position, option); finishPointerDrag(option, false, "native-mouse-up"); }
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
    void resizeEvent(QResizeEvent *event) override
    {
        QWidget::resizeEvent(event);
        if (pinButton) { pinButton->setGeometry(chrome == WindowChrome::Compact ? 84 : qMax(0, width() - 25), 0, 24, contentsMargins().top()); pinButton->raise(); }
    }
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
            painter.drawText(QRect(110, 0, qMax(0, width() - 118), 28), Qt::AlignVCenter | Qt::AlignLeft, windowTitle());
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
        logEvent(QStringLiteral("drag.end"), {{"window", objectName()}, {"reason", reason}, {"cancelled", cancelled}, {"attach", attach}, {"target", target ? target->objectName() : QString()}, {"edge", int(edge)}});
        const auto generation = dragGeneration;
        if (attach) QTimer::singleShot(0, this, [this, target, edge, generation] {
            if (generation == dragGeneration && !dragging && target && target->isVisible() && attachRequested) attachRequested(target, edge);
        });
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
        const bool align = QApplication::keyboardModifiers().testFlag(Qt::ShiftModifier);
        const QRect snapped = align ? alignWindowEdges(snapWindowFrame(proposed, targets, displays), targets, alignment) : proposed;
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
        snapPosition = align ? QRect(snapped.topLeft() - offset, desired.size()) : QRect();
        QRect outline = snapped;
        DockZone chosen;
        int closest = 100000;
        // Only the frontmost eligible destination can offer docking controls.
        QWidget *destination = nullptr;
        const auto ordered = nativeOrderedWindowNumbers();
        for (auto number : ordered) {
            QWidget *hit = nullptr;
            for (auto candidate : QApplication::topLevelWidgets()) {
                if (candidate == this || candidate == preview || candidate == dropMarker ||
                    candidate == guideWidgets[0] || candidate == guideWidgets[1] ||
                    !candidate->isVisible() || candidate->isMinimized() || nativeWindowNumber(candidate) != number) continue;
                const bool eligible = candidate->objectName().startsWith("unboundFloating_");
                const auto area = eligible ? candidate->frameGeometry().adjusted(-36, -36, 36, 36) : candidate->frameGeometry();
                if (area.contains(dragPointer)) { hit = candidate; break; }
            }
            if (hit) {
                if (hit->objectName().startsWith("unboundFloating_")) destination = hit;
                break;
            }
        }
        if (attachRequested) for (auto candidate : QApplication::topLevelWidgets()) {
            if (candidate != destination) continue;
            QList<QWidget *> surfaces{candidate};
            for (auto child : candidate->findChildren<QWidget *>())
                if (child->objectName().startsWith("unboundGroupCell_") && child->isVisible()) surfaces.append(child);
            for (auto slot : surfaces) {
                const auto frame = slot->isWindow() ? slot->frameGeometry() : QRect(slot->parentWidget()->mapToGlobal(QPoint()), slot->parentWidget()->size());
                const auto zone = slot == candidate && surfaces.size() > 1 ? outerDockZone(frame, dragPointer) : localDockZone(frame, dragPointer);
                if (!zone.target.isValid() || (slot == candidate && surfaces.size() > 1 && zone.edge == Qt::Edge(0))) continue;
                const int distance = (zone.active ? -10000 : 0) + (zone.target.center() - dragPointer).manhattanLength() +
                    (frame.contains(dragPointer) ? 0 : 1000) + (slot == candidate && surfaces.size() > 1 ? 50 : 0);
                if (distance >= closest) continue;
                closest = distance; chosen = zone;
                attachmentTarget = zone.active ? slot : nullptr;
            }
        }
        const QString selection = attachmentTarget ? attachmentTarget->objectName() + QString::number(int(chosen.edge)) : QString();
        if (selection != lastDockSelection) {
            lastDockSelection = selection;
            logEvent(QStringLiteral("dock.target.selection"), {{"source", objectName()},
                {"target", attachmentTarget ? attachmentTarget->objectName() : QString()}, {"active", bool(attachmentTarget)},
                {"kind", chosen.edge == Qt::Edge(0) ? "tab" : attachmentTarget && attachmentTarget->isWindow() && surfacesAreGrouped(attachmentTarget) ? "group-split" : "panel-split"},
                {"edge", int(chosen.edge)}});
        }
        if (chosen.target.isValid()) {
            if (!dropMarker) {
                auto label = new QLabel(this, Qt::Tool | Qt::FramelessWindowHint | Qt::WindowTransparentForInput | Qt::WindowDoesNotAcceptFocus);
                label->setObjectName(QStringLiteral("unboundDockTarget")); label->setAlignment(Qt::AlignCenter);
                label->setAttribute(Qt::WA_ShowWithoutActivating); label->setAttribute(Qt::WA_TransparentForMouseEvents);
                label->setStyleSheet(QStringLiteral("background:#34475b; color:white; border:1px solid #80a5c9; border-radius:4px;"));
                dropMarker = label;
            }
            dropMarker->setToolTip(chosen.edge == Qt::Edge(0) ? QStringLiteral("Add as tab") :
                attachmentTarget && attachmentTarget->isWindow() && surfacesAreGrouped(attachmentTarget) ? QStringLiteral("Split group") : QStringLiteral("Split panel"));
            static_cast<QLabel *>(dropMarker)->setText(chosen.edge == Qt::Edge(0) ? QString::fromUtf8("▣") : chosen.edge == Qt::LeftEdge ? QString::fromUtf8("←") : chosen.edge == Qt::RightEdge ? QString::fromUtf8("→") : chosen.edge == Qt::TopEdge ? QString::fromUtf8("↑") : QString::fromUtf8("↓"));
            dropMarker->setGeometry(chosen.target); dropMarker->show(); dropMarker->raise();
            attachmentEdge = chosen.edge;
            if (chosen.active) outline = chosen.preview;
        } else if (dropMarker) dropMarker->hide();
        if (!attachmentTarget) { if (preview) preview->hide(); return; }
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
    static bool surfacesAreGrouped(QWidget *widget) { return widget && widget->objectName().startsWith("unboundFloating_group_"); }
    void hidePreviews() { if (preview) preview->hide(); if (dropMarker) dropMarker->hide(); for (auto guide : guideWidgets) if (guide) guide->hide(); }
    QToolButton *pinButton = nullptr;
    WindowChrome chrome = WindowChrome::Standard;
    bool snapping = true;
    QWidget *preview = nullptr;
    QWidget *dropMarker = nullptr;
    QWidget *guideWidgets[2] = {nullptr, nullptr};
    QString lastDockSelection;
    QPoint dragPointer;
    QRect snapPosition;
    QPointer<QWidget> attachmentTarget;
    Qt::Edge attachmentEdge = Qt::LeftEdge;
    unsigned dragGeneration = 0;
    bool dragging = false;
    bool movedEnough = false;
    Qt::Edges dragEdges;
    QPoint start;
    QRect startGeometry;
    QPointer<QObject> releaseWatch;
};
}
