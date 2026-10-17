// SPDX-License-Identifier: MIT
#pragma once
#include <QMenu>
#include "panelsession.h"
#include "workspacesettings.h"
namespace Unbound {
// Global navigation belongs to the main window, not to a particular Resolve page.
class FooterPanel : public QObject
{
public:
    FooterPanel(QObject *owner, std::function<QWidget *()> mainWindow, QJsonObject saved)
        : QObject(owner), root(std::move(mainWindow)), state(std::move(saved)), session(this)
    {
        qApp->installEventFilter(this);
        QTimer::singleShot(0, this, [this] { wireMenu(); });
        session.changed = [this] { if (changed) changed(); };
        session.arrangementChanged = [this] { if (changed) changed(); };
    }
    ~FooterPanel() override { qApp->removeEventFilter(this); session.changed = {}; session.arrangementChanged = {}; }
    std::function<void()> changed;
    QString floatPanel()
    {
        if (session.active()) { session.setHiddenByUser(false); return {}; }
        auto main = root();
        auto footer = main ? main->findChild<QWidget *>("m_PageSwitcherContainer") : nullptr;
        if (!footer) return QStringLiteral("Native page navigation is not available yet.");
        const QPoint position = footer->mapToGlobal(QPoint());
        footer->show();
        const auto error = session.undock(footer, main, QStringLiteral("Page Navigation"));
        if (!error.isEmpty()) return error;
        const auto saved = WorkspaceSettings::rectangle(state);
        session.setFloatingGeometry(saved.isValid() ? saved : QRect(position, QSize(900,100)));
        state["floating"] = true;
        logEvent(QStringLiteral("footer.undock"));
        if (changed) changed();
        return {};
    }
    void dock()
    {
        state = snapshot(); state["floating"] = false;
        session.dock();
        if (changed) changed();
    }
    void restore()
    {
        wireMenu();
        if (!state["floating"].toBool() || session.active()) return;
        const bool hidden = state["hidden"].toBool();
        if (floatPanel().isEmpty()) session.setHiddenByUser(hidden);
    }
    void setState(const QJsonObject &value) { state = value; restore(); }
    QJsonObject snapshot() const
    {
        auto value = state;
        if (session.active()) {
            value = WorkspaceSettings::rectangle(session.floatingGeometry());
            value["floating"] = true; value["hidden"] = session.hiddenByUser();
        }
        return value;
    }
    void wireMenu()
    {
        for (auto widget : QApplication::allWidgets()) if (auto menu = qobject_cast<QMenu *>(widget))
            if (menu->objectName() == QLatin1String("workspaceShowPanel")) refreshMenu(menu);
    }
protected:
    bool eventFilter(QObject *object, QEvent *event) override
    {
        auto menu = qobject_cast<QMenu *>(object);
        if (menu && event->type() == QEvent::Show && menu->objectName() == QLatin1String("workspaceShowPanel")) refreshMenu(menu);
        return false;
    }
private:
    void refreshMenu(QMenu *menu)
    {
        QAction *entry = nullptr;
        for (auto action : menu->actions()) if (action->objectName() == QLatin1String("unboundPageNavigation")) entry = action;
        if (!entry) {
            entry = menu->addAction(QStringLiteral("Page Navigation (Footer)"));
            entry->setObjectName(QStringLiteral("unboundPageNavigation")); entry->setCheckable(true);
            connect(menu, &QMenu::aboutToShow, this, [this, menu] { refreshMenu(menu); });
            connect(entry, &QAction::triggered, this, [this] {
                QTimer::singleShot(0, this, [this] {
                    if (session.active()) session.setHiddenByUser(!session.hiddenByUser());
                    else {
                        const auto error = floatPanel();
                        logEvent(QStringLiteral("footer.open.result"), {{"success", error.isEmpty()}, {"error", error}});
                    }
                });
            });
        }
        entry->setChecked(session.active() && !session.hiddenByUser());
    }
private:
    std::function<QWidget *()> root;
    QJsonObject state;
    MacPanelSession session;
};
}
