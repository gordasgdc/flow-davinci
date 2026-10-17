// SPDX-License-Identifier: MIT
#pragma once
#include <QApplication>
#include <QMenu>
#include <QPointer>
#include <QTimer>
#include <functional>
namespace Unbound {
// Populate native menus before they are mirrored by Cocoa, not just on QWidget Show.
class WorkspaceCallers : public QObject
{
public:
    struct Command { QString id, label; std::function<bool()> visible; std::function<void()> toggle; };
    explicit WorkspaceCallers(QObject *owner) : QObject(owner) {}
    void add(Command command) { commands.append(std::move(command)); }
    void wire()
    {
        for (auto widget : QApplication::allWidgets()) if (auto menu = qobject_cast<QMenu *>(widget))
            if (menu->objectName() == QLatin1String("workspaceShowPanel")) refresh(menu);
    }
private:
    void refresh(QMenu *menu)
    {
        for (int i = 0; i < commands.size(); ++i) {
            const auto &command = commands[i]; QAction *entry = nullptr;
            for (auto action : menu->actions()) if (action->objectName() == command.id) entry = action;
            if (!entry) {
                entry = menu->addAction(command.label); entry->setObjectName(command.id); entry->setCheckable(true);
                if (!menu->property("unboundCallersWired").toBool()) {
                    menu->setProperty("unboundCallersWired", true);
                    connect(menu, &QMenu::aboutToShow, this, [this, menu] { refresh(menu); });
                }
                connect(entry, &QAction::triggered, this, [this, i] {
                    QTimer::singleShot(0, this, [this, i] { commands[i].toggle(); });
                });
            }
            entry->setChecked(command.visible());
        }
    }
    QList<Command> commands;
};
}
