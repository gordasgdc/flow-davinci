// SPDX-License-Identifier: MIT
#pragma once
#include <QBoxLayout>
#include <QCloseEvent>
#include <QGridLayout>
#include <QLabel>
#include <QPointer>
#include <QSplitter>
#include <QStackedWidget>
#include <QTimer>
#include <QWidget>
#include <functional>
#include "diagnostics.h"
#include "viewport.h"
#include "floatinghost.h"

namespace Unbound {
// Experimental Mac session. Each instance owns one host and preserves one slot.
class MacPanelSession : public QObject
{
public:
    explicit MacPanelSession(QObject *owner = nullptr) : QObject(owner)
    {
        arrangeTimer.setSingleShot(true);
        arrangeTimer.setInterval(180);
        connect(&arrangeTimer, &QTimer::timeout, this, [this] {
            if (constrain) keepOnDisplay(host);
            if (arrangementChanged) arrangementChanged();
        });
    }
    ~MacPanelSession() override { changed = {}; arrangementChanged = {}; dock(); }
    bool active() const { return !host.isNull(); }
    QWidget *window() const { return host; }
    std::function<void()> changed;
    std::function<void()> arrangementChanged;
    void setChrome(WindowChrome value) { chrome = value; if (host) static_cast<MacFloatingHost *>(host.data())->setChrome(value); }
    bool hiddenByUser() const { return userHidden; }
    void setConstrained(bool enabled) { constrain = enabled; if (enabled) keepOnDisplay(host); }
    void bringOnScreen() { keepOnDisplay(host); if (arrangementChanged) arrangementChanged(); }
    void setFloatingGeometry(const QRect &r) { if (host && r.isValid()) { host->setGeometry(r); if (constrain) keepOnDisplay(host); } }
    void setHiddenByUser(bool hide)
    {
        userHidden = hide;
        if (host) {
            if (hide) host->hide();
            else if (originalPage && originalPage->isVisible()) { host->showNormal(); keepOnDisplay(host); host->raise(); host->activateWindow(); }
        }
        logEvent(hide ? QStringLiteral("panel.hide") : QStringLiteral("panel.show"), {{"widget", widgetState(host)}});
        if (changed) changed();
        if (arrangementChanged) arrangementChanged();
    }

    QString undockContents(QWidget *slot, QWidget *page, const QString &title)
    {
        if (active()) return QStringLiteral("This panel is already floating.");
        if (!slot || !page || !page->isAncestorOf(slot) || !slot->isVisible() || !slot->layout())
            return QStringLiteral("Show this panel on the Edit page first.");
        originalParent = slot;
        originalPage = page;
        originalLayout = slot->layout();
        host = new MacFloatingHost;
        host->setAttribute(Qt::WA_QuitOnClose, false);
        host->setObjectName(QStringLiteral("unboundFloating_") + slot->objectName());
        host->setWindowTitle(title);
        host->resize(slot->size().expandedTo(QSize(320, 240)));
        host->setLayout(originalLayout);
        if (slot->layout() || host->layout() != originalLayout) {
            if (!slot->layout() && host->layout() == originalLayout) slot->setLayout(originalLayout);
            delete host.data(); clear(); return QStringLiteral("Could not transfer this panel's content layout.");
        }
        transferredLayout = true;
        placeholderLayout = new QVBoxLayout(slot);
        auto marker = new QLabel(title + QStringLiteral(" is floating."), slot);
        marker->setAlignment(Qt::AlignCenter);
        placeholder = marker;
        placeholderLayout->addWidget(marker);
        host->installEventFilter(this);
        page->installEventFilter(this);
        parentDestroyed = connect(slot, &QObject::destroyed, this, [this] {
            if (originalPage) originalPage->removeEventFilter(this);
            if (host) host->deleteLater();
            clear();
        });
        host->show();
        static_cast<MacFloatingHost *>(host.data())->setChrome(chrome);
        if (constrain) keepOnDisplay(host);
        logEvent(QStringLiteral("panel.contents.undock"), {{"panel", title}, {"slot", widgetState(slot)}});
        if (changed) changed();
        return {};
    }

