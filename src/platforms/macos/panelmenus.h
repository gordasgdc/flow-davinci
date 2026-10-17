// SPDX-License-Identifier: MIT
#pragma once
#include <QMenu>
#include <QJsonArray>
#include <QPushButton>
#include <QToolButton>
#include <QTimer>
#include <QSet>
#include <functional>
#include "diagnostics.h"

namespace Unbound {
class PanelMenus : public QObject
{
public:
    struct Panel {
        QString name;
        std::function<bool()> floating;
        std::function<void()> toggle;
    };
    explicit PanelMenus(QObject *owner) : QObject(owner) { qApp->installEventFilter(this); }
    ~PanelMenus() override { qApp->removeEventFilter(this); }
    void add(Panel panel) { panels.append(std::move(panel)); }
    void wire()
    {
        for (auto widget : QApplication::allWidgets()) if (auto menu = qobject_cast<QMenu *>(widget)) {
            auto button = qobject_cast<QWidget *>(menu->parent());
            if (optionsButton(button)) prepareMenu(menu);
        }
    }
protected:
    bool eventFilter(QObject *object, QEvent *event) override
    {
        if (event->type() != QEvent::Show) return false;
        auto menu = qobject_cast<QMenu *>(object);
        if (!menu || menu->property("unboundOwnMenu").toBool()) return false;
        prepareMenu(menu);
        return false;
    }
private:
    void prepareMenu(QMenu *menu)
    {
        if (!wiredMenus.contains(menu)) {
            wiredMenus.insert(menu);
            connect(menu, &QObject::destroyed, this, [this, menu] { wiredMenus.remove(menu); });
            connect(menu, &QMenu::aboutToShow, this, [this, menu] { prepareMenu(menu); });
        }
        const int index = ownerOf(menu);
        if (index < 0) {
            for (auto command : menu->actions()) if (command->objectName() == QLatin1String("unboundDockPanel")) command->setVisible(false);
            QJsonArray parents;
            for (QObject *parent = menu->parent(); parent && parents.size() < 8; parent = parent->parent())
                parents.append(QJsonObject{{"name", parent->objectName()}, {"class", parent->metaObject()->className()}});
            logEvent(QStringLiteral("panel.menu.unmatched"), {{"name", menu->objectName()}, {"parents", parents}});
            return;
        }
        auto &panel = panels[index];
        QAction *command = nullptr;
        for (auto action : menu->actions()) if (action->objectName() == QLatin1String("unboundDockPanel")) command = action;
        if (!command) {
            menu->addSeparator();
            command = menu->addAction(QString());
            command->setObjectName(QStringLiteral("unboundDockPanel"));
        }
        if (!wired.contains(command)) {
            wired.insert(command);
            connect(command, &QObject::destroyed, this, [this, command] { wired.remove(command); });
            connect(command, &QAction::triggered, this, [this, command] {
                const int index = command->property("unboundPanelIndex").toInt();
                // Let Resolve finish closing its menu before changing widget ownership.
                QTimer::singleShot(0, this, [this, index] {
                    if (index < 0 || index >= panels.size()) return;
                    logEvent(QStringLiteral("panel.menu.toggle"), {{"panel", panels[index].name}});
                    panels[index].toggle();
                });
            });
        }
        command->setProperty("unboundPanelIndex", index);
        command->setVisible(true);
        command->setText(panel.floating() ? QStringLiteral("Dock Panel") : QStringLiteral("Undock Panel"));
        logEvent(QStringLiteral("panel.menu.ready"), {{"panel", panel.name}, {"floating", panel.floating()}});
    }
    static bool optionsButton(QWidget *button)
    {
        if (!button) return false;
        const QString name = button->objectName();
        if (name == "m_BtnMenu") return button->parentWidget() && button->parentWidget()->objectName() == "frameHeaderTitle";
        return name == "btnMediaPoolOptions" || name == "btnTitleOptions" || name == "btnAudioMixerOptions" ||
            name == "btnOptions" || name == "m_BtnDisplayOptions" || name == "btnEdlOptions" ||
            name == "tbtnPlayerOptions" || name == "metadataSelectionMenuBtn" || name == "btnCurveSettingsOptions" ||
            name == "btnEditToolbarPreferences";
    }
    int closestPanel(QWidget *button) const
    {
        for (auto parent = button->parentWidget(); parent; parent = parent->parentWidget())
            for (int i = 0; i < panels.size(); ++i) {
                // Timeline display preferences describe the timeline, not its detachable tools row.
                if (button->objectName() == "btnEditToolbarPreferences" && panels[i].name == "frameEditToolbarTop") continue;
                if (parent->objectName() == panels[i].name ||
                    parent->objectName() == QStringLiteral("unboundFloating_") + panels[i].name ||
                    parent->objectName() == QStringLiteral("unboundGroupCell_") + panels[i].name) return i;
            }
        return -1;
    }
    int ownerOf(QMenu *menu) const
    {
        // Resolve's custom options buttons can parent a menu without exposing it
        // through QPushButton::menu(). Prefer that explicit ownership relationship.
        int result = -1;
        for (auto parent = menu->parent(); parent; parent = parent->parent())
            if (auto widget = qobject_cast<QWidget *>(parent))
                if (optionsButton(widget)) { result = closestPanel(widget); break; }
        // Only menus directly attached to a panel's known options button qualify.
        for (auto window : QApplication::topLevelWidgets()) {
            for (auto button : window->findChildren<QAbstractButton *>()) {
                if (!optionsButton(button)) continue;
                QMenu *attached = nullptr;
                if (auto tool = qobject_cast<QToolButton *>(button)) attached = tool->menu();
                if (auto push = qobject_cast<QPushButton *>(button)) attached = push->menu();
                if (attached != menu) continue;
                const int index = closestPanel(button);
                if (index < 0) continue;
                if (result >= 0 && result != index) return -1;
                result = index;
            }
        }
        return result;
    }
    QList<Panel> panels;
    QSet<QAction *> wired;
    QSet<QMenu *> wiredMenus;
};
}
