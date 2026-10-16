// SPDX-License-Identifier: MIT
#pragma once
#include <QMenu>
#include <QJsonArray>
#include <QPushButton>
#include <QToolButton>
#include <QTimer>
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
protected:
    bool eventFilter(QObject *object, QEvent *event) override
    {
        if (event->type() != QEvent::Show) return false;
        auto menu = qobject_cast<QMenu *>(object);
        if (!menu || menu->property("unboundOwnMenu").toBool()) return false;
        const int index = ownerOf(menu);
        if (index < 0) {
            for (auto command : menu->actions()) if (command->objectName() == QLatin1String("unboundDockPanel")) command->setVisible(false);
            QJsonArray parents;
            for (QObject *parent = menu->parent(); parent && parents.size() < 8; parent = parent->parent())
                parents.append(QJsonObject{{"name", parent->objectName()}, {"class", parent->metaObject()->className()}});
            logEvent(QStringLiteral("panel.menu.unmatched"), {{"name", menu->objectName()}, {"parents", parents}});
            return false;
        }
        auto &panel = panels[index];
        QAction *command = nullptr;
        for (auto action : menu->actions()) if (action->objectName() == QLatin1String("unboundDockPanel")) command = action;
        if (!command) {
            menu->addSeparator();
            command = menu->addAction(QString());
            command->setObjectName(QStringLiteral("unboundDockPanel"));
            connect(command, &QAction::triggered, this, [this, command] {
                const int index = command->property("unboundPanelIndex").toInt();
                // Let Resolve finish closing its menu before changing widget ownership.
                QTimer::singleShot(0, this, [this, index] {
                    logEvent(QStringLiteral("panel.menu.toggle"), {{"panel", panels[index].name}});
                    panels[index].toggle();
                });
            });
        }
        command->setProperty("unboundPanelIndex", index);
        command->setVisible(true);
        command->setText(panel.floating() ? QStringLiteral("Dock Panel") : QStringLiteral("Undock Panel"));
        logEvent(QStringLiteral("panel.menu.ready"), {{"panel", panel.name}, {"floating", panel.floating()}});
        return false;
    }
private:
    int ownerOf(QMenu *menu) const
    {
        int result = -1;
        // Only menus directly attached to a panel's known options button qualify.
        for (auto window : QApplication::topLevelWidgets()) {
            for (auto button : window->findChildren<QAbstractButton *>()) {
                const QString name = button->objectName();
                if (name != "btnMediaPoolOptions" && name != "btnTitleOptions" && name != "btnAudioMixerOptions" &&
                    name != "btnOptions" && name != "m_BtnDisplayOptions" && name != "btnEdlOptions" &&
                    name != "tbtnPlayerOptions" && name != "metadataSelectionMenuBtn" && name != "btnCurveSettingsOptions" &&
                    name != "btnEditToolbarPreferences") continue;
                QMenu *attached = nullptr;
                if (auto tool = qobject_cast<QToolButton *>(button)) attached = tool->menu();
                if (auto push = qobject_cast<QPushButton *>(button)) attached = push->menu();
                if (attached != menu) continue;
                for (auto parent = button->parentWidget(); parent; parent = parent->parentWidget())
                    for (int i = 0; i < panels.size(); ++i)
                        if (parent->objectName() == panels[i].name ||
                            parent->objectName() == QStringLiteral("unboundFloating_") + panels[i].name ||
                            parent->objectName() == QStringLiteral("unboundGroupCell_") + panels[i].name) {
                            if (result >= 0 && result != i) return -1;
                            result = i;
                        }
            }
        }
        return result;
    }
    QList<Panel> panels;
};
}
