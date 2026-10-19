// SPDX-License-Identifier: MIT
#pragma once
#include <QAbstractButton>
#include <functional>
#include <QLayout>
#include <QPointer>
#include <QSignalBlocker>
#include <QTimer>
#include <QToolButton>
#include "diagnostics.h"
namespace Unbound {
// Extra callers only. Placement and persisted state remain with panel owners.
class ToolbarDoors : public QObject {
public:
    struct Door { QString id,title;std::function<bool()> shown;std::function<void()> toggle; };
    explicit ToolbarDoors(QObject *owner):QObject(owner) {}
    ~ToolbarDoors() override { for (auto button:buttons) if (button) delete button; }
    void add(Door door) { doors.append(std::move(door)); }
    void wire(QWidget *surface) {
        if (!surface) return;
        for (auto row:surface->findChildren<QWidget *>()) if (row->objectName().startsWith("frameEditTopBarLeft") && row->layout())
            for (const auto &door:doors) {
                const auto id="openGateToolbarDoor_"+door.id;
                auto button=row->findChild<QToolButton *>(id,Qt::FindDirectChildrenOnly);
                if (!button) {
                    button=new QToolButton(row);button->setObjectName(id);button->setText(door.title);
                    button->setAutoRaise(true);button->setCheckable(true);button->setFocusPolicy(Qt::NoFocus);
                    button->setToolTip("Show or hide "+door.title+" in its remembered location");row->layout()->addWidget(button);buttons.append(button);
                    connect(button,&QToolButton::clicked,this,[this,door] {
                        QTimer::singleShot(0,this,[door] { logEvent("toolbar.panel-door",{{"panel",door.id}});if (door.toggle) door.toggle(); });
                    });
                }
                const QSignalBlocker blocked(button);button->setChecked(door.shown && door.shown());
            }
    }
private:
    QList<Door> doors;QList<QPointer<QToolButton>> buttons;
};
}
