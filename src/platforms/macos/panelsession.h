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
#include <QScopedValueRollback>
#include <QWidget>
#include <functional>
#include "diagnostics.h"
#include "viewport.h"
#include "floatinghost.h"
#include "panelwidth.h"
#include "cinemaviewer.h"
#include "uiworktiming.h"

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
            if (constrain && window()) keepOnDisplay(window()->window());
            if (arrangementChanged) arrangementChanged();
        });
    }
    ~MacPanelSession() override { changed = {}; arrangementChanged = {}; dock(); }
    bool active() const { return !host.isNull(); }
    bool dockPending() const { return pendingDock; }
    QWidget *window() const { return grouped() ? groupContainer.data() : host.data(); }
    std::function<void()> changed;
    std::function<void()> expandedClosed;
    std::function<void()> arrangementChanged;
    std::function<void()> releaseGroup;
    std::function<void(QWidget *, Qt::Edge)> attachRequested;
    bool grouped() const { return !groupContainer.isNull(); }
    QRect floatingGeometry() const { return grouped() ? detachedGeometry : host ? host->geometry() : QRect(); }
    QWidget *nativeOwnerWindow() const { return originalMain; }
    QWidget *pageWidget() const { return originalPage; }
    QString attachToGroup(QWidget *container, bool deferHostHide = false)
    {
        UiWorkTiming timing("panel.attach-to-group",host);
        if (!host || !host->layout() || grouped() || !container || container->layout() || container->window()->parentWidget() != originalMain)
            return QStringLiteral("Panel cannot join this group.");
        detachedGeometry = host->geometry();
        QScopedValueRollback<bool> moving(recovering, true);
        {
            UiWorkTiming transfer("panel.group.layout-transfer",host);
            container->setLayout(host->layout());
        }
        if (host->layout() || !container->layout()) return QStringLiteral("Could not transfer panel layout into group.");
        groupContainer = container;
        if (relaxedWidth) groupWidth.apply(container);
        groupContainer->installEventFilter(this);
        if (!deferHostHide) {
            UiWorkTiming hide("panel.group.hide-old-host",host);
            host->hide();
        }
        groupContainer->setVisible(!userHidden);
        return {};
    }
    void finishGroupTransfer()
    {
        if (!grouped() || !host) return;
        UiWorkTiming hide("panel.group.hide-old-host",host);
        host->hide();
    }
    void detachFromGroup()
    {
        endGroupedExpansion();
        if (!grouped()) return;
        if (!host) return;
        QScopedValueRollback<bool> moving(recovering, true);
        groupWidth.restore();
        if (groupContainer->layout()) host->setLayout(groupContainer->layout());
        groupContainer->removeEventFilter(this);
        groupContainer = nullptr;
        static_cast<MacFloatingHost *>(host.data())->setChrome(chrome);
        setFloatingGeometry(detachedGeometry);
        host->setVisible(!userHidden && originalPage && originalPage->isVisible());
    }
    QWidget *expandedWindow() const { return expandedHost; }
    bool beginGroupedExpansion()
    {
        if (!grouped() || expandedHost || !groupContainer->layout()) return false;
        QScopedValueRollback<bool> moving(recovering, true);
        expandedHost = new MacFloatingHost(originalMain);
        expandedHost->setObjectName(QStringLiteral("unboundFloating_expanded_") + groupContainer->objectName());
        expandedHost->setWindowTitle(groupContainer->windowTitle());
        expandedHost->setAttribute(Qt::WA_QuitOnClose, false);
        expandedHost->setLayout(groupContainer->layout());
        static_cast<MacFloatingHost *>(expandedHost.data())->setChrome(chrome);
        expandedHost->setGeometry(QRect(groupContainer->mapToGlobal(QPoint()), groupContainer->size()));
        expandedHost->installEventFilter(this); expandedHost->show(); return true;
    }
    void endGroupedExpansion()
    {
        if (!expandedHost) return;
        QScopedValueRollback<bool> moving(recovering, true);
        if (groupContainer && expandedHost->layout()) groupContainer->setLayout(expandedHost->layout());
        expandedHost->removeEventFilter(this); expandedHost->hide(); expandedHost->deleteLater(); expandedHost = nullptr;
    }
    void setChrome(WindowChrome value) { chrome = value; if (host && !grouped()) static_cast<MacFloatingHost *>(host.data())->setChrome(value); }
    bool pinned() const { return host && host->property("unboundPinned").toBool(); }
    void setPinned(bool value) { if (host) static_cast<MacFloatingHost *>(host.data())->setPinned(value); }
    void setSnapping(bool enabled) { snapping = enabled; if (host) static_cast<MacFloatingHost *>(host.data())->setSnapping(enabled); }
    bool hiddenByUser() const { return userHidden; }
    void setConstrained(bool enabled) { constrain = enabled; if (enabled && window()) keepOnDisplay(window()->window()); }
    void bringOnScreen() { if (window()) keepOnDisplay(window()->window()); if (arrangementChanged) arrangementChanged(); }
    void centerOnCurrentDisplay()
    {
        if (!window()) return;
        setHiddenByUser(false);
        auto top = window()->window(); top->showNormal();
        auto screen = QGuiApplication::screenAt(QCursor::pos()); if (!screen) screen = QGuiApplication::primaryScreen();
        if (!screen) return;
        QRect frame(QPoint(), top->frameGeometry().size().boundedTo(screen->availableGeometry().size()));
        frame.moveCenter(screen->availableGeometry().center());
        const auto decoration = top->frameGeometry().size() - top->size();
        top->resize(frame.size() - decoration); top->move(frame.topLeft()); keepOnDisplay(top);
        top->raise(); top->activateWindow();
        logEvent(QStringLiteral("panel.center"), {{"host", widgetState(top)}});
        if (arrangementChanged) arrangementChanged();
    }
    void setFloatingGeometry(const QRect &r) { if (grouped()) { if (r.isValid()) detachedGeometry = r; } else if (host && r.isValid()) { host->setGeometry(r); if (constrain) keepOnDisplay(host); } }
    void setHiddenByUser(bool hide, bool activate = true)
    {
        userHidden = hide;
        if (host) {
            auto surface = window();
            if (hide) surface->hide();
            else if (originalPage && originalPage->isVisible()) {
                if (activate) {
                    auto window = surface->window();
                    if (!window->isVisible() || window->isMinimized()) window->showNormal();
                    surface->show();
                    // A door changes space inside its group; it must not move
                    // the group's outer frame or repeatedly reactivate it.
                    if (!grouped()) keepOnDisplay(window);
                    if (QApplication::activeWindow() != window) { window->raise(); window->activateWindow(); }
                } else {
                    // Viewer-mode commands change presentation without stealing
                    // editing focus or reordering an already-visible group.
                    auto window = surface->window();
                    const bool prior = window->testAttribute(Qt::WA_ShowWithoutActivating);
                    window->setAttribute(Qt::WA_ShowWithoutActivating);
                    window->show(); surface->show();
                    window->setAttribute(Qt::WA_ShowWithoutActivating, prior);
                }
            }
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
        originalMain = page->window();
        originalLayout = slot->layout();
        ++generation;
        host = new MacFloatingHost(originalMain);
        static_cast<MacFloatingHost *>(host.data())->attachRequested = [this](QWidget *target, Qt::Edge edge) { if (attachRequested) attachRequested(target, edge); };
        static_cast<MacFloatingHost *>(host.data())->setSnapping(snapping);
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
        relaxedWidth = slot->objectName() == QLatin1String("frameEditPageMediaPool");
        if (relaxedWidth) { widthAllowance.apply(host); host->setProperty("unboundRelaxedWidth", true); }
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

    QString undock(QWidget *widget, QWidget *page, const QString &title, bool retainHiddenSurface = false)
    {
        logEvent(QStringLiteral("panel.undock.request"), {{"panel", title}, {"widget", widgetState(widget)}});
        if (active()) return QStringLiteral("This panel is already floating.");
        if (!widget || !page || !page->isAncestorOf(widget) ||
            (!widget->isVisible() && !(retainHiddenSurface && page->isVisible())) || widget->isWindow())
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
        originalMain = page->window();
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
        ++generation;
        host = new MacFloatingHost(originalMain);
        static_cast<MacFloatingHost *>(host.data())->attachRequested = [this](QWidget *target, Qt::Edge edge) { if (attachRequested) attachRequested(target, edge); };
        static_cast<MacFloatingHost *>(host.data())->setSnapping(snapping);
        host->setAttribute(Qt::WA_QuitOnClose, false);
        host->setWindowTitle(title);
        host->setObjectName(QStringLiteral("unboundFloating_") + widget->objectName());
        host->resize(widget->size().expandedTo(QSize(320, 240)));
        auto box = new QVBoxLayout(host);
        box->setContentsMargins(0, 0, 0, 0);
        box->addWidget(widget);
        relaxedWidth = widget->objectName() == QLatin1String("frameConformTimelines") ||
            widget->objectName() == QLatin1String("widgetEditInspector") ||
            widget->objectName() == QLatin1String("frameEditPageMediaPool");
        if (relaxedWidth) widthAllowance.apply(widget);
        // A detached tools row must not leave a second empty tools row in Timeline.
        marker->setVisible(widget->objectName() != QLatin1String("frameEditToolbarTop"));
        widget->show();
        widget->installEventFilter(this);
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
        endGroupedExpansion();
        if (releaseGroup) { auto release = releaseGroup; release(); }
        logEvent(QStringLiteral("panel.dock.request"), {{"widget", widgetState(content)},
            {"active", active()}, {"original_parent", originalParent ? originalParent->objectName() : QString()}});
        if (!active()) return QStringLiteral("This panel is already docked.");
        if (!transferredLayout && content && originalPage && !originalPage->isVisible() &&
            content->parentWidget() != host && content->parentWidget() != originalParent && nativeOwner(content->parentWidget())) {
            pendingDock = true;
            logEvent(QStringLiteral("panel.dock.deferred"), {{"widget", widgetState(content)}});
            if (changed) changed();
            return {};
        }
        QScopedValueRollback<bool> restoring(docking, true);
        if (transferredLayout) {
            if (!originalParent || !originalLayout || !placeholderLayout ||
                originalParent->layout() != placeholderLayout || host->layout() != originalLayout)
                return QStringLiteral("Resolve changed this panel's content layout.");
            widthAllowance.restore();
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
        if (!content || !originalParent || !placeholder)
            return QStringLiteral("Resolve changed the panel hierarchy; restoration was stopped.");
        if (content->parentWidget() != host && content->parentWidget() != originalParent && !nativeOwner(content->parentWidget()))
            return QStringLiteral("This panel belongs to an unexpected window; restoration was stopped.");
        const bool reclaimed = content->parentWidget() == originalParent;
        QLayout *currentLayout = originalParent->layout();
        if (reclaimed && (!currentLayout || currentLayout->indexOf(content) >= 0 || originalStack || originalSplitter)) {
            if (currentLayout) currentLayout->removeWidget(placeholder);
            logEvent(QStringLiteral("panel.dock.native-reclaimed"), {{"widget", widgetState(content)}});
        } else if (originalStack) {
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
        } else if (currentLayout) {
            if (currentLayout->indexOf(placeholder) < 0)
                return QStringLiteral("The original layout changed.");
            auto item = currentLayout->replaceWidget(placeholder, content, Qt::FindDirectChildrenOnly);
            if (!item) return QStringLiteral("Could not restore the panel slot.");
            delete item;
            content->setParent(originalParent, flags);
        } else {
            if (originalParent->layout()) return QStringLiteral("Resolve added a layout to this panel slot.");
            content->setParent(originalParent, flags);
        }
        if (content->parentWidget() != originalParent || content->isWindow())
            return QStringLiteral("Resolve did not retain the restored panel in its original parent.");
        widthAllowance.restore();
        content->setGeometry(geometry);
        content->setVisible(!hidden && (!originalStack || originalStack->currentWidget() == content));
        if (currentLayout) currentLayout->activate();
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
        if (object == expandedHost && event->type() == QEvent::Close) {
            if (expandedClosed) expandedClosed(); else endGroupedExpansion(); return true;
        }
        if (object == host && event->type() == QEvent::Close) {
            const QString error = dock();
            logEvent(QStringLiteral("panel.close.result"), {{"success", error.isEmpty()}, {"error", error}});
            static_cast<QCloseEvent *>(event)->setAccepted(error.isEmpty());
            return true;
        }
        if ((object == host || object == groupContainer) && (event->type() == QEvent::Move || event->type() == QEvent::Resize)) arrangeTimer.start();
        if (object == content && !docking && !recovering &&
            (event->type() == QEvent::Hide || event->type() == QEvent::ParentChange)) {
            logEvent(QStringLiteral("panel.native-change"), {{"widget", widgetState(content)},
                {"event_type", int(event->type())}, {"page_visible", originalPage && originalPage->isVisible()}});
            queueRecovery();
        }
        if (object == originalPage && host) {
            if (event->type() == QEvent::Hide) {
                logEvent(QStringLiteral("panel.page.leave"), {{"host", host->objectName()}});
                host->hide();
                if (grouped()) groupContainer->hide();
            }
            if (event->type() == QEvent::Show) {
                logEvent(QStringLiteral("panel.page.return"), {{"host", host->objectName()}});
                queueRecovery();
            }
        }
        return false;
    }
private:
    bool nativeOwner(QWidget *parent) const
    {
        return parent && originalMain && parent->window() == originalMain;
    }
    void queueRecovery()
    {
        if (recoveryQueued || docking || !host || expandedHost) return;
        recoveryQueued = true;
        const unsigned token = generation;
        QTimer::singleShot(0, this, [this, token] {
            if (token != generation) return;
            recoveryQueued = false;
            if (!host || !originalPage || !originalPage->isVisible() || docking) return;
            // Cinema Viewer temporarily borrows/hides native player content.
            // Reclaiming it during that command interrupts Resolve's presentation.
            if (visibleCinemaViewer()) {
                QTimer::singleShot(100, this, [this, token] { if (token == generation) queueRecovery(); });
                return;
            }
            if (pendingDock) {
                const QString error = dock();
                logEvent(QStringLiteral("panel.dock.deferred-result"), {{"success", error.isEmpty()}, {"error", error}});
                return;
            }
            QScopedValueRollback<bool> recovery(recovering, true);
            QWidget *surface = window();
            if (!transferredLayout && content) {
                if (content->parentWidget() != surface) {
                    if (!nativeOwner(content->parentWidget())) {
                        logEvent(QStringLiteral("panel.recover.rejected"), {{"widget", widgetState(content)}}); return;
                    }
                    surface->layout()->addWidget(content);
                }
                content->show();
            }
            if (!userHidden && !surface->isVisible()) {
                const bool previous = surface->testAttribute(Qt::WA_ShowWithoutActivating);
                surface->setAttribute(Qt::WA_ShowWithoutActivating, true);
                surface->show(); surface->setAttribute(Qt::WA_ShowWithoutActivating, previous);
            }
            logEvent(QStringLiteral("panel.recover.complete"), {{"widget", widgetState(content)}, {"host", widgetState(host)}});
        });
    }
    void clear()
    {
        if (releaseGroup) { auto release = releaseGroup; release(); }
        groupWidth.restore(); widthAllowance.restore(); relaxedWidth = false;
        ++generation;
        if (content) content->removeEventFilter(this);
        host = nullptr; content = nullptr; placeholder = nullptr;
        groupContainer = nullptr;
        originalParent = nullptr; originalLayout = nullptr;
        originalSplitter = nullptr; originalPage = nullptr;
        originalStack = nullptr;
        originalMain = nullptr;
        splitterSizes.clear();
        transferredLayout = false;
        placeholderLayout = nullptr;
        userHidden = false;
        arrangeTimer.stop();
        recoveryQueued = false; pendingDock = false;
    }
    QPointer<QWidget> host, content, placeholder, originalParent, originalPage;
    PanelWidthAllowance widthAllowance, groupWidth;
    bool relaxedWidth = false;
    QPointer<QWidget> originalMain;
    QPointer<QLayout> originalLayout;
    QPointer<QSplitter> originalSplitter;
    QPointer<QStackedWidget> originalStack;
    QPointer<QLayout> placeholderLayout;
    bool transferredLayout = false;
    bool userHidden = false;
    bool constrain = true;
    bool snapping = true;
    QPointer<QWidget> groupContainer, expandedHost;
    QRect detachedGeometry;
    QTimer arrangeTimer;
    WindowChrome chrome = WindowChrome::Standard;
    int stackIndex = -1;
    QMetaObject::Connection parentDestroyed;
    QList<int> splitterSizes;
    QRect geometry;
    Qt::WindowFlags flags;
    bool hidden = false;
    bool docking = false, recovering = false, recoveryQueued = false, pendingDock = false;
    unsigned generation = 0;
};
}