    QString undock(QWidget *widget, QWidget *page, const QString &title)
    {
        logEvent(QStringLiteral("panel.undock.request"), {{"panel", title}, {"widget", widgetState(widget)}});
        if (active()) return QStringLiteral("This panel is already floating.");
        if (!widget || !page || !page->isAncestorOf(widget) || !widget->isVisible() || widget->isWindow())
            return QStringLiteral("Show this panel on the Edit page first.");
        QWidget *parent = widget->parentWidget();
        auto splitter = qobject_cast<QSplitter *>(parent);
        auto stack = qobject_cast<QStackedWidget *>(parent);
        QLayout *layout = parent->layout();
        if (splitter || stack) layout = nullptr;
        if (layout && !qobject_cast<QBoxLayout *>(layout) && !qobject_cast<QGridLayout *>(layout))
            return QStringLiteral("This panel's layout is not supported yet.");
        if ((layout && layout->indexOf(widget) < 0) || (splitter && splitter->indexOf(widget) < 0))
            return QStringLiteral("The panel is not in its expected layout.");

        content = widget;
        originalParent = parent;
        originalLayout = layout;
        originalSplitter = splitter;
        originalStack = stack;
        if (stack) stackIndex = stack->indexOf(widget);
        originalPage = page;
        geometry = widget->geometry();
        flags = widget->windowFlags();
        hidden = widget->isHidden();
        if (splitter) splitterSizes = splitter->sizes();
        // QSplitter automatically inserts new children; replaceWidget needs an outsider.
        auto marker = new QLabel(title + QStringLiteral(" is floating."), (splitter || stack) ? nullptr : parent);
        marker->setAlignment(Qt::AlignCenter);
        marker->setSizePolicy(widget->sizePolicy());
        marker->setMinimumSize(widget->minimumSize());
        marker->setMaximumSize(widget->maximumSize());
        placeholder = marker;
        if (stack) {
            const bool selected = stack->currentWidget() == widget;
            stack->removeWidget(widget);
            stack->insertWidget(stackIndex, marker);
            if (selected) stack->setCurrentWidget(marker);
        } else if (layout) {
            auto item = layout->replaceWidget(widget, marker, Qt::FindDirectChildrenOnly);
            if (!item) { delete marker; clear(); return QStringLiteral("Could not preserve the panel slot."); }
            delete item;
        } else if (splitter) {
            const int index = splitter->indexOf(widget);
            if (splitter->replaceWidget(index, marker) != widget) {
                delete marker; clear(); return QStringLiteral("Could not preserve the splitter slot.");
            }
            splitter->setSizes(splitterSizes);
        } else {
            marker->setGeometry(geometry);
        }
        host = new MacFloatingHost;
        host->setAttribute(Qt::WA_QuitOnClose, false);
        host->setWindowTitle(title);
        host->setObjectName(QStringLiteral("unboundFloating_") + widget->objectName());
        host->resize(widget->size().expandedTo(QSize(320, 240)));
        auto box = new QVBoxLayout(host);
        box->setContentsMargins(0, 0, 0, 0);
        box->addWidget(widget);
        marker->show();
        widget->show();
        host->installEventFilter(this);
        page->installEventFilter(this);
        parentDestroyed = connect(parent, &QObject::destroyed, this, [this] {
            // The original owner is gone; its displaced content ends with it.
            if (originalPage) originalPage->removeEventFilter(this);
            if (host) host->deleteLater();
            clear();
        });
        host->show();
        static_cast<MacFloatingHost *>(host.data())->setChrome(chrome);
        if (constrain) keepOnDisplay(host);
        logEvent(QStringLiteral("panel.undock.complete"), {{"panel", title}, {"widget", widgetState(content)},
            {"original_parent", originalParent->objectName()},
            {"layout", originalLayout ? originalLayout->metaObject()->className() : "none"}});
        if (changed) changed();
        return {};
    }

