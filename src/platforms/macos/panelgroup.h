// SPDX-License-Identifier: MIT
#pragma once
#include <QHash>
#include <QPushButton>
#include <QTabWidget>
#include "grouplayout.h"
#include "panelsession.h"
#include "workspacesettings.h"
#include "tabtearoff.h"
#include "groupvisibility.h"
#include "docksurfaces.h"
#include "uiworktiming.h"
#include "grouphostreserve.h"

namespace Unbound {
class PanelGroup : public QObject
{
public:
    explicit PanelGroup(QObject *owner, GroupHostReserve *reserve = nullptr) : QObject(owner), reserve(reserve)
    {
        timer.setSingleShot(true); timer.setInterval(180);
        connect(&timer, &QTimer::timeout, this, [this] { if (host) report("group.arrangement"); });
    }
    ~PanelGroup() override { changed = {}; dissolve(); }
    bool active() const { return host; }
    QWidget *window() const { return host; }
    bool contains(MacPanelSession *session) const { return host && members.contains(session); }
    void setChrome(WindowChrome value) { if (host) host->setChrome(value); }
    void setSnapping(bool value) { if (host) host->setSnapping(value); }
    void fitFrame(const QRect &frame)
    {
        if (!host) return;
        UiWorkTiming timing("group.fit-frame",host);
        host->setGeometry(QRect(frame.topLeft() + host->geometry().topLeft() - host->frameGeometry().topLeft(),
            frame.size() - (host->frameGeometry().size() - host->size()))); keepOnDisplay(host);
    }
    std::function<void()> changed;
    std::function<void(QWidget *, Qt::Edge)> attachRequested;
    QString join(MacPanelSession *a, MacPanelSession *b, Qt::Orientation orientation, const QRect &destination = {})
    {
        if (!a || !b || !a->active() || !b->active()) return "Show both panels first.";
        const auto left = a->floatingGeometry(), right = b->floatingGeometry();
        return initialize({a, b}, splitGroupTree({{"panel", panelName(a)}}, {{"panel", panelName(b)}}, orientation),
            destination.isValid() ? destination : QRect(left.topLeft(), orientation == Qt::Horizontal ? QSize(left.width() + right.width() + 4, qMax(left.height(), right.height())) :
                QSize(qMax(left.width(), right.width()), left.height() + right.height() + 4)));
    }
    QString initialize(const QList<MacPanelSession *> &panels, const QJsonObject &tree, const QRect &position)
    {
        UiWorkTiming timing("group.initialize",nullptr);
        QStringList expected, actual;
        if (host || panels.size() < 2 || !groupTreeNames(tree, expected) || expected.size() != panels.size()) return "Invalid group layout.";
        auto a = panels.first();
        if (!a || !a->pageWidget() || !a->pageWidget()->isVisible()) return "Open Edit before grouping panels.";
        for (auto panel : panels) {
            if (!panel || !panel->active() || panel->grouped() || panel->pageWidget() != a->pageWidget() ||
                panel->nativeOwnerWindow() != a->nativeOwnerWindow() || actual.contains(panelName(panel))) return "Choose available panels from the same Edit page.";
            actual.append(panelName(panel));
        }
        expected.sort(); actual.sort(); if (expected != actual) return "Saved group contains different panels.";
        page = a->pageWidget(); host = reserve ? reserve->take(a->nativeOwnerWindow()) : new MacFloatingHost(a->nativeOwnerWindow());
        host->setObjectName(QStringLiteral("unboundFloating_group_") + actual.join('_'));
        host->setAttribute(Qt::WA_QuitOnClose, false);
        auto layout = new QVBoxLayout(host); layout->setContentsMargins(0, 0, 0, 0); host->setGeometry(position);
        for (auto panel : panels) { const auto error = addMember(panel); if (!error.isEmpty()) { dissolve(); return error; } }
        rebuild(tree); host->setChrome(WindowChrome::Standard);
        host->attachRequested = [this](QWidget *target, Qt::Edge edge) { if (attachRequested) attachRequested(target, edge); };
        host->installEventFilter(this); page->installEventFilter(this);
        refresh();
        for (auto panel : panels) panel->finishGroupTransfer();
        keepOnDisplay(host); report("group.create"); return {};
    }
    QString add(MacPanelSession *panel, Qt::Orientation orientation, bool before = false, const QString &target = {})
    {
        if (!host || !panel || !panel->active() || panel->grouped() || panel->pageWidget() != page ||
            panel->nativeOwnerWindow() != host->parentWidget()) return "Panel cannot join this group.";
        if (!target.isEmpty() && !names().contains(target)) return "Dock target no longer exists.";
        const auto tree = insertGroupTree(treeSnapshot(root), {{"panel", panelName(panel)}}, target, orientation, before);
        const auto error = addMember(panel); if (!error.isEmpty()) return error;
        rebuild(tree); refresh(); panel->finishGroupTransfer(); report("group.add"); return {};
    }
    QString merge(PanelGroup *other, Qt::Orientation orientation, bool before = false, const QString &target = {})
    {
        if (!host || !other || !other->host || other == this || other->page != page ||
            host->parentWidget() != other->host->parentWidget()) return "These groups cannot be merged.";
        if (!target.isEmpty() && !names().contains(target)) return "Dock target no longer exists.";
        const auto tree = insertGroupTree(treeSnapshot(root), other->treeSnapshot(other->root), target, orientation, before);
        QScopedValueRollback<bool> moving(other->releasing, true);
        for (auto member : other->members) if (member) {
            auto wrapper = other->leaves.value(member);
            member->window()->removeEventFilter(other); QObject::disconnect(member->window(), nullptr, other, nullptr);
            for (auto label : wrapper->findChildren<QLabel *>("unboundGroupHeader")) label->removeEventFilter(other);
            for (auto button : wrapper->findChildren<QPushButton *>("unboundDetachPanel")) QObject::disconnect(button, nullptr, other, nullptr);
            wrapper->setParent(host); members.append(member); leaves[member] = wrapper; wireMember(member);
        }
        other->members.clear(); other->leaves.clear(); other->retireHost();
        rebuild(tree); refresh(); if (other->changed) other->changed(); report("group.merge"); return {};
    }
    // Transfer a tab containing a split subtree without flattening its panels.
    PanelGroup *detachSection(QWidget *section)
    {
        if (!host || !section || !root || section == root || !root->isAncestorOf(section)) return nullptr;
        const auto tree = treeSnapshot(section); QStringList selectedNames;
        if (!groupTreeNames(tree, selectedNames) || selectedNames.size() < 2) return nullptr;
        const auto originalTree = treeSnapshot(root); const auto originalFrame = host->geometry();
        const QRect frame(section->mapToGlobal(QPoint()), section->size());
        QList<MacPanelSession *> selected, all;
        for (auto candidate : members) if (candidate) {
            all.append(candidate);
            if (selectedNames.contains(panelName(candidate))) selected.append(candidate);
        }
        if (selected.size() != selectedNames.size() || selected.size() == all.size()) return nullptr;
        for (auto candidate : selected) if (contains(candidate)) detachMember(candidate);
        auto detached = new PanelGroup(parent(),reserve);
        const auto error = detached->initialize(selected, tree, frame);
        if (!error.isEmpty()) {
            delete detached; dissolve();
            const auto rollback = initialize(all, originalTree, originalFrame);
            logEvent(QStringLiteral("group.section.rollback"), {{"error", error}, {"rollback_error", rollback}});
            return nullptr;
        }
        if (sectionDetached) sectionDetached(detached);
        logEvent(QStringLiteral("group.section.detach"), {{"panels", QJsonArray::fromStringList(selectedNames)}});
        return detached;
    }
    std::function<void(PanelGroup *)> sectionDetached;
    void dissolve(bool dockMembers = false)
    {
        if (releasing || (!host && members.isEmpty())) return;
        QScopedValueRollback<bool> guard(releasing, true);
        QList<MacPanelSession *> current;
        for (auto member : members) if (member) {
            current.append(member); member->releaseGroup = {};
            if (member->window()) member->window()->removeEventFilter(this);
            member->detachFromGroup();
        }
        members.clear(); leaves.clear(); retireHost();
        if (dockMembers) for (auto member : current) if (member->active()) {
            const auto error = member->dock(); if (!error.isEmpty()) logEvent(QStringLiteral("group.dock.failed"), {{"error", error}});
        }
        logEvent(QStringLiteral("group.release"), {{"dock_members", dockMembers}}); if (changed) changed();
    }
    QJsonObject snapshot() const
    {
        if (!host) return {};
        auto value = WorkspaceSettings::rectangle(host->geometry()); const auto tree = treeSnapshot(root);
        value["pinned"] = host->pinned();
        value["tree"] = tree; value["panels"] = QJsonArray::fromStringList(names());
        value["orientation"] = tree["orientation"]; value["sizes"] = tree["sizes"]; return value;
    }
    static QJsonObject savedTree(const QJsonObject &value)
    {
        if (value["tree"].isObject()) return value["tree"].toObject();
        const auto panels = value["panels"].toArray(); if (panels.size() != 2) return {};
        auto tree = splitGroupTree({{"panel", panels[0]}}, {{"panel", panels[1]}}, value["orientation"] == "vertical" ? Qt::Vertical : Qt::Horizontal);
        tree["sizes"] = value["sizes"]; return tree;
    }
    bool restore(const QJsonObject &value)
    {
        if (!host) return false;
        const auto tree = savedTree(value); QStringList expected;
        if (!groupTreeNames(tree, expected)) return false;
        auto current = names(); current.sort(); expected.sort(); if (current != expected) return false;
        const auto position = WorkspaceSettings::rectangle(value); if (position.isValid()) host->setGeometry(position);
        host->setPinned(value["pinned"].toBool());
        rebuild(tree); refresh(); keepOnDisplay(host); return true;
    }
    bool selected(MacPanelSession *member) const
    {
        auto wrapper = leaves.value(member);
        if (!wrapper || member->hiddenByUser()) return false;
        for (auto parent = wrapper->parentWidget(); parent && parent != host; parent = parent->parentWidget())
            if (auto tabs = qobject_cast<QTabWidget *>(parent))
                if (!tabs->currentWidget() || (tabs->currentWidget() != wrapper && !tabs->currentWidget()->isAncestorOf(wrapper))) return false;
        return true;
    }
    void syncVisibility(MacPanelSession *select = nullptr)
    {
        if (!host || rebuilding || releasing || syncingVisibility) return;
        UiWorkTiming timing("group.visibility",host);
        QScopedValueRollback<bool> guard(syncingVisibility, true);
        if (root) visibility.remember(root);
        for (auto member : members) if (member) {
            auto wrapper = leaves.value(member); if (!wrapper) continue;
            const bool visible = !member->hiddenByUser();
            for (auto tabs : host->findChildren<QTabWidget *>()) for (int i = 0; i < tabs->count(); ++i)
                if (tabs->widget(i) == wrapper) visibility.tabVisibility(tabs, wrapper, visible, select == member);
            if (!wrapper->parentWidget() || !qobject_cast<QStackedWidget *>(wrapper->parentWidget())) wrapper->setVisible(visible);
            member->window()->setVisible(visible);
        }
        if (host->layout()) host->layout()->activate();
        if (root) visibility.restore(root);
        refresh();
    }
    void detachMember(MacPanelSession *member)
    {
        if (!contains(member) || releasing) return;
        const QRect groupFrame = host->frameGeometry(); auto wrapper = leaves.value(member);
        if (!wrapper || !member->window()) { dissolve(); return; }
        QScopedValueRollback<bool> guard(releasing, true);
        const QRect detachedFrame(wrapper->mapToGlobal(QPoint()), wrapper->size());
        const auto tree = removeGroupLeaf(treeSnapshot(root), panelName(member));
        member->releaseGroup = {}; member->window()->removeEventFilter(this);
        QObject::disconnect(member->window(), nullptr, this, nullptr);
        member->detachFromGroup(); members.removeAll(member); leaves.remove(member); wrapper->hide(); wrapper->deleteLater();
        if (members.size() == 1) {
            auto remaining = members.first(); remaining->releaseGroup = {}; remaining->window()->removeEventFilter(this);
            remaining->detachFromGroup(); place(remaining, groupFrame); members.clear(); leaves.clear(); retireHost();
        } else if (members.isEmpty()) retireHost(); else rebuild(tree);
        place(member, detachedFrame);
        logEvent(QStringLiteral("group.detach"), {{"panel", member->window()->objectName()}, {"remaining", members.size()}});
        if (changed) changed();
    }
protected:
    bool eventFilter(QObject *object, QEvent *event) override
    {
        if (!host || releasing || rebuilding) return false;
        auto header = qobject_cast<QLabel *>(object);
        if (header && header->objectName() == QLatin1String("unboundGroupHeader")) {
            if (event->type() == QEvent::MouseButtonPress) {
                auto mouse = static_cast<QMouseEvent *>(event); if (mouse->button() != Qt::LeftButton) return false;
                press = mouse->globalPos(); headerPressed = true;
                headerOffset = press - header->parentWidget()->parentWidget()->mapToGlobal(QPoint()); return true;
            }
            if (event->type() == QEvent::MouseMove) {
                auto mouse = static_cast<QMouseEvent *>(event);
                if (!headerPressed || !mouse->buttons().testFlag(Qt::LeftButton) || (mouse->globalPos() - press).manhattanLength() < QApplication::startDragDistance()) return false;
                headerPressed = false; QPointer<MacPanelSession> member;
                for (auto candidate : members) if (candidate && panelName(candidate) == header->property("groupPanel").toString()) member = candidate;
                if (!member) return true;
                detachMember(member); auto dragHost = static_cast<MacFloatingHost *>(member->window());
                dragHost->move(mouse->globalPos() - headerOffset); dragHost->raise(); dragHost->activateWindow();
                dragHost->beginPointerDrag(mouse->globalPos(), true); return true;
            }
            if (event->type() == QEvent::MouseButtonRelease) headerPressed = false;
        }
        if (object == host && event->type() == QEvent::Close) { static_cast<QCloseEvent *>(event)->accept(); dissolve(true); return true; }
        if (object == host && (event->type() == QEvent::Move || event->type() == QEvent::Resize)) timer.start();
        if (event->type() == QEvent::Show || event->type() == QEvent::Hide) {
            if (object == page && event->type() == QEvent::Hide) host->hide();
            QTimer::singleShot(0, this, [this] { refresh(); });
        }
        return false;
    }
private:
    static QString panelName(MacPanelSession *session)
    { return session->window()->objectName().mid(session->grouped() ? QStringLiteral("unboundGroupCell_").size() : QStringLiteral("unboundFloating_").size()); }
    QStringList names() const { QStringList result; for (auto member : members) if (member) result.append(panelName(member)); return result; }
    QString addMember(MacPanelSession *member)
    {
        const auto name = panelName(member), title = member->window()->windowTitle();
        auto wrapper = new QWidget(host); wrapper->setProperty("groupPanel", name);
        auto layout = new QVBoxLayout(wrapper); layout->setContentsMargins(0, 0, 0, 0); layout->setSpacing(0);
        auto bar = new QWidget(wrapper); auto row = new QHBoxLayout(bar); row->setContentsMargins(6, 2, 4, 2);
        auto label = new QLabel(title, bar); label->setObjectName("unboundGroupHeader"); label->setProperty("groupPanel", name);
        label->setMinimumWidth(0); label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
        label->setProperty("groupMember", members.size()); label->setToolTip("Drag to detach this panel"); label->installEventFilter(this);
        auto detach = new QPushButton(QStringLiteral("Detach"), bar); detach->setObjectName(QStringLiteral("unboundDetachPanel"));
        auto hide = new QPushButton(QStringLiteral("Hide"), bar); hide->setObjectName("unboundHidePanel");
        for (auto button : {hide,detach}) {
            button->setFixedWidth(button->fontMetrics().horizontalAdvance(button->text()) + 16);
            button->setToolTip(button == hide ? "Hide this panel" : "Detach this panel into its own window");
        }
        hide->setToolTip("Hide this panel; reopen it from Workspace > Show Panel in Workspace");
        row->addWidget(label, 1); row->addWidget(hide); row->addWidget(detach);
        auto content = new QWidget(wrapper); content->setObjectName(QStringLiteral("unboundGroupCell_") + name); content->setWindowTitle(title);
        layout->addWidget(bar); layout->addWidget(content, 1);
        // Keep old hosts until the destination is ready. Hiding the active host
        // earlier makes Cocoa activate another window in the middle of transfer.
        const auto error = member->attachToGroup(content,true); if (!error.isEmpty()) { delete wrapper; return error; }
        members.append(member); leaves[member] = wrapper; wireMember(member); return {};
    }
    void wireMember(MacPanelSession *member)
    {
        member->releaseGroup = [this, member] { detachMember(member); }; member->window()->installEventFilter(this);
        const auto wrapper = leaves.value(member);
        for (auto label : wrapper->findChildren<QLabel *>("unboundGroupHeader")) label->installEventFilter(this);
        for (auto button : wrapper->findChildren<QPushButton *>("unboundDetachPanel")) {
            QObject::disconnect(button, nullptr, this, nullptr);
            connect(button, &QPushButton::clicked, this, [this, member] { detachMember(member); });
        }
        for (auto button : wrapper->findChildren<QPushButton *>("unboundHidePanel")) {
            QObject::disconnect(button, nullptr, nullptr, nullptr);
            connect(button, &QPushButton::clicked, this, [this, member] { member->setHiddenByUser(true); syncVisibility(); });
        }
        connect(member->window(), &QObject::destroyed, this, [this] { if (!releasing && !rebuilding) dissolve(); });
    }
    QJsonObject treeSnapshot(QWidget *widget) const
    {
        if (!widget) return {};
        if (auto tabs = qobject_cast<QTabWidget *>(widget)) {
            QJsonArray children;
            for (int i = 0; i < tabs->count(); ++i) children.append(treeSnapshot(tabs->widget(i)));
            return {{"orientation", "tabs"}, {"children", children}, {"selected", tabs->currentIndex()}};
        }
        auto splitter = qobject_cast<QSplitter *>(widget); if (!splitter) return {{"panel", widget->property("groupPanel").toString()}};
        QJsonArray children, sizes;
        for (int i = 0; i < splitter->count(); ++i) children.append(treeSnapshot(splitter->widget(i)));
        auto measured = splitter->sizes();
        const auto remembered = splitter->property("unboundVisibleSizes").value<QList<int>>();
        if (measured.contains(0) && remembered.size() == measured.size()) measured = remembered;
        for (int size : measured) sizes.append(size);
        return {{"orientation", splitter->orientation() == Qt::Horizontal ? "horizontal" : "vertical"}, {"children", children}, {"sizes", sizes}};
    }
    QWidget *buildTree(const QJsonObject &node)
    {
        if (node.contains("panel")) {
            for (auto member : members) if (member && panelName(member) == node["panel"].toString()) return leaves.value(member);
            return nullptr;
        }
        if (node["orientation"] == "tabs") {
            auto tabs = new QTabWidget(host); tabs->setObjectName("unboundPanelTabs");
            tabs->setDocumentMode(true); tabs->setMovable(false);
            for (auto child : node["children"].toArray()) if (auto widget = buildTree(child.toObject())) {
                QString title = child.toObject().value("panel").toString();
                for (auto member : members) if (member && panelName(member) == title) title = member->window()->windowTitle();
                tabs->addTab(widget, title.isEmpty() ? QStringLiteral("Panel group") : title);
            }
            new TabTearOff(tabs, tabs, [this](QWidget *leaf, QPoint pointer) {
                QPointer<MacPanelSession> member;
                for (auto candidate : members) if (leaves.value(candidate) == leaf) member = candidate;
                if (!member) {
                    if (auto section = detachSection(leaf)) {
                        auto floating = static_cast<MacFloatingHost *>(section->window());
                        floating->move(pointer - QPoint(80,12)); floating->raise(); floating->activateWindow();
                        floating->beginPointerDrag(pointer, true);
                    }
                    return;
                }
                detachMember(member);
                auto floating = static_cast<MacFloatingHost *>(member->window());
                floating->move(pointer - QPoint(80, 12)); floating->raise(); floating->activateWindow();
                floating->beginPointerDrag(pointer, true);
            });
            tabs->setCurrentIndex(qBound(0, node["selected"].toInt(), tabs->count() - 1));
            connect(tabs, &QTabWidget::currentChanged, this, [this] { if (!rebuilding) timer.start(); });
            return tabs;
        }
        auto splitter = new QSplitter(node["orientation"] == "vertical" ? Qt::Vertical : Qt::Horizontal, host); splitter->setChildrenCollapsible(false); splitter->setHandleWidth(6);
        for (auto child : node["children"].toArray()) if (auto widget = buildTree(child.toObject())) splitter->addWidget(widget);
        splitter->setProperty("savedSizes", node["sizes"].toArray().toVariantList());
        connect(splitter, &QSplitter::splitterMoved, this, [this] { timer.start(); }); return splitter;
    }
    void rebuild(const QJsonObject &tree)
    {
        UiWorkTiming timing("group.rebuild",host);
        QScopedValueRollback<bool> guard(rebuilding, true);
        visibility.reset();
        for (auto member : members) if (member) leaves.value(member)->setParent(host);
        if (root) { host->layout()->removeWidget(root); root->hide(); root->deleteLater(); }
        root = buildTree(tree);
        if (!root || !host->layout()) {
            logEvent(QStringLiteral("group.rebuild.failed"), {{"tree", tree}, {"panels", QJsonArray::fromStringList(names())}, {"root", !root.isNull()}, {"layout", host->layout() != nullptr}});
            return;
        }
        host->layout()->addWidget(root);
        for (auto member : members) if (member) {
            auto wrapper = leaves.value(member);
            // QStackedWidget owns tab-page visibility. Showing an inactive page
            // here makes it overlap the selected native panel and receive input.
            const bool visible = !member->hiddenByUser();
            if (auto stack = qobject_cast<QStackedWidget *>(wrapper->parentWidget())) {
                if (auto tabs = qobject_cast<QTabWidget *>(stack->parentWidget())) visibility.tabVisibility(tabs,wrapper,visible,false);
            } else wrapper->setVisible(visible);
            member->window()->setVisible(visible);
        }
        root->show(); host->layout()->activate();
        auto splitters = root->findChildren<QSplitter *>(); if (auto splitter = qobject_cast<QSplitter *>(root.data())) splitters.prepend(splitter);
        for (auto splitter : splitters) {
            QList<int> sizes; for (auto size : splitter->property("savedSizes").toList()) sizes.append(qMax(1, size.toInt()));
            if (sizes.size() == splitter->count()) splitter->setSizes(sizes);
        }
        QStringList titles; for (auto member : members) if (member && member->window()) titles.append(member->window()->windowTitle());
        host->setWindowTitle(titles.join(QStringLiteral(" + ")));
        QList<QWidget *> surfaces;
        for (auto member : members) if (member) surfaces.append(member->window());
        registerDockSurfaces(host,surfaces);
        // Visibility is established before the initial layout/show. Scheduling
        // another visibility/layout pass after presentation repeats that work.
    }
    void retireHost()
    {
        timer.stop(); if (page) page->removeEventFilter(this);
        if (host) { host->removeEventFilter(this); host->hide(); host->deleteLater(); }
        host = nullptr; root = nullptr;
    }
    void refresh()
    {
        if (!host || releasing || rebuilding) return;
        bool visible = false;
        for (auto member : members) { if (!member || !member->active()) { dissolve(); return; } visible |= !member->hiddenByUser(); }
        visible &= page && page->isVisible();
        if (host->isVisible() != visible) {
            UiWorkTiming timing("group.host.visibility",host);
            host->setAttribute(Qt::WA_ShowWithoutActivating); host->setVisible(visible);
        }
    }
    void report(const QString &event) { logEvent(event, snapshot()); if (changed) changed(); }
    static void place(MacPanelSession *session, const QRect &frame)
    {
        if (!session || !session->window()) return; auto window = session->window();
        session->setFloatingGeometry(QRect(frame.topLeft() + window->geometry().topLeft() - window->frameGeometry().topLeft(),
            frame.size() - (window->frameGeometry().size() - window->size())));
    }
    GroupVisibility visibility;
    QPointer<GroupHostReserve> reserve;
    QPointer<MacFloatingHost> host;
    QPointer<QWidget> root, page;
    QList<QPointer<MacPanelSession>> members;
    QHash<MacPanelSession *, QPointer<QWidget>> leaves;
    QTimer timer;
    bool releasing = false, rebuilding = false, headerPressed = false, syncingVisibility = false;
    QPoint press, headerOffset;
};
}
