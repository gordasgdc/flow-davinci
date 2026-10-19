// SPDX-License-Identifier: MIT
#pragma once
#include <QAbstractButton>
#include <QSet>
#include <QSignalBlocker>
#include <QWidget>
#include <functional>

namespace Unbound {
// Bridges native Resolve toggle signals to panel doors. Does not own or move
// Resolve widgets, and never writes checked state with native signals enabled.
class NativePanelToggles : public QObject {
public:
    explicit NativePanelToggles(QObject *owner) : QObject(owner) {}
    void add(QString toggle,std::function<bool()> active,std::function<bool()> hidden,
             std::function<void()> request)
    { entries.append({std::move(toggle),std::move(active),std::move(hidden),std::move(request)}); }
    void sync(int index,QWidget *root)
    {
        const auto &entry=entries[index];
        if (!root || entry.toggle.isEmpty() || !entry.active()) return;
        const auto prefix=QStringLiteral("btnEditToggle")+entry.toggle+"In";
        for (auto button:root->findChildren<QAbstractButton *>()) if (button->objectName().startsWith(prefix)) {
            const QSignalBlocker blocked(button); button->setChecked(!entry.hidden());
        }
    }
    void wire(QWidget *root)
    {
        if (!root) return;
        const auto buttons=root->findChildren<QAbstractButton *>();
        for (const auto &entry:entries) {
            if (entry.toggle.isEmpty()) continue;
            const auto prefix=QStringLiteral("btnEditToggle")+entry.toggle+"In";
            for (auto button:buttons) {
                if (!button->objectName().startsWith(prefix) || observed.contains(button)) continue;
                observed.insert(button);
                connect(button,&QObject::destroyed,this,[this,button] { observed.remove(button); });
                connect(button,&QAbstractButton::clicked,this,[request=entry.request] { request(); });
                connect(button,&QAbstractButton::toggled,this,[hidden=entry.hidden,request=entry.request](bool checked) {
                    if (checked!=!hidden()) request();
                });
            }
        }
    }
private:
    struct Entry { QString toggle; std::function<bool()> active,hidden; std::function<void()> request; };
    QList<Entry> entries;
    QSet<QAbstractButton *> observed;
};
}