    QString dock()
    {
        logEvent(QStringLiteral("panel.dock.request"), {{"widget", widgetState(content)},
            {"active", active()}, {"original_parent", originalParent ? originalParent->objectName() : QString()}});
        if (!active()) return QStringLiteral("This panel is already docked.");
        if (transferredLayout) {
            if (!originalParent || !originalLayout || !placeholderLayout ||
                originalParent->layout() != placeholderLayout || host->layout() != originalLayout)
                return QStringLiteral("Resolve changed this panel's content layout.");
            delete placeholderLayout.data();
            delete placeholder.data();
            originalParent->setLayout(originalLayout);
            if (originalParent->layout() != originalLayout)
                return QStringLiteral("Could not restore this panel's content layout.");
            originalLayout->activate();
            disconnect(parentDestroyed);
            if (originalPage) originalPage->removeEventFilter(this);
            auto oldHost = host.data();
            oldHost->removeEventFilter(this);
            logEvent(QStringLiteral("panel.contents.dock"), {{"slot", widgetState(originalParent)}});
            clear(); oldHost->hide(); oldHost->deleteLater();
            if (changed) changed();
            return {};
        }
        if (!content || !originalParent || !placeholder || content->parentWidget() != host)
            return QStringLiteral("Resolve changed the panel hierarchy; restoration was stopped.");
        if (originalStack) {
            if (originalStack->indexOf(placeholder) < 0)
                return QStringLiteral("The original viewer slot changed.");
            const bool selected = originalStack->currentWidget() == placeholder;
            originalStack->removeWidget(placeholder);
            originalStack->insertWidget(qMin(stackIndex, originalStack->count()), content);
            if (selected) originalStack->setCurrentWidget(content);
        } else if (originalSplitter) {
            const int index = originalSplitter->indexOf(placeholder);
            if (index < 0 || originalSplitter->replaceWidget(index, content) != placeholder)
                return QStringLiteral("The original splitter slot changed.");
            originalSplitter->setSizes(splitterSizes);
        } else if (originalLayout) {
            if (originalParent->layout() != originalLayout || originalLayout->indexOf(placeholder) < 0)
                return QStringLiteral("The original layout changed.");
            auto item = originalLayout->replaceWidget(placeholder, content, Qt::FindDirectChildrenOnly);
            if (!item) return QStringLiteral("Could not restore the panel slot.");
            delete item;
            content->setParent(originalParent, flags);
        } else {
            if (originalParent->layout()) return QStringLiteral("Resolve added a layout to this panel slot.");
            content->setParent(originalParent, flags);
        }
        if (content->parentWidget() != originalParent || content->isWindow())
            return QStringLiteral("Resolve did not retain the restored panel in its original parent.");
        content->setGeometry(geometry);
        content->setVisible(!hidden && (!originalStack || originalStack->currentWidget() == content));
        if (originalLayout) originalLayout->activate();
        disconnect(parentDestroyed);
        if (originalPage) originalPage->removeEventFilter(this);
        auto oldHost = host.data();
        oldHost->removeEventFilter(this);
        delete placeholder.data();
        const auto restored = content;
        const auto destination = originalParent;
        logEvent(QStringLiteral("panel.dock.complete"), {{"widget", widgetState(content)}});
        clear();
        oldHost->hide();
        oldHost->deleteLater();
        QTimer::singleShot(0, this, [restored, destination] {
            logEvent(QStringLiteral("panel.dock.verify"), {{"widget", widgetState(restored)},
                {"passed", restored && destination && restored->parentWidget() == destination && !restored->isWindow()}});
        });
        if (changed) changed();
        return {};
    }
protected:
    bool eventFilter(QObject *object, QEvent *event) override
    {
        if (object == host && event->type() == QEvent::Close) {
            const QString error = dock();
            logEvent(QStringLiteral("panel.close.result"), {{"success", error.isEmpty()}, {"error", error}});
            static_cast<QCloseEvent *>(event)->setAccepted(error.isEmpty());
            return true;
        }
        if (object == host && (event->type() == QEvent::Move || event->type() == QEvent::Resize)) arrangeTimer.start();
        if (object == originalPage && host) {
            if (event->type() == QEvent::Hide) host->hide();
            if (event->type() == QEvent::Show && !userHidden) host->show();
        }
        return false;
    }
private:
    void clear()
    {
        host = nullptr; content = nullptr; placeholder = nullptr;
        originalParent = nullptr; originalLayout = nullptr;
        originalSplitter = nullptr; originalPage = nullptr;
        originalStack = nullptr;
        splitterSizes.clear();
        transferredLayout = false;
        placeholderLayout = nullptr;
        userHidden = false;
        arrangeTimer.stop();
    }
    QPointer<QWidget> host, content, placeholder, originalParent, originalPage;
    QPointer<QLayout> originalLayout;
    QPointer<QSplitter> originalSplitter;
    QPointer<QStackedWidget> originalStack;
    QPointer<QLayout> placeholderLayout;
    bool transferredLayout = false;
    bool userHidden = false;
    bool constrain = true;
    QTimer arrangeTimer;
    WindowChrome chrome = WindowChrome::Standard;
    int stackIndex = -1;
    QMetaObject::Connection parentDestroyed;
    QList<int> splitterSizes;
    QRect geometry;
    Qt::WindowFlags flags;
    bool hidden = false;
};
}
