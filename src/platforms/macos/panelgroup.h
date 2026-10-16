// SPDX-License-Identifier: MIT
#pragma once
#include <QJsonArray>
#include <QSplitter>
#include <QPushButton>
#include "panelsession.h"
#include "workspacesettings.h"

namespace Unbound {
class PanelGroup : public QObject
{
public:
    explicit PanelGroup(QObject *owner) : QObject(owner)
    {
        timer.setSingleShot(true); timer.setInterval(180);
        connect(&timer, &QTimer::timeout, this, [this] {
            if (!host) return;
            logEvent(QStringLiteral("group.arrangement"), snapshot());
            if (changed) changed();
        });
    }
    ~PanelGroup() override { qApp->removeEventFilter(this); changed = {}; dissolve(); }
    bool active() const { return host; }
    QWidget *window() const { return host; }
    bool contains(MacPanelSession *session) const { return host && (first == session || second == session); }
    void setChrome(WindowChrome value) { if (host) host->setChrome(value); }
    void setSnapping(bool value) { if (host) host->setSnapping(value); }
    void fitFrame(const QRect &frame)
    {
        if (!host) return;
        const auto offset = host->geometry().topLeft() - host->frameGeometry().topLeft();
        const auto decoration = host->frameGeometry().size() - host->size();
        host->setGeometry(QRect(frame.topLeft() + offset, frame.size() - decoration));
        keepOnDisplay(host);
    }
    std::function<void()> changed;
    QString join(MacPanelSession *a, MacPanelSession *b, Qt::Orientation orientation)
    {
        if (host || !a || !b || a == b || !a->active() || !b->active() || a->grouped() || b->grouped() ||
            a->pageWidget() != b->pageWidget() || a->nativeOwnerWindow() != b->nativeOwnerWindow() ||
            !a->pageWidget() || !a->pageWidget()->isVisible()) return QStringLiteral("Choose two available, ungrouped floating panels on Edit.");
        first = a; second = b; page = a->pageWidget();
        names = QStringList{a->window()->objectName().mid(16), b->window()->objectName().mid(16)};
        host = new MacFloatingHost(a->nativeOwnerWindow());
        host->setObjectName(QStringLiteral("unboundFloating_group_") + names.join('_'));
        host->setProperty("unboundGroupTier", qMax(tier(names[0]), tier(names[1])));
        host->setWindowTitle(a->window()->windowTitle() + QStringLiteral(" + ") + b->window()->windowTitle());
        host->setAttribute(Qt::WA_QuitOnClose, false);
        auto layout = new QVBoxLayout(host); layout->setContentsMargins(0, 0, 0, 0);
        split = new QSplitter(orientation, host);
        split->setChildrenCollapsible(false);
        layout->addWidget(split);
        const auto left = a->floatingGeometry(), right = b->floatingGeometry();
        const auto cell = [this](MacPanelSession *session) {
            auto wrapper = new QWidget(split);
            auto layout = new QVBoxLayout(wrapper); layout->setContentsMargins(0, 0, 0, 0); layout->setSpacing(0);
            auto bar = new QWidget(wrapper); auto row = new QHBoxLayout(bar);
            row->setContentsMargins(6, 2, 4, 2);
            auto title = new QLabel(session->window()->windowTitle(), bar);
            title->setObjectName(QStringLiteral("unboundGroupHeader"));
            title->setToolTip(QStringLiteral("Drag to detach this panel"));
            title->setProperty("groupMember", session == first ? 0 : 1);
            title->installEventFilter(this);
            auto detach = new QPushButton(QStringLiteral("Detach"), bar);
            connect(detach, &QPushButton::clicked, this, [this, session] { detachMember(session); });
            row->addWidget(title, 1); row->addWidget(detach);
            auto content = new QWidget(wrapper);
            layout->addWidget(bar); layout->addWidget(content, 1);
            return content;
        };
        auto firstCell = cell(a), secondCell = cell(b);
        firstCell->setObjectName(QStringLiteral("unboundGroupCell_") + names[0]);
        secondCell->setObjectName(QStringLiteral("unboundGroupCell_") + names[1]);
        QString error = a->attachToGroup(firstCell);
        if (error.isEmpty()) error = b->attachToGroup(secondCell);
        if (!error.isEmpty()) { dissolve(); return error; }
        a->releaseGroup = b->releaseGroup = [this] { dissolve(); };
        host->setGeometry(left.topLeft().x(), left.topLeft().y(),
            orientation == Qt::Horizontal ? left.width() + right.width() + split->handleWidth() : qMax(left.width(), right.width()),
            orientation == Qt::Vertical ? left.height() + right.height() + split->handleWidth() : qMax(left.height(), right.height()));
        host->setChrome(WindowChrome::Standard);
        host->installEventFilter(this); page->installEventFilter(this);
        a->window()->installEventFilter(this); b->window()->installEventFilter(this);
        connect(split, &QSplitter::splitterMoved, this, [this] { timer.start(); });
        for (auto session : {a, b}) connect(session->window(), &QObject::destroyed, this, [this] { dissolve(); });
        refresh(); keepOnDisplay(host);
        logEvent(QStringLiteral("group.create"), snapshot());
        if (changed) changed();
        return {};
    }
    void dissolve(bool dockMembers = false)
    {
        if (releasing || (!host && !first && !second)) return;
        QScopedValueRollback<bool> guard(releasing, true);
        timer.stop();
        auto a = first, b = second;
        if (page) page->removeEventFilter(this);
        for (auto session : {a, b}) if (session) {
            session->releaseGroup = {};
            if (session->window()) session->window()->removeEventFilter(this);
            session->detachFromGroup();
        }
        if (host) { host->removeEventFilter(this); host->hide(); host->deleteLater(); }
        host = nullptr; split = nullptr; first = nullptr; second = nullptr;
        if (dockMembers) for (auto session : {a, b}) if (session && session->active()) {
            const auto error = session->dock();
            if (!error.isEmpty()) logEvent(QStringLiteral("group.dock.failed"), {{"error", error}});
        }
        logEvent(QStringLiteral("group.release"), {{"panels", names.join(",")}, {"dock_members", dockMembers}});
        if (changed) changed();
    }
    QJsonObject snapshot() const
    {
        if (!host || !split) return {};
        QJsonArray sizes;
        for (int size : split->sizes()) sizes.append(size);
        auto value = WorkspaceSettings::rectangle(host->geometry());
        value["panels"] = QJsonArray::fromStringList(names);
        value["orientation"] = split->orientation() == Qt::Horizontal ? "horizontal" : "vertical";
        value["sizes"] = sizes;
        return value;
    }
    void restore(const QJsonObject &value)
    {
        if (!host || !split) return;
        const auto position = WorkspaceSettings::rectangle(value);
        if (position.isValid()) host->setGeometry(position);
        QList<int> sizes;
        for (auto size : value["sizes"].toArray()) sizes.append(qMax(1, size.toInt()));
        if (sizes.size() == 2) split->setSizes(sizes);
        keepOnDisplay(host);
    }
    void detachMember(MacPanelSession *member)
    {
        if (!contains(member)) return;
        const auto a = first, b = second;
        const QRect firstRect(a->window()->parentWidget()->mapToGlobal(QPoint()), a->window()->parentWidget()->size());
        const QRect secondRect(b->window()->parentWidget()->mapToGlobal(QPoint()), b->window()->parentWidget()->size());
        dissolve();
        place(a, firstRect); place(b, secondRect);
        logEvent(QStringLiteral("group.detach"), {{"panel", member->window()->objectName()}});
    }
protected:
    bool eventFilter(QObject *object, QEvent *event) override
    {
        if (dragHost && (event->type() == QEvent::MouseMove || event->type() == QEvent::MouseButtonRelease)) {
            auto mouse = static_cast<QMouseEvent *>(event);
            dragHost->move(mouse->globalPos() - dragOffset);
            if (event->type() == QEvent::MouseButtonRelease) {
                dragHost->releaseMouse(); dragHost = nullptr; qApp->removeEventFilter(this);
            }
            return true;
        }
        if (!host || releasing) return false;
        auto header = qobject_cast<QLabel *>(object);
        if (header && header->objectName() == QLatin1String("unboundGroupHeader")) {
            if (event->type() == QEvent::MouseButtonPress) {
                auto mouse = static_cast<QMouseEvent *>(event);
                if (mouse->button() != Qt::LeftButton) return false;
                press = mouse->globalPos();
                headerPressed = true;
                headerOffset = press - header->parentWidget()->parentWidget()->mapToGlobal(QPoint());
                return true;
            }
            if (event->type() == QEvent::MouseMove) {
                auto mouse = static_cast<QMouseEvent *>(event);
                if (!headerPressed || !mouse->buttons().testFlag(Qt::LeftButton) || (mouse->globalPos() - press).manhattanLength() < QApplication::startDragDistance()) return false;
                headerPressed = false;
                auto member = header->property("groupMember").toInt() == 0 ? first : second;
                detachMember(member);
                if (!member || !member->window()) return true;
                dragHost = member->window();
                // QWidget::move uses the outer frame position for top-level windows.
                dragOffset = headerOffset;
                dragHost->move(mouse->globalPos() - dragOffset);
                dragHost->raise(); dragHost->activateWindow(); dragHost->grabMouse();
                qApp->installEventFilter(this);
                return true;
            }
            if (event->type() == QEvent::MouseButtonRelease) headerPressed = false;
        }
        if (object == host && event->type() == QEvent::Close) {
            static_cast<QCloseEvent *>(event)->accept(); dissolve(true); return true;
        }
        if (object == host && (event->type() == QEvent::Move || event->type() == QEvent::Resize)) timer.start();
        if (event->type() == QEvent::Show || event->type() == QEvent::Hide) {
            if (object == page && event->type() == QEvent::Hide) host->hide();
            QTimer::singleShot(0, this, [this] { refresh(); });
        }
        return false;
    }
private:
    static void place(MacPanelSession *session, const QRect &frame)
    {
        if (!session || !session->window()) return;
        auto window = session->window();
        const auto offset = window->geometry().topLeft() - window->frameGeometry().topLeft();
        const auto decoration = window->frameGeometry().size() - window->size();
        session->setFloatingGeometry(QRect(frame.topLeft() + offset, frame.size() - decoration));
    }
    static int tier(const QString &name)
    { return name.contains("Player") ? 2 : name == "frameConformTimelines" ? 0 : 1; }
    void refresh()
    {
        if (!host || releasing) return;
        if (!first || !second || !first->active() || !second->active()) { dissolve(); return; }
        const bool visible = page && page->isVisible() && (!first->hiddenByUser() || !second->hiddenByUser());
        if (host->isVisible() != visible) {
            host->setAttribute(Qt::WA_ShowWithoutActivating); host->setVisible(visible);
        }
    }
    QPointer<MacFloatingHost> host;
    QPointer<QSplitter> split;
    QPointer<MacPanelSession> first, second;
    QPointer<QWidget> page;
    QStringList names;
    QTimer timer;
    bool releasing = false;
    QPointer<QWidget> dragHost;
    QPoint press, headerOffset, dragOffset;
    bool headerPressed = false;
};
}
